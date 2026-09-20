# Day 01 Verification Checklist

Complete these tests before adding alcohol intervention logic.

## Test 1 - Alcohol input
- Keep accelerator at zero.
- Move the alcohol potentiometer from minimum to maximum.
- Confirm Serial Monitor alcohol value changes approximately from 0 to 100.
- Motor must remain controlled only by accelerator.

## Test 2 - CO2 input
- Move the CO2 potentiometer.
- Confirm Serial Monitor CO2 value changes approximately from 0 to 100.
- Motor behaviour must not change yet.

## Test 3 - Accelerator
- Set alcohol and CO2 anywhere.
- Increase accelerator from 0 to 100.
- Confirm MotorPWM changes from approximately 0 to 255.
- Confirm DC motor speed visibly increases.

## Test 4 - Minimum accelerator
- Set accelerator to zero.
- Motor must stop.

## Test 5 - Status outputs
- Green LED must remain ON.
- Red LED must remain OFF.
- Buzzer must remain silent.

## Pass condition
Day 01 passes when all three analog inputs are independently readable and accelerator demand reliably controls drivetrain PWM.

## Evidence to capture
1. Screenshot of complete Tinkercad circuit.
2. Screenshot of Serial Monitor with at least three different input values.
3. Screenshot or short recording showing motor speed responding to accelerator.
