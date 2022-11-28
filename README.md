# Amplifier Trigger Hub
Use 12V trigger from Denon AVR to switch NAD C352 and Behringer A500 on and off together.

## Detect Denon AVR power state
The Denon AVR has a 12V trigger output, that is high when it's on and low when it's off. Trigger is read through a voltage divider (30kΩ - 20kΩ, giving 12V -> ~4.8V) on `TRIGGER_PIN`.

## Switching NAD on / off
The NAD has an external IR in, which takes Extended NEC IR commands (without a carrier wave). Just connect a 3.5mm jack from the external IR input on the NAD to `IR_PIN`.

The IR signal is sent with [this direct NEC transmitter library](https://github.com/dennisfrett/Arduino-Direct-NEC-Transmitter).

## Switching Behringer on / off
There is no external control possible on this amplifier, so I use a Sonoff Mini relay to switch the power. Just connect a 2N2222 between S1 and S2 on the Sonoff (S1 = ground, connect to emitter. S2 = pulled up to 3.3, connect to collector. Base connects to `RELAY_PIN` through a 1kΩ resistor).


