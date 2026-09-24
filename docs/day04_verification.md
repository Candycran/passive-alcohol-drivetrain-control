# Day 04 Verification

1. Reach MONITORING with low alcohol/CO2 and accelerator around 70%. RequestedPWM and AllowedPWM should match.
2. Trigger VERIFYING, then lower one sensor before 3 seconds. The system should return to MONITORING.
3. Trigger a sustained event. State should progress to CONTROLLED_SLOWDOWN.
4. Keep accelerator fixed. LimitPWM and AllowedPWM should fall progressively; the motor should visibly slow.
5. During slowdown, increase accelerator to 100%. RequestedPWM should rise, but AllowedPWM must remain capped by LimitPWM.
6. Reduce accelerator below LimitPWM. AllowedPWM should follow the lower requested value.
7. Lower alcohol and CO2 after slowdown starts. Intervention must continue; it is latched.
8. Allow LimitPWM to reach zero. State should become DRIVETRAIN_LOCKOUT and AllowedPWM should remain 0.
9. Set accelerator to 100% during lockout. Motor must remain stopped.
10. Restart simulation. Day 04 currently begins again at STARTING; persistent restart lockout is not implemented yet.

Capture screenshots of MONITORING, VERIFYING, early slowdown, mid-slowdown with RequestedPWM > AllowedPWM, 100% accelerator during slowdown, and DRIVETRAIN_LOCKOUT.
