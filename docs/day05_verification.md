# Day 05 Verification

1. Reach `DRIVETRAIN_LOCKOUT`. Confirm `AllowedPWM=0` and `Lockout=1`.
2. While Tinkercad is still running, press the red RESET button on the Arduino. Do not stop the simulation.
3. Confirm startup prints `Stored lockout detected: YES`.
4. Set accelerator to 100%. Confirm `AllowedPWM=0` during STARTING and WARMING.
5. After warm-up, confirm `RECOVERY_VERIFYING`.
6. Keep alcohol high: `RecoveryMs` must remain 0.
7. Lower alcohol below its release threshold: `RecoveryMs` must increase.
8. Raise alcohol before 5000 ms: the timer must reset.
9. Test `BREATH_ONLY`: alcohol low, CO2 high. `RecoveryMs` must continue increasing.
10. Keep alcohol inactive for >5 s. Confirm `Lockout=0`, state becomes `MONITORING`, and accelerator control returns.
11. Press RESET again. Confirm startup prints `Stored lockout detected: NO`.

Save screenshots for lockout, reset-with-lockout, recovery timer, interrupted recovery, and successful unlock.
