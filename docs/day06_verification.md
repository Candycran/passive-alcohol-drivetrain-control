# Day 06 Verification

## MATLAB test

Run:

`day06_vehicle_response`

Expected:

- three figures are created;
- a CSV file is saved;
- vehicle speed decreases after intervention begins;
- safety limit falls progressively;
- allowed command cannot exceed safety limit;
- braking force increases progressively;
- lockout becomes true only when speed reaches the stop threshold.

## Simulink tests

### Test 1 — Pre-intervention

Run the model.

Before 5 s:

- intervention progress = 0;
- safety limitation is inactive;
- controlled brake force = 0;
- vehicle remains moving.

### Test 2 — Progressive intervention

From 5 s to 11 s:

- safety limit decreases smoothly;
- allowed propulsion decreases;
- brake request increases;
- vehicle speed falls progressively rather than instantaneously.

### Test 3 — Zero propulsion is not yet lockout

At about 11 s:

- propulsion limit reaches 0;
- vehicle speed should still be above zero.

This is a critical Day 06 result.

It proves why speed feedback is needed.

### Test 4 — Physical stop feedback

Continue simulation.

Expected:

- speed eventually crosses <= 0.10 m/s;
- Vehicle Stopped becomes true;
- lockout can now be considered physically complete.

## Evidence to save

1. Simulink block diagram.
2. Vehicle-speed Scope.
3. Safety-limit / allowed-command Scope.
4. Brake-force Scope.
5. Vehicle Stopped signal.
6. MATLAB vehicle speed plot.
7. MATLAB safety arbitration plot.
8. CSV result file.

## Pass condition

Day 06 passes when the model demonstrates that:

- intervention is progressive;
- driver demand is restricted by the safety controller;
- the vehicle does not unrealistically stop the instant propulsion reaches zero;
- speed feedback independently confirms the physical stop.
