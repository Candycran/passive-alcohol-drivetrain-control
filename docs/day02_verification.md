# Day 02 Verification Checklist

Use the same physical/simulated wiring as Day 01.

## Test A - Safe
Alcohol about 10%, CO2 about 10%.
Expected state: SAFE.

## Test B - Breath only
Alcohol below 50%, CO2 above 55%.
Expected state: BREATH_ONLY.

## Test C - Alcohol only
Alcohol above 60%, CO2 below 45%.
Expected state: ALCOHOL_ONLY.

## Test D - Short candidate
Set alcohol above 60% and CO2 above 55%, but remove one condition before 3 seconds.
Expected: CANDIDATE, then reset. It must not become ALCOHOL_CONFIRMED.

## Test E - Confirmed condition
Hold alcohol above 60% and CO2 above 55% continuously for more than 3 seconds.
Expected:
- CANDIDATE
- red LED blinks
- after about 3 seconds: ALCOHOL_CONFIRMED
- green LED off
- red LED steady
- short buzzer tone

## Test F - Hysteresis
After alcohol becomes active, lower it to about 56%.
Expected: it stays active.
Lower it to 50% or less.
Expected: it becomes inactive.

Repeat for CO2: activate at 55%, release at 45%.

## Test G - Motor independence
Keep accelerator around 70% while cycling through all sensing states.
Expected: motor speed continues to follow accelerator. Day 02 must not intervene in propulsion.

## Evidence
Capture screenshots of:
- SAFE
- ALCOHOL_ONLY
- BREATH_ONLY
- CANDIDATE
- ALCOHOL_CONFIRMED
- Serial Monitor showing ConfirmMs increasing to 3000

Day 02 passes only when all states behave correctly.
