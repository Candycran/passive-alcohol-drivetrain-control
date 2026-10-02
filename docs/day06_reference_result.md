# Day 06 Reference Result

Using the supplied simulation parameters, the reference numerical model reaches the stop threshold at approximately:

**20.21 seconds simulation time**

The intervention begins at:

**5.00 seconds**

Therefore the simulated stopping interval after intervention begins is approximately:

**15.21 seconds**

Your MATLAB result should be close to this reference. Minor numerical differences are acceptable if your solver/settings differ slightly.

The important engineering result is not the exact number. It is that the propulsion command reaches zero **before** the physical vehicle speed reaches zero, demonstrating why independent speed feedback is required.
