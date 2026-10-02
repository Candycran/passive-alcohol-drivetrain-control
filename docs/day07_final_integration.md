# Day 07 — Final Integration

Final behavior:
1. STARTING
2. WARMING
3. MONITORING
4. VERIFYING
5. CONTROLLED_SLOWDOWN
6. Vehicle speed falls in MATLAB/Simulink
7. Vehicle Stopped becomes TRUE at speed <= 0.10 m/s
8. Lockout becomes physically eligible only when intervention is active AND Vehicle Stopped is TRUE
9. EEPROM-based persistent lockout is handled by the Arduino layer
10. Safe recovery requires sustained alcohol-inactive sensing

Arduino/Tinkercad and MATLAB/Simulink are verified as coordinated simulation layers, not live co-simulation.
