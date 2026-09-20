/*
  Day 01 Baseline
  Passive Dual-Sensor Driver Alcohol Detection and Controlled
  Drivetrain Inhibit System

  Purpose:
  - Read alcohol, CO2/breath, and accelerator analog inputs
  - Prove accelerator-to-motor PWM control
  - Prove LEDs and buzzer outputs
  - Print live engineering values to Serial Monitor

  IMPORTANT:
  Alcohol intervention logic is intentionally NOT enabled on Day 01.
  We first verify the basic sensing and actuation path.
*/

const byte PIN_ALCOHOL = A0;
const byte PIN_CO2 = A1;
const byte PIN_ACCELERATOR = A2;

const byte PIN_MOTOR_ENABLE = 9;  // PWM
const byte PIN_MOTOR_IN1 = 8;
const byte PIN_MOTOR_IN2 = 7;

const byte PIN_GREEN_LED = 4;
const byte PIN_RED_LED = 5;
const byte PIN_BUZZER = 6;

unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL_MS = 250;

int readPercent(byte pin) {
  int raw = analogRead(pin);
  return map(raw, 0, 1023, 0, 100);
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_MOTOR_ENABLE, OUTPUT);
  pinMode(PIN_MOTOR_IN1, OUTPUT);
  pinMode(PIN_MOTOR_IN2, OUTPUT);

  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Forward-only drivetrain direction for the baseline simulation.
  digitalWrite(PIN_MOTOR_IN1, HIGH);
  digitalWrite(PIN_MOTOR_IN2, LOW);

  // Baseline status: system powered and no safety intervention implemented yet.
  digitalWrite(PIN_GREEN_LED, HIGH);
  digitalWrite(PIN_RED_LED, LOW);
  noTone(PIN_BUZZER);

  analogWrite(PIN_MOTOR_ENABLE, 0);

  Serial.println("Day 01 baseline started.");
  Serial.println("Alcohol%, CO2%, Accelerator%, MotorPWM");
}

void loop() {
  int alcoholPercent = readPercent(PIN_ALCOHOL);
  int co2Percent = readPercent(PIN_CO2);
  int acceleratorPercent = readPercent(PIN_ACCELERATOR);

  // Convert accelerator demand (0-100%) to Arduino PWM (0-255).
  int motorPwm = map(acceleratorPercent, 0, 100, 0, 255);

  // Day 01: accelerator directly controls drivetrain.
  // Safety override is added only after this baseline is verified.
  analogWrite(PIN_MOTOR_ENABLE, motorPwm);

  if (millis() - lastPrintTime >= PRINT_INTERVAL_MS) {
    lastPrintTime = millis();

    Serial.print(alcoholPercent);
    Serial.print(", ");
    Serial.print(co2Percent);
    Serial.print(", ");
    Serial.print(acceleratorPercent);
    Serial.print(", ");
    Serial.println(motorPwm);
  }
}
