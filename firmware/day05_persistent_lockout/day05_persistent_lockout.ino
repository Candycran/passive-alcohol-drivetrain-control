#include <EEPROM.h>

const byte PIN_ALCOHOL=A0, PIN_CO2=A1, PIN_ACCELERATOR=A2;
const byte PIN_MOTOR_ENABLE=9, PIN_MOTOR_IN1=8, PIN_MOTOR_IN2=7;
const byte PIN_GREEN_LED=4, PIN_RED_LED=5, PIN_BUZZER=6;

const int EEPROM_LOCKOUT_ADDRESS=0;
const byte EEPROM_LOCKOUT_MAGIC=0xA5;
const byte EEPROM_CLEAR_VALUE=0x00;
bool persistentLockout=false;

const byte STATE_STARTING=0;
const byte STATE_WARMING=1;
const byte STATE_MONITORING=2;
const byte STATE_VERIFYING=3;
const byte STATE_CONTROLLED_SLOWDOWN=4;
const byte STATE_DRIVETRAIN_LOCKOUT=5;
const byte STATE_RECOVERY_VERIFYING=6;
byte systemState=STATE_STARTING;

const byte CONDITION_SAFE=0;
const byte CONDITION_ALCOHOL_ONLY=1;
const byte CONDITION_BREATH_ONLY=2;
const byte CONDITION_ALCOHOL_AND_BREATH=3;
byte sensorCondition=CONDITION_SAFE;

const unsigned long STARTING_TIME_MS=1500;
const unsigned long WARMING_TIME_MS=5000;
const unsigned long CONFIRMATION_TIME_MS=3000;
const unsigned long RECOVERY_SAFE_TIME_MS=5000;
const unsigned long SAMPLE_INTERVAL_MS=100;
const unsigned long PRINT_INTERVAL_MS=250;
const unsigned long SLOWDOWN_STEP_INTERVAL_MS=300;
const int SLOWDOWN_STEP_PWM=10;

unsigned long stateStartTime=0, verificationStartTime=0, recoveryStartTime=0;
unsigned long lastSampleTime=0, lastPrintTime=0, lastSlowdownStepTime=0, lastWarningBeepTime=0;
bool recoveryTiming=false;

const byte FILTER_SIZE=10;
int alcoholSamples[FILTER_SIZE], co2Samples[FILTER_SIZE];
long alcoholSum=0, co2Sum=0;
byte sampleIndex=0;

const int ALCOHOL_HIGH_THRESHOLD=60, ALCOHOL_LOW_THRESHOLD=50;
const int CO2_HIGH_THRESHOLD=55, CO2_LOW_THRESHOLD=45;
bool alcoholActive=false, breathActive=false;

int requestedMotorPwm=0, slowdownLimitPwm=255, allowedMotorPwm=0;

int analogToPercent(int rawValue){ return map(rawValue,0,1023,0,100); }

void initialiseFilter(){
  int firstAlcohol=analogRead(PIN_ALCOHOL);
  int firstCo2=analogRead(PIN_CO2);
  alcoholSum=0; co2Sum=0;
  for(byte i=0;i<FILTER_SIZE;i++){
    alcoholSamples[i]=firstAlcohol;
    co2Samples[i]=firstCo2;
    alcoholSum+=firstAlcohol;
    co2Sum+=firstCo2;
  }
}

void sampleSensors(){
  int newAlcohol=analogRead(PIN_ALCOHOL);
  int newCo2=analogRead(PIN_CO2);
  alcoholSum-=alcoholSamples[sampleIndex];
  co2Sum-=co2Samples[sampleIndex];
  alcoholSamples[sampleIndex]=newAlcohol;
  co2Samples[sampleIndex]=newCo2;
  alcoholSum+=newAlcohol;
  co2Sum+=newCo2;
  sampleIndex++;
  if(sampleIndex>=FILTER_SIZE) sampleIndex=0;
}

int filteredAlcoholPercent(){ return analogToPercent(alcoholSum/FILTER_SIZE); }
int filteredCo2Percent(){ return analogToPercent(co2Sum/FILTER_SIZE); }

void updateHysteresis(int alcoholPercent,int co2Percent){
  if(!alcoholActive && alcoholPercent>=ALCOHOL_HIGH_THRESHOLD) alcoholActive=true;
  else if(alcoholActive && alcoholPercent<=ALCOHOL_LOW_THRESHOLD) alcoholActive=false;

  if(!breathActive && co2Percent>=CO2_HIGH_THRESHOLD) breathActive=true;
  else if(breathActive && co2Percent<=CO2_LOW_THRESHOLD) breathActive=false;
}

void classifySensorCondition(){
  if(alcoholActive && breathActive) sensorCondition=CONDITION_ALCOHOL_AND_BREATH;
  else if(alcoholActive) sensorCondition=CONDITION_ALCOHOL_ONLY;
  else if(breathActive) sensorCondition=CONDITION_BREATH_ONLY;
  else sensorCondition=CONDITION_SAFE;
}

