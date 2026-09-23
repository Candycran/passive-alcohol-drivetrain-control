# Day 03 Verification Checklist

Use the exact Day 02 circuit. No rewiring is required.

1. Restart: state must begin as STARTING for about 1.5 s.
2. WARMING: green LED blinks slowly for about 5 s; alcohol readings must not trigger VERIFYING yet.
3. MONITORING: green LED steady, red off.
4. BREATH_ONLY: CO2 above 55%, alcohol below 50%; state remains MONITORING.
5. ALCOHOL_ONLY: alcohol above 60%, CO2 below 45%; state remains MONITORING.
6. VERIFYING: alcohol above 60% and CO2 above 55%; red blinks and VerifyMs rises.
7. Cancel: drop either validated signal before 3 s; state must return to MONITORING.
8. Confirm: hold both high for more than 3 s; state becomes ALCOHOL_CONFIRMED, red steady, green off, short buzzer tone.
9. Latch: lower both signals after confirmation; state must remain ALCOHOL_CONFIRMED until restart.
10. Motor independence: in all Day 03 states, motor must still follow the accelerator.

Save screenshots for STARTING, WARMING, MONITORING, VERIFYING, ALCOHOL_CONFIRMED, and the latched confirmed state.
