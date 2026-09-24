# Day 04 State Map

STARTING -> WARMING -> MONITORING -> VERIFYING

If verification fails: VERIFYING -> MONITORING

If verification succeeds: VERIFYING -> CONTROLLED_SLOWDOWN -> DRIVETRAIN_LOCKOUT

CONTROLLED_SLOWDOWN and DRIVETRAIN_LOCKOUT do not return to MONITORING merely because sensor values fall.
