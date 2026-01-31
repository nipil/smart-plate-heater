# smart-plate-heater

A weight-based, smart plate heater for ease of use and safety

## Pinouts

Target perfboard using PIC10F202 SOT-23-to-DIP pinout

    PIN 1 = GP0 = OUT_HEAT  PIN 6 = GP3 = IN_BUTTON
    PIN 2 = VSS (-)         PIN 5 = VDD (+)
    PIN 3 = GP1 = OUT_CLK   PIN 4 = GP2 = IN_DATA

Arduino prototype on ESP32 DevKit pinout

    G14 = OUT_HEAT
    G25 = OUT_CLOCK
    G13 = IN_DATA
    G34 = IN_BUTTON

## kicad folder

Design for electronics and target solderboard in a box

## mplab.x folder

PIC IDE, and C code built with XC8 compiler to generate HEX file

### Debugging

Go to `Tools/Options/Embedded`

- debug reset `reset vector`
- debug startup `halt at reset vector`

### Chip fuses

Go to `Window/Target Memory View/Configuration bits`

- disable MCLRE to be able to use GP3

## arduino-esp-prototype-software folder

This is an archived folder for an arduino framework, ESP32-devboard-based prototype

Achieved goals :

- verify that everything works on 3.3V
- discover raw values for "empty" aparatus
- discover raw values for "edge" beam strain gage
- discover effect of the weight of the cable (!)
- discover the "one kilogram on center" desired threshold
- verify light / transistor / relay working when desired
- verify values upon cable disconnection
- verify disconnection disabling
- verify negative/zero HX711 values disabling
- verify reset conditions disabling
- verify heat triggering by button when above threshold
- verify heat disabling when going under threshold
