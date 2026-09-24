/*
  Day 04 - Controlled Drivetrain Slowdown and Lockout
  Simulation/portfolio controller only. PWM ramp values are simulation
  parameters and are NOT suitable for controlling a real vehicle.
  PWM is a motor command, not measured vehicle speed. Intervention stays
  latched until reset; no EEPROM persistence is implemented.
*/

const byte PIN_ALCOHOL = A0;
const byte PIN_CO2 = A1;
const byte PIN_ACCELERATOR = A2;
const byte PIN_MOTOR_ENABLE = 9;
const byte PIN_MOTOR_IN1 = 8;
const byte PIN_MOTOR_IN2 = 7;
const byte PIN_GREEN_LED = 4;
const byte PIN_RED_LED = 5;
const byte PIN_BUZZER = 6;

const byte STATE_STARTING = 0;
const byte STATE_WARMING = 1;
const byte STATE_MONITORING = 2;
const byte STATE_VERIFYING = 3;
const byte STATE_CONTROLLED_SLOWDOWN = 4;
const byte STATE_DRIVETRAIN_LOCKOUT = 5;
byte systemState = STATE_STARTING;

const byte CONDITION_SAFE = 0;
const byte CONDITION_ALCOHOL_ONLY = 1;
const byte CONDITION_BREATH_ONLY = 2;
const byte CONDITION_ALCOHOL_AND_BREATH = 3;
byte sensorCondition = CONDITION_SAFE;

const unsigned long STARTING_TIME_MS = 1500;
const unsigned long WARMING_TIME_MS = 5000;
const unsigned long CONFIRMATION_TIME_MS = 3000;
const unsigned long SAMPLE_INTERVAL_MS = 100;
const unsigned long PRINT_INTERVAL_MS = 250;
const unsigned long SLOWDOWN_STEP_INTERVAL_MS = 300;
const int SLOWDOWN_STEP_PWM = 10;

unsigned long stateStartTime = 0;
unsigned long verificationStartTime = 0;
unsigned long lastSampleTime = 0;
unsigned long lastPrintTime = 0;
unsigned long lastSlowdownStepTime = 0;
unsigned long lastWarningBeepTime = 0;

const byte FILTER_SIZE = 10;
int alcoholSamples[FILTER_SIZE];
int co2Samples[FILTER_SIZE];
long alcoholSum = 0;
long co2Sum = 0;
byte sampleIndex = 0;

const int ALCOHOL_HIGH_THRESHOLD = 60;
const int ALCOHOL_LOW_THRESHOLD  = 50;
const int CO2_HIGH_THRESHOLD = 55;
const int CO2_LOW_THRESHOLD  = 45;

bool alcoholActive = false;
bool breathActive = false;

int requestedMotorPwm = 0;
int allowedMotorPwm = 0;
int slowdownLimitPwm = 255;

int analogToPercent(int rawValue) {
  return map(rawValue, 0, 1023, 0, 100);
}

void initialiseFilter() {
  int firstAlcohol = analogRead(PIN_ALCOHOL);
  int firstCo2 = analogRead(PIN_CO2);
  alcoholSum = 0;
  co2Sum = 0;
  sampleIndex = 0;

  for (byte i = 0; i < FILTER_SIZE; i++) {
    alcoholSamples[i] = firstAlcohol;
    co2Samples[i] = firstCo2;
    alcoholSum += firstAlcohol;
    co2Sum += firstCo2;
  }
}

void sampleSensors() {
  int newAlcohol = analogRead(PIN_ALCOHOL);
  int newCo2 = analogRead(PIN_CO2);

  alcoholSum -= alcoholSamples[sampleIndex];
  co2Sum -= co2Samples[sampleIndex];
  alcoholSamples[sampleIndex] = newAlcohol;
  co2Samples[sampleIndex] = newCo2;
  alcoholSum += newAlcohol;
  co2Sum += newCo2;

  sampleIndex++;
  if (sampleIndex >= FILTER_SIZE) sampleIndex = 0;
}

int filteredAlcoholPercent() {
  return analogToPercent(alcoholSum / FILTER_SIZE);
}

int filteredCo2Percent() {
  return analogToPercent(co2Sum / FILTER_SIZE);
}

