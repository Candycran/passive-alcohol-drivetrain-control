Implement Day 05 of the existing Arduino Uno passive alcohol drivetrain project without modifying Day 01–04.

Preserve pins, moving-average filtering, thresholds/hysteresis, state timings, and Day 04 slowdown logic.

Add Arduino EEPROM persistence:
- include EEPROM.h
- address 0
- lockout magic 0xA5
- clear value 0x00
- read marker in setup
- save marker with EEPROM.update() when DRIVETRAIN_LOCKOUT is entered
- a stored lockout must force AllowedPWM=0 during STARTING and WARMING
- after warm-up with a stored lockout, enter RECOVERY_VERIFYING
- recovery succeeds only when alcoholActive remains false continuously for 5000 ms
- BREATH_ONLY must not block recovery
- any alcohol reactivation resets the recovery timer
- after successful recovery clear EEPROM and enter MONITORING

Keep millis()-based non-blocking timing. Do not use delay(). Do not add hardware. Do not treat PWM as measured vehicle speed. Use integer state constants for Tinkercad compatibility.

Serial columns:
State,Condition,Alcohol%,CO2%,Accel%,RequestedPWM,LimitPWM,AllowedPWM,Lockout,VerifyMs,RecoveryMs
