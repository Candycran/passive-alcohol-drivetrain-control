# Day 03 - System State Machine and Sensor Warm-Up

## Objective
Day 03 introduces a system-level state machine so the Arduino behaves differently depending on the current operating phase.

## States
- STARTING: 1.5 s controller boot phase.
- WARMING: 5 s simulated sensor stabilisation period. Sensor readings are sampled but not trusted for detection decisions.
- MONITORING: normal continuous dual-sensor monitoring.
- VERIFYING: alcohol and breath are simultaneously active; a 3 s persistence timer runs.
- ALCOHOL_CONFIRMED: the 3 s verification succeeded. This state is latched until restart on Day 03.

## Sensor condition vs system state
These are separate concepts. For example, the system can be MONITORING while the sensor condition is BREATH_ONLY or ALCOHOL_ONLY. This separation makes the controller easier to test and extend.

## Important Day 03 rule
The DC motor still follows accelerator demand in every state. No drivetrain intervention is enabled yet.

## Simulation timing
The 5 s warm-up is intentionally compressed for simulation. It is not a claim about the warm-up requirement of a real MQ-series sensor.
