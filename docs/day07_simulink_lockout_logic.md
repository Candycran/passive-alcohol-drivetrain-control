# Final Simulink Lockout Logic

1. Branch `Intervention Progress`.
2. Add `Compare To Constant`.
3. Set Operator to `>` and Constant to `0`.
4. Rename it `Intervention Active`.

Then:
1. Add `Logical Operator`.
2. Set Operator to `AND`.
3. Set number of inputs to `2`.
4. Connect `Intervention Active` to input 1.
5. Connect `Vehicle Stopped` to input 2.
6. Rename the block `Lockout Eligible`.
7. Add a Display to the output and rename it `Lockout Status`.

Expected:
- before intervention: 0
- during slowdown while moving: 0
- after speed <= 0.10 m/s: 1
