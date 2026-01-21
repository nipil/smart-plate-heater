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

// oscillator frequency for _delay*() macros
// https://support.microchip.com/s/article/Delay-functions-in-XC16---XC8-using-PIC-devices
#define _XTAL_FREQ 4000000L

// non repeating pattern for stimulus
// 11111011 10011010 11000101

// PINS
// GPO / output / OPTOTRIAC
// GP1 / output / HX711 / PD_SCK
// GP2 / input / HX711 / DOUT
// GP3 / input / button (not debounced)

// HX711 : after reset channel is A with gain 128
#define HX711_N_BITS 24

void main(void) {
    // Allow using GP2 as GPIO (disable T0CS)
    OPTION = 0xFF & ~T0CS;
    // use GP3 and GP2 as input, GP1 and GP0 as output
    TRIS = 0b1100;
    // disable optotriac
    GPIObits.GP0 = 0;
    // disable HX711
    GPIObits.GP1 = 1;
    // make sure HX711 goes to sleep
    _delay(100);

    // make sure HX711 goes to sleep (>60us) at first
    while (1) {

        // DEBUG: time marker
        GPIObits.GP0 = 1;

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
            value = (value << 1) | GPIObits.GP2;
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

        // DEBUG: time marker
        GPIObits.GP0 = 0;
        _delay(1);

        // make sure HX711 goes to sleep
        _delay(100);
    }
}