void savePersistentLockout(){
  EEPROM.update(EEPROM_LOCKOUT_ADDRESS,EEPROM_LOCKOUT_MAGIC);
  persistentLockout=true;
}

void clearPersistentLockout(){
  EEPROM.update(EEPROM_LOCKOUT_ADDRESS,EEPROM_CLEAR_VALUE);
  persistentLockout=false;
}

void enterState(byte newState){
  systemState=newState;
  stateStartTime=millis();

  if(newState==STATE_VERIFYING) verificationStartTime=millis();

  if(newState==STATE_CONTROLLED_SLOWDOWN){
    slowdownLimitPwm=requestedMotorPwm;
    lastSlowdownStepTime=millis();
    lastWarningBeepTime=0;
    tone(PIN_BUZZER,1200,400);
  }

  if(newState==STATE_DRIVETRAIN_LOCKOUT){
    slowdownLimitPwm=0;
    savePersistentLockout();
    tone(PIN_BUZZER,900,600);
  }

  if(newState==STATE_RECOVERY_VERIFYING){
    recoveryTiming=false;
    recoveryStartTime=0;
  }
}

void updateStateMachine(){
  unsigned long now=millis();

  switch(systemState){
    case STATE_STARTING:
      if(now-stateStartTime>=STARTING_TIME_MS) enterState(STATE_WARMING);
      break;

    case STATE_WARMING:
      if(now-stateStartTime>=WARMING_TIME_MS){
        if(persistentLockout) enterState(STATE_RECOVERY_VERIFYING);
        else enterState(STATE_MONITORING);
      }
      break;

    case STATE_MONITORING:
      if(sensorCondition==CONDITION_ALCOHOL_AND_BREATH) enterState(STATE_VERIFYING);
      break;

    case STATE_VERIFYING:
      if(sensorCondition!=CONDITION_ALCOHOL_AND_BREATH) enterState(STATE_MONITORING);
      else if(now-verificationStartTime>=CONFIRMATION_TIME_MS) enterState(STATE_CONTROLLED_SLOWDOWN);
      break;

    case STATE_CONTROLLED_SLOWDOWN:
      if(now-lastSlowdownStepTime>=SLOWDOWN_STEP_INTERVAL_MS){
        lastSlowdownStepTime=now;
        slowdownLimitPwm-=SLOWDOWN_STEP_PWM;
        if(slowdownLimitPwm<=0){
          slowdownLimitPwm=0;
          enterState(STATE_DRIVETRAIN_LOCKOUT);
        }
      }
      if(now-lastWarningBeepTime>=1000){
        lastWarningBeepTime=now;
        tone(PIN_BUZZER,1200,150);
      }
      break;

    case STATE_DRIVETRAIN_LOCKOUT:
      break;

    case STATE_RECOVERY_VERIFYING:
      if(!alcoholActive){
        if(!recoveryTiming){
          recoveryTiming=true;
          recoveryStartTime=now;
        }
        if(now-recoveryStartTime>=RECOVERY_SAFE_TIME_MS){
          clearPersistentLockout();
          recoveryTiming=false;
          tone(PIN_BUZZER,1500,250);
          enterState(STATE_MONITORING);
        }
      } else {
        recoveryTiming=false;
        recoveryStartTime=0;
      }
      break;
  }
}

void updateMotorControl(){
  if(persistentLockout) allowedMotorPwm=0;
  else if(systemState==STATE_CONTROLLED_SLOWDOWN) allowedMotorPwm=min(requestedMotorPwm,slowdownLimitPwm);
  else if(systemState==STATE_DRIVETRAIN_LOCKOUT || systemState==STATE_RECOVERY_VERIFYING) allowedMotorPwm=0;
  else allowedMotorPwm=requestedMotorPwm;

  analogWrite(PIN_MOTOR_ENABLE,allowedMotorPwm);
}

void updateStatusIndicators(){
  unsigned long now=millis();

  switch(systemState){
    case STATE_STARTING:
      digitalWrite(PIN_GREEN_LED,LOW);
      digitalWrite(PIN_RED_LED,persistentLockout?HIGH:LOW);
      break;

    case STATE_WARMING:
      if(persistentLockout){
        digitalWrite(PIN_GREEN_LED,LOW);
        digitalWrite(PIN_RED_LED,((now/300)%2)?HIGH:LOW);
      } else {
        digitalWrite(PIN_GREEN_LED,((now/500)%2)?HIGH:LOW);
        digitalWrite(PIN_RED_LED,LOW);
      }
      break;

    case STATE_MONITORING:
      digitalWrite(PIN_GREEN_LED,HIGH);
      digitalWrite(PIN_RED_LED,LOW);
      break;

    case STATE_VERIFYING:
      digitalWrite(PIN_GREEN_LED,HIGH);
      digitalWrite(PIN_RED_LED,((now/250)%2)?HIGH:LOW);
      break;

    case STATE_CONTROLLED_SLOWDOWN:
      digitalWrite(PIN_GREEN_LED,LOW);
      digitalWrite(PIN_RED_LED,((now/150)%2)?HIGH:LOW);
      break;

    case STATE_DRIVETRAIN_LOCKOUT:
      digitalWrite(PIN_GREEN_LED,LOW);
      digitalWrite(PIN_RED_LED,HIGH);
      break;

    case STATE_RECOVERY_VERIFYING:
      digitalWrite(PIN_GREEN_LED,((now/250)%2)?HIGH:LOW);
      digitalWrite(PIN_RED_LED,HIGH);
      break;
  }
}

