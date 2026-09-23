/*
  Day 03 - System State Machine and Sensor Warm-Up
  Passive Dual-Sensor Driver Alcohol Detection and Controlled
  Drivetrain Inhibit System

  SYSTEM STATES
  STARTING -> WARMING -> MONITORING -> VERIFYING -> ALCOHOL_CONFIRMED

  Day 03 rule: the motor still follows the accelerator in ALL states.
  Drivetrain intervention begins on Day 04.
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

// Integer state constants are used for Tinkercad compatibility.
const byte STATE_STARTING = 0;
const byte STATE_WARMING = 1;
const byte STATE_MONITORING = 2;
const byte STATE_VERIFYING = 3;
const byte STATE_ALCOHOL_CONFIRMED = 4;
byte systemState = STATE_STARTING;

const byte CONDITION_SAFE = 0;
const byte CONDITION_ALCOHOL_ONLY = 1;
const byte CONDITION_BREATH_ONLY = 2;
const byte CONDITION_ALCOHOL_AND_BREATH = 3;
byte sensorCondition = CONDITION_SAFE;

const unsigned long STARTING_TIME_MS = 1500;
const unsigned long WARMING_TIME_MS = 5000;   // shortened for simulation
const unsigned long CONFIRMATION_TIME_MS = 3000;
const unsigned long SAMPLE_INTERVAL_MS = 100;
const unsigned long PRINT_INTERVAL_MS = 250;

unsigned long stateStartTime = 0;
unsigned long verificationStartTime = 0;
unsigned long lastSampleTime = 0;
unsigned long lastPrintTime = 0;

const byte FILTER_SIZE = 10;
int alcoholSamples[FILTER_SIZE];
int co2Samples[FILTER_SIZE];
long alcoholSum = 0;
long co2Sum = 0;
byte sampleIndex = 0;

const int ALCOHOL_HIGH_THRESHOLD = 60;
const int ALCOHOL_LOW_THRESHOLD = 50;
const int CO2_HIGH_THRESHOLD = 55;
const int CO2_LOW_THRESHOLD = 45;

bool alcoholActive = false;
bool breathActive = false;

int analogToPercent(int rawValue) {
  return map(rawValue, 0, 1023, 0, 100);
}

void initialiseFilter() {
  int firstAlcohol = analogRead(PIN_ALCOHOL);
  int firstCo2 = analogRead(PIN_CO2);

  alcoholSum = 0;
  co2Sum = 0;

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
  if (!alcoholActive && alcoholPercent >= ALCOHOL_HIGH_THRESHOLD) {
    alcoholActive = true;
  } else if (alcoholActive && alcoholPercent <= ALCOHOL_LOW_THRESHOLD) {
    alcoholActive = false;
  }

  if (!breathActive && co2Percent >= CO2_HIGH_THRESHOLD) {
    breathActive = true;
  } else if (breathActive && co2Percent <= CO2_LOW_THRESHOLD) {
    breathActive = false;
  }
}

void classifySensorCondition() {
  if (alcoholActive && breathActive) {
    sensorCondition = CONDITION_ALCOHOL_AND_BREATH;
  } else if (alcoholActive && !breathActive) {
    sensorCondition = CONDITION_ALCOHOL_ONLY;
  } else if (!alcoholActive && breathActive) {
    sensorCondition = CONDITION_BREATH_ONLY;
  } else {
    sensorCondition = CONDITION_SAFE;
  }
}

void enterState(byte newState) {
  systemState = newState;
  stateStartTime = millis();

  if (newState == STATE_VERIFYING) {
    verificationStartTime = millis();
  }

  if (newState == STATE_ALCOHOL_CONFIRMED) {
    tone(PIN_BUZZER, 1200, 400);
  }
}

void updateStateMachine() {
  unsigned long now = millis();

  switch (systemState) {
    case STATE_STARTING:
      if (now - stateStartTime >= STARTING_TIME_MS) {
        enterState(STATE_WARMING);
      }
      break;

    case STATE_WARMING:
      if (now - stateStartTime >= WARMING_TIME_MS) {
        enterState(STATE_MONITORING);
      }
      break;

    case STATE_MONITORING:
      if (sensorCondition == CONDITION_ALCOHOL_AND_BREATH) {
        enterState(STATE_VERIFYING);
      }
      break;

    case STATE_VERIFYING:
      if (sensorCondition != CONDITION_ALCOHOL_AND_BREATH) {
        enterState(STATE_MONITORING);
      } else if (now - verificationStartTime >= CONFIRMATION_TIME_MS) {
        enterState(STATE_ALCOHOL_CONFIRMED);
      }
      break;

    case STATE_ALCOHOL_CONFIRMED:
      // Latched until restart on Day 03.
      break;
  }
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

    case STATE_ALCOHOL_CONFIRMED:
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
    case STATE_ALCOHOL_CONFIRMED: return "ALCOHOL_CONFIRMED";
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
  if (elapsed > CONFIRMATION_TIME_MS) elapsed = CONFIRMATION_TIME_MS;
  return elapsed;
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_MOTOR_ENABLE, OUTPUT);
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);
  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_GREEN_LED, LOW);
  digitalWrite(PIN_RED_LED, LOW);

  initialiseFilter();
  stateStartTime = millis();

  Serial.println("DAY 03 STARTED");
  Serial.println("State,Condition,Alcohol%,CO2%,Accel%,PWM,AlcoholActive,BreathActive,VerifyMs");
}

void loop() {
  unsigned long now = millis();

  if (now - lastSampleTime >= SAMPLE_INTERVAL_MS) {
    lastSampleTime = now;
    sampleSensors();

    int alcoholPercent = filteredAlcoholPercent();
    int co2Percent = filteredCo2Percent();

    updateHysteresis(alcoholPercent, co2Percent);
    classifySensorCondition();
  }

  updateStateMachine();

  // Day 03: no propulsion override yet.
  int acceleratorPercent = analogToPercent(analogRead(PIN_ACCELERATOR));
  int motorPwm = map(acceleratorPercent, 0, 100, 0, 255);
  analogWrite(PIN_MOTOR_ENABLE, motorPwm);

  updateStatusIndicators();

  if (now - lastPrintTime >= PRINT_INTERVAL_MS) {
    lastPrintTime = now;

    Serial.print(systemStateName(systemState));
    Serial.print(",");
    Serial.print(sensorConditionName(sensorCondition));
    Serial.print(",");
    Serial.print(filteredAlcoholPercent());
    Serial.print(",");
    Serial.print(filteredCo2Percent());
    Serial.print(",");
    Serial.print(acceleratorPercent);
    Serial.print(",");
    Serial.print(motorPwm);
    Serial.print(",");
    Serial.print(alcoholActive ? 1 : 0);
    Serial.print(",");
    Serial.print(breathActive ? 1 : 0);
    Serial.print(",");
    Serial.println(verificationElapsedMs());
  }
}
