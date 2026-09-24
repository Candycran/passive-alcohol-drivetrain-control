# Day 04 - Controlled Drivetrain Slowdown and Lockout

Day 04 connects the validated alcohol event to drivetrain control.

The controller now tracks three motor values: Requested PWM (driver demand), Limit PWM (maximum command allowed by the safety controller), and Allowed PWM (the command actually sent to the motor driver).

Before confirmation, Allowed PWM equals Requested PWM. During controlled slowdown, Allowed PWM equals the smaller of Requested PWM and Limit PWM. During drivetrain lockout, Allowed PWM is zero.

This means the driver can always request less propulsion than the safety controller permits, but cannot request more once intervention begins.

The new CONTROLLED_SLOWDOWN state begins after the alcohol-plus-breath condition survives the full verification period. The Limit PWM starts at the current requested value and is reduced by 10 counts every 300 ms. These are simulation parameters only, not real-vehicle deceleration settings.

Once the limit reaches zero, the controller enters DRIVETRAIN_LOCKOUT. Monitoring electronics remain active, but propulsion is inhibited.

Day 04 controls motor command, not measured vehicle speed. Zero PWM means zero commanded propulsion, not proof that a real vehicle has physically stopped. Vehicle dynamics are represented later in the project using MATLAB/Simulink.
