# Day 06 Codex Master Prompt

You are working inside the existing `passive-driver-alcohol-drivetrain-system` repository.

Do not redesign the project and do not modify Day 01-Day 05 firmware.

Create the Day 06 MATLAB/Simulink vehicle-response layer.

The project is a simulation-only Arduino Uno passive alcohol detection and controlled drivetrain inhibit system. Day 05 already handles sensing, state-machine logic, controlled PWM reduction, EEPROM lockout, and recovery.

Day 06 must solve the physical modelling gap that zero PWM does not imply zero vehicle speed.

Create a MATLAB script named `matlab/day06_vehicle_response.m` using a simple one-dimensional longitudinal vehicle model:

`m*a = F_drive - F_brake - F_rolling - F_aero`

Use these simulation parameters:
- dt = 0.01 s
- total simulation = 25 s
- mass = 1200 kg
- gravity = 9.81 m/s^2
- air density = 1.225 kg/m^3
- Cd = 0.30
- frontal area = 2.2 m^2
- rolling resistance coefficient = 0.015
- max propulsion force = 700 N
- max controlled intervention brake force = 1800 N
- initial vehicle speed = 20 m/s
- driver demand = 0.65
- intervention begins at 5 s
- safety propulsion limit ramps from 0.65 to 0 over 6 seconds
- controlled brake request ramps from 0 to maxBrakeForce over the same period
- AllowedCommand = min(DriverDemand, SafetyLimit)
- speed must never become negative
- stopped threshold = 0.10 m/s
- lockout feedback becomes true only once speed is at/below the threshold after intervention

The script must:
- calculate speed over time;
- report stopping time;
- save a CSV;
- generate separate plots for vehicle speed, safety arbitration, and brake force;
- clearly label all axes and units;
- state in comments that values are simulation parameters, not production calibration.

Also create clear documentation for manually reproducing the same architecture in Simulink with standard blocks such as Constant, Ramp, Saturation, Gain, MinMax, Sum, Math Function, Integrator, Compare To Constant, and Scope.

Do not claim PWM equals speed.
Do not simulate a complete vehicle.
Do not add unrelated features.
Keep the model understandable to someone learning from the project.
