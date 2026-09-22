# Day 02 - Dual-Sensor Processing and Breath Validation

## Objective

Day 02 turns the alcohol and CO2 inputs into a validated detection decision without yet allowing that decision to interfere with propulsion.

## Inputs

- A0: simulated alcohol signal
- A1: simulated CO2/exhaled-breath signal
- A2: simulated accelerator

## Filtering

Alcohol and CO2 are sampled every 100 ms. The controller stores 10 samples and uses their moving average, representing about one second of recent data.

## Hysteresis

Alcohol activates at 60% and does not deactivate until 50% or below.

CO2/breath activates at 55% and does not deactivate until 45% or below.

This prevents noisy readings close to one threshold from repeatedly switching the decision on and off.

## Detection states

SAFE: neither signal is active.

ALCOHOL_ONLY: alcohol is elevated without breath confirmation.

BREATH_ONLY: breath/CO2 is elevated without alcohol.

CANDIDATE: both signals are active, but the required persistence time has not elapsed.

ALCOHOL_CONFIRMED: both signals have remained active continuously for 3000 ms.

The threshold values are simulation indices only. They are not BAC, BrAC, or calibrated ppm measurements.

## Day 02 propulsion rule

The accelerator continues to control the DC motor directly. Even ALCOHOL_CONFIRMED does not reduce motor speed today. Day 02 exists to prove that the sensing decision itself works correctly before that decision gains authority over propulsion.
