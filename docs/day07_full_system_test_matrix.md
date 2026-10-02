# Full-System Test Matrix

FT-01 Normal startup -> STARTING -> WARMING -> MONITORING
FT-02 Safe sensing -> no intervention
FT-03 Breath only -> BREATH_ONLY
FT-04 Alcohol only -> ALCOHOL_ONLY
FT-05 Short dual-sensor event -> verification cancels
FT-06 Sustained dual-sensor event -> intervention latches
FT-07 Accelerator increase during slowdown -> cannot exceed safety limit
FT-08 Accelerator reduction -> lower request respected
FT-09 Sensors become safe after confirmation -> intervention continues
FT-10 Propulsion reaches zero -> vehicle may still be moving
FT-11 Speed <= 0.10 m/s -> Vehicle Stopped TRUE
FT-12 Intervention Active AND Vehicle Stopped -> Lockout Eligible TRUE
FT-13 EEPROM lockout stored
FT-14 Reset while locked -> propulsion remains disabled
FT-15 Unsafe recovery -> fails
FT-16 Interrupted recovery -> timer resets
FT-17 Breath-only recovery -> timer continues
FT-18 Sustained alcohol-safe recovery -> monitoring restored
