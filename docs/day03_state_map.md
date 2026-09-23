# Day 03 State Transition Map

STARTING
  |
  | after 1.5 s
  v
WARMING
  |
  | after 5 s
  v
MONITORING
  |
  | alcohol active AND breath active
  v
VERIFYING
  |                         |
  | condition disappears    | both remain active for 3 s
  |                         |
  +------> MONITORING        v
                      ALCOHOL_CONFIRMED
                              |
                              | latched on Day 03
                              v
                        restart required

The motor still follows accelerator demand in every Day 03 state.