void updateHysteresis(int alcoholPercent, int co2Percent) {
  if (!alcoholActive && alcoholPercent >= ALCOHOL_HIGH_THRESHOLD) alcoholActive = true;
  else if (alcoholActive && alcoholPercent <= ALCOHOL_LOW_THRESHOLD) alcoholActive = false;

  if (!breathActive && co2Percent >= CO2_HIGH_THRESHOLD) breathActive = true;
  else if (breathActive && co2Percent <= CO2_LOW_THRESHOLD) breathActive = false;
}

void classifySensorCondition() {
  if (alcoholActive && breathActive) sensorCondition = CONDITION_ALCOHOL_AND_BREATH;
  else if (alcoholActive) sensorCondition = CONDITION_ALCOHOL_ONLY;
  else if (breathActive) sensorCondition = CONDITION_BREATH_ONLY;
  else sensorCondition = CONDITION_SAFE;
}

void enterState(byte newState) {
  unsigned long now = millis();
  systemState = newState;
  stateStartTime = now;

  if (newState == STATE_VERIFYING) verificationStartTime = now;

  if (newState == STATE_CONTROLLED_SLOWDOWN) {
    // Capture demand once. Later accelerator increases cannot raise this ceiling.
    slowdownLimitPwm = requestedMotorPwm;
    lastSlowdownStepTime = now;
    lastWarningBeepTime = now;
    if (slowdownLimitPwm == 0) {
      enterState(STATE_DRIVETRAIN_LOCKOUT);
      return;
    }
    tone(PIN_BUZZER, 1200, 400);
  }

  if (newState == STATE_DRIVETRAIN_LOCKOUT) {
    slowdownLimitPwm = 0;
    tone(PIN_BUZZER, 900, 600);
  }
}

void updateStateMachine() {
  unsigned long now = millis();

  switch (systemState) {
    case STATE_STARTING:
      if (now - stateStartTime >= STARTING_TIME_MS) enterState(STATE_WARMING);
      break;

    case STATE_WARMING:
      if (now - stateStartTime >= WARMING_TIME_MS) enterState(STATE_MONITORING);
      break;

    case STATE_MONITORING:
      if (sensorCondition == CONDITION_ALCOHOL_AND_BREATH) enterState(STATE_VERIFYING);
      break;

    case STATE_VERIFYING:
      if (sensorCondition != CONDITION_ALCOHOL_AND_BREATH) enterState(STATE_MONITORING);
      else if (now - verificationStartTime >= CONFIRMATION_TIME_MS) enterState(STATE_CONTROLLED_SLOWDOWN);
      break;

    case STATE_CONTROLLED_SLOWDOWN:
      // No sensor-based exit: confirmation latches the intervention.
      if (now - lastSlowdownStepTime >= SLOWDOWN_STEP_INTERVAL_MS) {
        lastSlowdownStepTime = now;
        slowdownLimitPwm -= SLOWDOWN_STEP_PWM;
        if (slowdownLimitPwm <= 0) {
          slowdownLimitPwm = 0;
          enterState(STATE_DRIVETRAIN_LOCKOUT);
          break;  // Preserve the lockout tone; do not issue another warning.
        }
      }

      if (now - lastWarningBeepTime >= 1000) {
        lastWarningBeepTime = now;
        tone(PIN_BUZZER, 1200, 150);
      }
      break;

    case STATE_DRIVETRAIN_LOCKOUT:
      // Terminal until reset. loop() continues sampling and printing diagnostics.
      break;
  }
}

void updateMotorControl() {
  // A lower demand is always allowed, but cannot change the safety ceiling.
  if (systemState == STATE_CONTROLLED_SLOWDOWN) {
    allowedMotorPwm = min(requestedMotorPwm, slowdownLimitPwm);
  } else if (systemState == STATE_DRIVETRAIN_LOCKOUT) {
    allowedMotorPwm = 0;
  } else {
    allowedMotorPwm = requestedMotorPwm;
  }

  analogWrite(PIN_MOTOR_ENABLE, allowedMotorPwm);
}

