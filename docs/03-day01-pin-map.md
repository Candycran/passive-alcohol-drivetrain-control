# 03 - Day 01 Pin Map

| Signal | Arduino Uno pin | Day 01 simulated component |
|---|---|---|
| Alcohol sensor signal | A0 | 10k potentiometer |
| CO2/breath signal | A1 | 10k potentiometer |
| Accelerator demand | A2 | 10k potentiometer |
| Motor PWM / L293D Enable 1 | D9 | L293D pin 1 |
| Motor direction A | D8 | L293D pin 2 |
| Motor direction B | D7 | L293D pin 7 |
| Green SAFE LED | D4 | LED + 220 ohm resistor |
| Red STATUS LED | D5 | LED + 220 ohm resistor |
| Buzzer | D6 | Piezo buzzer |

## L293D connections used on Day 01

- Pin 1 (1,2EN) -> Arduino D9
- Pin 2 (1A) -> Arduino D8
- Pin 3 (1Y) -> DC motor terminal 1
- Pin 4 -> GND
- Pin 5 -> GND
- Pin 6 (2Y) -> DC motor terminal 2
- Pin 7 (2A) -> Arduino D7
- Pin 8 (VCC2) -> motor supply
- Pin 16 (VCC1) -> 5V logic supply
- L293D grounds -> common GND

The Day 01 firmware drives the motor in one direction only. D8 is held HIGH and D7 LOW. Speed is controlled by PWM on D9.
