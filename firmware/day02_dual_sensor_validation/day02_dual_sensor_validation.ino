const byte PIN_ALCOHOL = A0;
const byte PIN_CO2 = A1;
const byte PIN_ACCELERATOR = A2;

const byte PIN_MOTOR_ENABLE = 9;
const byte PIN_MOTOR_IN1 = 8;
const byte PIN_MOTOR_IN2 = 7;
const byte PIN_GREEN_LED = 4;
const byte PIN_RED_LED = 5;
const byte PIN_BUZZER = 6;

const unsigned long SAMPLE_INTERVAL_MS = 100;
const unsigned long PRINT_INTERVAL_MS = 250;
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
const unsigned long CONFIRMATION_TIME_MS = 3000;

bool alcoholActive = false;
bool breathActive = false;
bool candidateTiming = false;

unsigned long candidateStartTime = 0;
unsigned long lastSampleTime = 0;
unsigned long lastPrintTime = 0;

enum DetectionState {
  SAFE,
  ALCOHOL_ONLY,
  BREATH_ONLY,
  CANDIDATE,
  ALCOHOL_CONFIRMED
};

DetectionState detectionState = SAFE;
DetectionState previousDetectionState = SAFE;

int analogToPercent(int raw) {
  return map(raw, 0, 1023, 0, 100);
}

void initialiseFilter() {
  int firstAlcohol = analogRead(PIN_ALCOHOL);
  int firstCo2 = analogRead(PIN_CO2);

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

  sampleIndex = (sampleIndex + 1) % FILTER_SIZE;
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

void updateDetectionState() {
  previousDetectionState = detectionState;

  if (alcoholActive && breathActive) {
    if (!candidateTiming) {
      candidateTiming = true;
      candidateStartTime = millis();
    }

    if (millis() - candidateStartTime >= CONFIRMATION_TIME_MS) {
      detectionState = ALCOHOL_CONFIRMED;
    } else {
      detectionState = CANDIDATE;
    }
  } else {
    candidateTiming = false;

    if (alcoholActive) {
      detectionState = ALCOHOL_ONLY;
    } else if (breathActive) {
      detectionState = BREATH_ONLY;
    } else {
      detectionState = SAFE;
    }
  }

  if (detectionState == ALCOHOL_CONFIRMED &&
      previousDetectionState != ALCOHOL_CONFIRMED) {
    tone(PIN_BUZZER, 1200, 300);
  }
}

void updateIndicators() {
  if (detectionState == ALCOHOL_CONFIRMED) {
    digitalWrite(PIN_GREEN_LED, LOW);
    digitalWrite(PIN_RED_LED, HIGH);
  } else if (detectionState == CANDIDATE) {
    digitalWrite(PIN_GREEN_LED, HIGH);
    digitalWrite(PIN_RED_LED, ((millis() / 250) % 2) ? HIGH : LOW);
  } else {
    digitalWrite(PIN_GREEN_LED, HIGH);
    digitalWrite(PIN_RED_LED, LOW);
  }
}

const char* stateName(int state) {
  switch (state) {
    case SAFE: return "SAFE";
    case ALCOHOL_ONLY: return "ALCOHOL_ONLY";
    case BREATH_ONLY: return "BREATH_ONLY";
    case CANDIDATE: return "CANDIDATE";
    case ALCOHOL_CONFIRMED: return "ALCOHOL_CONFIRMED";
    default: return "UNKNOWN";
  }
}

unsigned long candidateElapsedMs() {
  if (!candidateTiming) return 0;
  unsigned long elapsed = millis() - candidateStartTime;
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

  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);
  digitalWrite(PIN_GREEN_LED, HIGH);
  digitalWrite(PIN_RED_LED, LOW);

  initialiseFilter();

  Serial.println("DAY 02 STARTED");
  Serial.println("Alcohol%,CO2%,Accel%,PWM,AlcoholActive,BreathActive,State,ConfirmMs");
}

void loop() {
  unsigned long now = millis();

  if (now - lastSampleTime >= SAMPLE_INTERVAL_MS) {
    lastSampleTime = now;

    sampleSensors();

    int alcoholPercent = filteredAlcoholPercent();
    int co2Percent = filteredCo2Percent();

    updateHysteresis(alcoholPercent, co2Percent);
    updateDetectionState();
  }

  int acceleratorPercent = analogToPercent(analogRead(PIN_ACCELERATOR));
  int motorPwm = map(acceleratorPercent, 0, 100, 0, 255);

  // DAY 02 RULE:
  // Detection is observed, but does not yet override propulsion.
  analogWrite(PIN_MOTOR_ENABLE, motorPwm);

  updateIndicators();

  if (now - lastPrintTime >= PRINT_INTERVAL_MS) {
    lastPrintTime = now;

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
    Serial.print(stateName(detectionState));
    Serial.print(",");
    Serial.println(candidateElapsedMs());
  }
}