const char* systemStateName(int state){
  switch(state){
    case STATE_STARTING:return "STARTING";
    case STATE_WARMING:return "WARMING";
    case STATE_MONITORING:return "MONITORING";
    case STATE_VERIFYING:return "VERIFYING";
    case STATE_CONTROLLED_SLOWDOWN:return "CONTROLLED_SLOWDOWN";
    case STATE_DRIVETRAIN_LOCKOUT:return "DRIVETRAIN_LOCKOUT";
    case STATE_RECOVERY_VERIFYING:return "RECOVERY_VERIFYING";
    default:return "UNKNOWN";
  }
}

const char* sensorConditionName(int condition){
  switch(condition){
    case CONDITION_SAFE:return "SAFE";
    case CONDITION_ALCOHOL_ONLY:return "ALCOHOL_ONLY";
    case CONDITION_BREATH_ONLY:return "BREATH_ONLY";
    case CONDITION_ALCOHOL_AND_BREATH:return "ALCOHOL_AND_BREATH";
    default:return "UNKNOWN";
  }
}

unsigned long verificationElapsedMs(){
  if(systemState!=STATE_VERIFYING) return 0;
  unsigned long elapsed=millis()-verificationStartTime;
  return elapsed>CONFIRMATION_TIME_MS?CONFIRMATION_TIME_MS:elapsed;
}

unsigned long recoveryElapsedMs(){
  if(systemState!=STATE_RECOVERY_VERIFYING || !recoveryTiming) return 0;
  unsigned long elapsed=millis()-recoveryStartTime;
  return elapsed>RECOVERY_SAFE_TIME_MS?RECOVERY_SAFE_TIME_MS:elapsed;
}

void setup(){
  Serial.begin(9600);

  pinMode(PIN_MOTOR_ENABLE,OUTPUT);
  pinMode(PIN_MOTOR_IN1,OUTPUT);
  pinMode(PIN_MOTOR_IN2,OUTPUT);
  pinMode(PIN_GREEN_LED,OUTPUT);
  pinMode(PIN_RED_LED,OUTPUT);
  pinMode(PIN_BUZZER,OUTPUT);

  digitalWrite(PIN_MOTOR_IN1,HIGH);
  digitalWrite(PIN_MOTOR_IN2,LOW);
  digitalWrite(PIN_GREEN_LED,LOW);
  digitalWrite(PIN_RED_LED,LOW);

  initialiseFilter();

  persistentLockout=(EEPROM.read(EEPROM_LOCKOUT_ADDRESS)==EEPROM_LOCKOUT_MAGIC);
  stateStartTime=millis();

  Serial.println("DAY 05 STARTED");
  Serial.print("Stored lockout detected: ");
  Serial.println(persistentLockout?"YES":"NO");
  Serial.println("State,Condition,Alcohol%,CO2%,Accel%,RequestedPWM,LimitPWM,AllowedPWM,Lockout,VerifyMs,RecoveryMs");
}

void loop(){
  unsigned long now=millis();

  int acceleratorPercent=analogToPercent(analogRead(PIN_ACCELERATOR));
  requestedMotorPwm=map(acceleratorPercent,0,100,0,255);

  if(now-lastSampleTime>=SAMPLE_INTERVAL_MS){
    lastSampleTime=now;
    sampleSensors();
    updateHysteresis(filteredAlcoholPercent(),filteredCo2Percent());
    classifySensorCondition();
  }

  updateStateMachine();
  updateMotorControl();
  updateStatusIndicators();

  if(now-lastPrintTime>=PRINT_INTERVAL_MS){
    lastPrintTime=now;
    Serial.print(systemStateName(systemState)); Serial.print(",");
    Serial.print(sensorConditionName(sensorCondition)); Serial.print(",");
    Serial.print(filteredAlcoholPercent()); Serial.print(",");
    Serial.print(filteredCo2Percent()); Serial.print(",");
    Serial.print(acceleratorPercent); Serial.print(",");
    Serial.print(requestedMotorPwm); Serial.print(",");
    Serial.print(slowdownLimitPwm); Serial.print(",");
    Serial.print(allowedMotorPwm); Serial.print(",");
    Serial.print(persistentLockout?1:0); Serial.print(",");
    Serial.print(verificationElapsedMs()); Serial.print(",");
    Serial.println(recoveryElapsedMs());
  }
}
