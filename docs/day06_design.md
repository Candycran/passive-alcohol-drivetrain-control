# Day 06 — Vehicle Dynamics and Speed Feedback

## Why Day 06 exists

Day 04 demonstrated that the Arduino can progressively reduce the drivetrain command.

However, a zero propulsion command is not the same thing as zero vehicle speed. A moving vehicle has inertia and may continue coasting.

Day 06 closes that modelling gap with a one-dimensional longitudinal vehicle model.

## System boundary

This project still does not simulate a complete car.

The plant includes only:

- propulsion force;
- controlled braking force;
- rolling resistance;
- aerodynamic resistance;
- vehicle mass/inertia;
- longitudinal speed.

## Longitudinal equation

`m a = F_drive - F_brake - F_rolling - F_aero`

The acceleration result is integrated to obtain vehicle speed.

## Safety intervention

Before intervention, driver demand passes through normally.

After the confirmed event:

1. the maximum permitted propulsion progressively falls;
2. controlled brake force progressively rises;
3. vehicle speed is calculated from the net longitudinal force;
4. a speed-feedback signal determines when the vehicle has actually stopped;
5. drivetrain lockout is considered physically complete only after speed is at or below the stop threshold.

## Important distinction

Arduino/Tinkercad demonstrates the embedded decision and actuator-command logic.

MATLAB/Simulink represents the vehicle dynamics and physical speed feedback.

Together they form the complete simulation architecture for the project.
