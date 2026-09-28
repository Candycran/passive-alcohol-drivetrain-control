# Day 05 — Persistent Lockout and Safe Recovery

When controlled slowdown reaches zero, the Arduino stores a lockout marker (`0xA5`) at EEPROM address 0. On reset, the firmware reads that marker before normal operation.

If a stored lockout exists, motor output is forced to zero during STARTING and WARMING. After warm-up, the system enters `RECOVERY_VERIFYING`, not `MONITORING`.

Recovery requires the alcohol condition to remain inactive continuously for five seconds. `BREATH_ONLY` is permitted because normal breathing without alcohol is not an alcohol event. If alcohol becomes active before five seconds, the recovery timer resets.

After five continuous safe seconds, the firmware clears the EEPROM marker and returns to `MONITORING`.

`EEPROM.update()` is used so the cell is written only when its stored value actually needs to change.
