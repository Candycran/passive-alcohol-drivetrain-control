# Passive Dual-Sensor Driver Alcohol Detection and Controlled Drivetrain Inhibit System

## Project status
Simulation-based mechatronics portfolio project using an Arduino Uno control prototype and a MATLAB/Simulink vehicle-response model.

## Core concept
The system passively monitors two simulated sensing signals located in the driver's breathing zone:

1. Alcohol-vapour signal
2. CO2/exhaled-breath confirmation signal

The driver does not perform a deliberate breath test. During normal operation, accelerator demand is passed to the drivetrain. When a sustained alcohol condition is confirmed together with a breath signal, the controller overrides the driver's requested propulsion, progressively reduces drivetrain command until vehicle speed reaches zero, and then enters a persistent drivetrain-lockout state.

The vehicle electrical/monitoring system remains powered during and after the intervention.

## Simulation architecture
- Arduino Uno: embedded controller
- Analog alcohol input: simulated sensor output
- Analog CO2 input: simulated breath-confirmation output
- Analog accelerator input: simulated pedal position
- L293D: DC motor driver
- DC motor: drivetrain representation
- LEDs and buzzer: driver warning/status
- MATLAB/Simulink: dynamic vehicle-speed response and controlled-deceleration analysis

## Repository structure
- `firmware/` Arduino firmware
- `docs/` engineering documentation
- `simulink/` MATLAB/Simulink model files
- `results/` plots and simulation evidence

## Day 01 objective
Build and verify the basic input/output path:
sensor proxies -> Arduino -> motor driver -> DC motor.

Day 01 does not yet implement alcohol intervention. Its purpose is to prove that all analog inputs are readable, accelerator demand can control drivetrain PWM, and status outputs function correctly.
