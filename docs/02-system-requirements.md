# 02 - System Requirements

## Functional requirements

FR-01: The controller shall begin monitoring automatically when powered.

FR-02: The controller shall accept an analog alcohol-sensor signal.

FR-03: The controller shall accept an analog CO2/breath-confirmation signal.

FR-04: The controller shall accept an analog accelerator-demand signal.

FR-05: A high alcohol signal alone shall not immediately trigger drivetrain intervention.

FR-06: Alcohol intervention shall require a sustained alcohol condition together with a qualifying breath-confirmation condition.

FR-07: During safe operation, accelerator demand shall control the drivetrain command.

FR-08: When alcohol is confirmed while moving, the controller shall progressively reduce the permitted drivetrain command rather than commanding an instantaneous stop.

FR-09: When simulated vehicle speed reaches zero after intervention, the controller shall inhibit further propulsion.

FR-10: Warning outputs shall indicate intervention and lockout states.

FR-11: A completed lockout shall eventually be persisted in Arduino EEPROM so a controller restart cannot immediately restore propulsion.

FR-12: Propulsion shall be restored only after the sensing condition remains safe for the defined verification period.

## Simulation constraints

SR-01: The project shall not simulate an entire vehicle.

SR-02: A DC motor shall represent the drivetrain/wheel system in the embedded simulation.

SR-03: Adjustable analog sources may be used as sensor proxies when an exact sensor is unavailable in the free simulator.

SR-04: Sensor values are simulation indices and shall not be presented as legally calibrated BAC values.

SR-05: MATLAB/Simulink shall be used for dynamic vehicle-speed modelling and controlled-deceleration analysis.
