# smart-plate-heater

A weight-based, smart plate heater for ease of use and safety

- The posh wooden plate that the user sees !

![nice plateau](images/plateau-bois-dessus-face.jpg)

- The back "measuring foot"
  - holding all the wieght in a 3-points-of-contact setup
  - maximum 5kg vertically on the back, so an average of 10kg in center
  - the back feet do nothing except prevent tumbling

![measuring foot](images/plateau-bois-arriere-pieds.jpg)

- due to the sensitivity of the voltages used for measuring using a strain gauge, the pre-processing must be done locally... plus the panel can be done locally too and everything fits in the 8 wires of the network cable

![the innards](images/plateau-bois-dessous.jpg)

The target "perfboard pcb", with only 3 additional wires (solid core, 26 AWG, ptfe isolated wires)

![perfboard front](images/perfboard-front.jpg)

Only 3 shrink fit insulation patches, where i forgot to make multi-level "manhattan-style" routing

![perfboard back](images/perfboard-back.jpg)

## Schematic and "perfboard" PCB

The way the main board works and AC safety is done in the derivation box

![perfboard schematic](images/perfboard-schematic.jpg)

I always prepare my perfboard routing on a appropriately sized PCB

![perfboard pcb](images/perfboard-pcb.png)

The way the appliance and user panel is wired

![appliance schematic](images/appliance-schematic.jpg)

## Hardware

Target board on perfboard (uses 8mA@3V3 at idle and 80ma@3V3 when relay is on)

- PIC 10F200-family microcontroller (here a 202 in SOP-23 a DIP breakout board) from rs-online.com

![PIC 10F20x](images/pic10f202-sot23-to-dip.jpg)

- 500mA 5x20 fuse and fuse holder (oversized, the whole thing draws less than 0.35 W at full load)

![Fuse holder](images/fuse-holder.jpg)

- varistor 10D561K (transient voltage suppressor TVS) ot protect the circuit from grid surges

![Varistor](images/varistor-tvs-10d561k.jpg)

- HLK-PM03 3W 240VAC to 3.3VDC converter (isolated)

![HLK-PM03 AC to DC converter](images/hi-link-pm03.jpg)

- HX711 breakout board (purple with holes for fixturing

![HX711 differential ADC for strain gauges](images/hx711-purple-with-holes.jpg)

- Beam-type load cell (maximum 5KG)

![Beam load cell 5KG](images/beam-type-load-cell-5kg.jpg)

- 3.3V relay board (active low, floating high)

![3.3V relay board](images/relay_3v3_active_low.jpg)

- network port breakout board, and a network cable to cut

![Network socket on a breakout board](images/network-port-without-holes.jpg)

- normally open temporary button, single pole single throw (SPST) rocker switch...
- screw terminals, 1/4W resistors, 5mm LED, headers,dc caps, 2N3904 NPN transitor...

For prototyping

![dev prototyping](images/esp32-dev-with-perfboard.jpg)

- breadboard and dupont cables
- ESP32 DevKit 38 pins

![ESP32 DevKit 38 pins](images/esp32-devkit-38pins.jpg)

- logic analyzer (salae clone)

![Logic analyzer](images/logic-analyzer-salae-clone.jpg)

Real wafeforms results in PulseView software from ESP prototype

- Macro waveforms to see sampling rate of HX711 breakout board (1 sample per 0.4 second = 2.5 Hz)

![waveforms macro view](images/esp32-pulseview-macro.jpg)

- Micro waveforms to check the clock and data timings (data transfer time is 115 micro-seconds)

![waveforms micro view](images/esp32-pulseview-micro.jpg)

## Pinouts

Using a PIC10F202 as microcontroller

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

- [Pre-compiled HEX file for a PIC10F202 microcontroller](mplab.X/pic10f202-production.hex)
- which you can program using my homemade [Arduino-UNO-based simple pic programmer](https://github.com/nipil/arduino-simple-pic-programmer)

### Debugging

Go to `Tools/Options/Embedded`

- debug reset `reset vector`
- debug startup `halt at reset vector`
- activate stimulus (*.scl) file
- use simulator to debug

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
