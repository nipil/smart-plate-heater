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

#define RAW_THRESHOLD_ALLOW_CENTER 850000L
#define RAW_RECODED_THRESHOLD_DISABLE_UNDEVALUE 50000L

// PINS
// GPO / output / HEAT_LED_AND_INVERTED_RELAY
// GP1 / output / HX711 / PD_SCK
// GP2 / input / HX711 / DOUT
// GP3 / input / button (not debounced)

// HX711 : after reset channel is A with gain 128
#define HX711_N_BITS 24

void main(void) {

    // Allow using GP2 as GPIO (disable T0CS)
    OPTION = 0xFF & ~T0CS;

    // define HEAT OUTPUT prior to enabling
    GPIObits.GP0 = 0;

    // define HX711 CLOCK OUTPUT prior to enabling
    GPIObits.GP1 = 1;

    // use GP3 and GP2 as input, GP1 and GP0 as output
    TRIS = 0b1100;

    // make sure HX711 goes to sleep
    _delay(100);

    // recorded value on threshold
    int32_t recorded_value = 0;

    while (1) {

        // HX711 output is a 24 bits **signed** int
        // output is a 2's complement value
        // from min 0x800000 to max 0x7FFFFF
        int32_t value = 0;

        // when PD_SCK is LOW, chip is active
        GPIObits.GP1 = 0;

        // wait for data ready (T1)
        while (GPIObits.GP2);

        // pull bits MSB first
        for (uint8_t i = 0; i < HX711_N_BITS; i++) {

            // clock rise
            GPIObits.GP1 = 1;
            // wait for data ready (T2)
            _delay(1);

            // read data bit and append from low side
            value = (value << 1);
            value |= GPIObits.GP2;

            // wait high (T3)
            _delay(1);
            // clock fall
            GPIObits.GP1 = 0;
            // wait low (T4)
            _delay(1);
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

        // turn an int24_t into a int32_t by setting more sign bits
        if (value >= 0x00800000) {
            value |= 0xFF000000;
        }

        // SAFETY: if too light at all, disable unconditionally
        if (value < RAW_THRESHOLD_ALLOW_CENTER) {
            GPIObits.GP0 = 0;
            continue;
        }

        // FUNCTION: if too light relatively to the recorded value, disable
        if (value < recorded_value) {
            GPIObits.GP0 = 0;
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

        // record current value for later use as a fine threshold
        recorded_value = value - RAW_RECODED_THRESHOLD_DISABLE_UNDEVALUE;
    }
}
