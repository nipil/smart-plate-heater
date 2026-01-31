/* 
 * File:   main.c
 * Author: nipil
 *
 * Created on 17 janvier 2026, 22:03
 */

// PIC10F202 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config WDTE = OFF       // Watchdog Timer (WDT disabled)
#pragma config CP = OFF         // Code Protect (Code protection off)
#pragma config MCLRE = OFF      // Master Clear Enable (GP3/MCLR pin fuction is digital I/O, MCLR internally tied to VDD)

#include <xc.h>

// Calibration done using an arduino with serial output
// const int32_t RAW_SELF_SUSPENDED_WEIGHT = 357882L;
// const int32_t RAW_SELF_GRAVITY_WEIGHT = 646718L;
// const int32_t RAW_EDGE_312_GRAMS = 793342L;
// const int32_t RAW_EDGE_1293_GRAMS = 1216109L;
// const int32_t RAW_EDGE_2254_GRAMS = 1615611L;
// const int32_t RAW_EDGE_2566_GRAMS = 1800440L;
// const int32_t RAW_EDGE_3547_GRAMS = 2248014L;
// const int32_t RAW_EDGE_3859_GRAMS = 2383308L;
// const int32_t RAW_EDGE_4619_GRAMS = 2609744L;
// const int32_t UNITS_PER_GRAM = 438L;

// in this application we only care for the 16 most significant bits
#define RAW_THRESHOLD_ALLOW_CENTER (900000L >> 8)
#define RAW_RECODED_THRESHOLD_DISABLE_UNDEVALUE (21000L >> 8)

// PINS
// GPO / output / HEAT_LED_AND_INVERTED_RELAY
// GP1 / output / HX711 / PD_SCK
// GP2 / input / HX711 / DOUT
// GP3 / input / button (not debounced)

// HX711 : after reset channel is A with gain 128
// HX711 output is a 24 bits **signed** int
// output is a 2's complement value
// from min 0x800000 to max 0x7FFFFF

// XC8 user's manual, section "Integer Data Types" :
//   All integer values are represented in little-endian format
//   with the Least Significant Byte (LSB) at the lower address

typedef union {
    // by reading 24 bits first in uint8 #3, then #2, then #1
    uint8_t as_uint8[4];
    // we then can read the most significant bits in int16 #1
    int16_t as_int16[2];
} HX711_VALUE;

void main(void) {

    // Allow using GP2 as GPIO (disable T0CS)
    OPTION = 0xFF & ~T0CS;

    // define HEAT OUTPUT (before TRIS ? need pull-down anyway)
    GPIObits.GP0 = 0;

    // define HX711 CLOCK OUTPUT (before TRIS ? need pull-up anyway)
    GPIObits.GP1 = 1;

    // use GP3 and GP2 as input, GP1 and GP0 as output
    TRIS = 0b1100;

    // define HEAT OUTPUT (after TRIS ? then glitched to whatever was set)
    GPIObits.GP0 = 0;

    // define HX711 CLOCK OUTPUT (after TRIS ? then glitched to whatever was set)
    GPIObits.GP1 = 1;

    // make sure HX711 goes to sleep
    _delay(100);

    // recorded value on threshold
    int16_t recorded_value = 0;

    while (1) {
        // less opcodes if using 8 or 16 bits than 32 bits
        HX711_VALUE values;
        values.as_int16[0] = 0;
        values.as_int16[1] = 0;

        // when PD_SCK is LOW, chip is active
        GPIObits.GP1 = 0;

        // wait for data ready (T1)
        while (GPIObits.GP2);

        // it is 2x faster to process 24 bits as 3 times 8 bits
        // than to use an int32, *AND* it prevents the HX711
        // from going to sleep if clock is high during +60us
        // IMPORTANT : HX711 gives MSB bit first
        for (uint8_t j = 3; j > 0; j--) {
            for (uint8_t i = 0; i < 8; i++) {

                // clock rise
                GPIObits.GP1 = 1;
                // wait for data ready (T2)
                _delay(1);

                // read data bit and append from low side
                values.as_uint8[j] <<= 1;
                values.as_uint8[j] |= GPIObits.GP2;

                // wait high (T3)
                _delay(1);
                // clock fall
                GPIObits.GP1 = 0;
                // wait low (T4)
                _delay(1);
            }
        }

        // PD_SCK should do 1-3 more cycle for next channel select
        // 1 = channel A gain 128
        // 2 = channel B gain 32
        // 3 = channel A gain 64

        // clock rise
        GPIObits.GP1 = 1;
        // wait high (T3)
        _delay(1);
        // clock fall
        GPIObits.GP1 = 0;
        // wait low (T4)
        _delay(1);

        // disable HX711
        GPIObits.GP1 = 1;

        // make sure HX711 goes to sleep
        _delay(100);

        // uint8_t stimulus_values[3] = {
        // 0x37, // 0b00110111 // MSB
        // 0xDF, // 0b11011111
        // 0xEF  // 0b11100111 // LSB
        // };

        // FIXME: process engouh bits ... or not ?

        // SAFETY: if too light at all, disable unconditionally
        if (values.as_int16[1] < RAW_THRESHOLD_ALLOW_CENTER) {
            GPIObits.GP0 = 0;
            continue;
        }

        // FUNCTION: if too light relatively to the recorded value, disable
        if (values.as_int16[1] < recorded_value) {
            GPIObits.GP0 = 0;
            recorded_value = 0;
            continue;
        }

        // enough weight present is still present

        // if button is not pressed (state high, pulled-up), stay the same
        if (GPIObits.GP3) {
            continue;
        }

        // button is pressed (state forced to low)

        // enable heat output
        GPIObits.GP0 = 1;

        // record current value for later use as a fine threshold (less opcodes if done in 2 steps)
        recorded_value = values.as_int16[1];
        recorded_value -= RAW_RECODED_THRESHOLD_DISABLE_UNDEVALUE;
    }
}