void updateStatusIndicators() {
  unsigned long now = millis();

  switch (systemState) {
    case STATE_STARTING:
      digitalWrite(PIN_GREEN_LED, LOW);
      digitalWrite(PIN_RED_LED, LOW);
      break;
    case STATE_WARMING:
      digitalWrite(PIN_GREEN_LED, ((now / 500) % 2) ? HIGH : LOW);
      digitalWrite(PIN_RED_LED, LOW);
      break;
    case STATE_MONITORING:
      digitalWrite(PIN_GREEN_LED, HIGH);
      digitalWrite(PIN_RED_LED, LOW);
      break;
    case STATE_VERIFYING:
      digitalWrite(PIN_GREEN_LED, HIGH);
      digitalWrite(PIN_RED_LED, ((now / 250) % 2) ? HIGH : LOW);
      break;
    case STATE_CONTROLLED_SLOWDOWN:
      digitalWrite(PIN_GREEN_LED, LOW);
      digitalWrite(PIN_RED_LED, ((now / 150) % 2) ? HIGH : LOW);
      break;
    case STATE_DRIVETRAIN_LOCKOUT:
      digitalWrite(PIN_GREEN_LED, LOW);
      digitalWrite(PIN_RED_LED, HIGH);
      break;
  }
}

const char* systemStateName(int state) {
  switch (state) {
    case STATE_STARTING: return "STARTING";
    case STATE_WARMING: return "WARMING";
    case STATE_MONITORING: return "MONITORING";
    case STATE_VERIFYING: return "VERIFYING";
    case STATE_CONTROLLED_SLOWDOWN: return "CONTROLLED_SLOWDOWN";
    case STATE_DRIVETRAIN_LOCKOUT: return "DRIVETRAIN_LOCKOUT";
    default: return "UNKNOWN";
  }
}

const char* sensorConditionName(int condition) {
  switch (condition) {
    case CONDITION_SAFE: return "SAFE";
    case CONDITION_ALCOHOL_ONLY: return "ALCOHOL_ONLY";
    case CONDITION_BREATH_ONLY: return "BREATH_ONLY";
    case CONDITION_ALCOHOL_AND_BREATH: return "ALCOHOL_AND_BREATH";
    default: return "UNKNOWN";
  }
}

unsigned long verificationElapsedMs() {
  if (systemState != STATE_VERIFYING) return 0;
  unsigned long elapsed = millis() - verificationStartTime;
  return elapsed > CONFIRMATION_TIME_MS ? CONFIRMATION_TIME_MS : elapsed;
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_MOTOR_ENABLE, OUTPUT);
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  analogWrite(PIN_MOTOR_ENABLE, 0);
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_GREEN_LED, LOW);
  digitalWrite(PIN_RED_LED, LOW);

  initialiseFilter();
  stateStartTime = millis();

  Serial.println("State,Condition,Alcohol%,CO2%,Accelerator%,RequestedPWM,LimitPWM,AllowedPWM,VerifyMs");
}

void loop() {
  unsigned long now = millis();

  int acceleratorPercent = analogToPercent(analogRead(PIN_ACCELERATOR));
  requestedMotorPwm = map(acceleratorPercent, 0, 100, 0, 255);

  if (now - lastSampleTime >= SAMPLE_INTERVAL_MS) {
    lastSampleTime = now;
    sampleSensors();
    updateHysteresis(filteredAlcoholPercent(), filteredCo2Percent());
    classifySensorCondition();
  }

  updateStateMachine();
  updateMotorControl();
  updateStatusIndicators();

  if (now - lastPrintTime >= PRINT_INTERVAL_MS) {
    lastPrintTime = now;

    Serial.print(systemStateName(systemState)); Serial.print(",");
    Serial.print(sensorConditionName(sensorCondition)); Serial.print(",");
    Serial.print(filteredAlcoholPercent()); Serial.print(",");
    Serial.print(filteredCo2Percent()); Serial.print(",");
    Serial.print(acceleratorPercent); Serial.print(",");
    Serial.print(requestedMotorPwm); Serial.print(",");
    Serial.print(slowdownLimitPwm); Serial.print(",");
    Serial.print(allowedMotorPwm); Serial.print(",");
    Serial.println(verificationElapsedMs());
  }
}
