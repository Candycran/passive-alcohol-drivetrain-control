# 01 - Locked Project Concept

The project is a passive dual-sensor driver alcohol detection and controlled drivetrain inhibit system.

The driver is not asked to blow into a mouthpiece or perform a deliberate breath test. A concealed sensing unit is conceptually located in the driver's breathing zone. The system continuously evaluates an alcohol-vapour signal together with a CO2/exhaled-breath confirmation signal.

A single high reading does not trigger intervention. The controller will use repeated samples and persistence checks before classifying the alcohol condition as confirmed.

During normal operation, accelerator demand is allowed to control the simulated drivetrain. If a sustained high alcohol condition is confirmed together with a breath signal while the drivetrain is moving, the Arduino overrides accelerator demand and progressively reduces the allowed motor command. The system does not abruptly lock a moving wheel.

When simulated vehicle speed reaches zero, the drivetrain enters a lockout state. Monitoring electronics remain powered, but propulsion is inhibited. A lockout will eventually be stored in Arduino EEPROM so a simple power cycle cannot bypass it. Drive authorization will return only after the system verifies a sustained safe sensing condition.

The Arduino/Tinkercad layer represents sensing, embedded control, warnings, and drivetrain command. MATLAB/Simulink will represent the dynamic vehicle-speed response and feedback behaviour.
