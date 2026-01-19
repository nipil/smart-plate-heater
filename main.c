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

void delay_cycles(uint8_t count) {
    // in simulator, triggers a cycle delay of :
    // 1=20cyc, 2=24cyc, 3=32cyc, 40cyc, 48cyc ... +8cyc/unit
    while (count--) {
    };
}

void wait_for_data_high() {
    while (1) {
        if (GPIObits.GP2)
            return;
    }
}

void wait_for_data_low() {
    // post-delay of about 10 cycles
    while (GPIObits.GP2);
}

// non repeating pattern for stimulus
// 11111011 10011010 11000101

// PINS
// GPO / output / OPTOTRIAC
// GP1 / output / HX711 / PD_SCK
// GP2 / input / HX711 / DOUT
// GP3 / input / button (not debounced)

// HX711 : after reset channel is A with gain 128

void main(void) {
    // init
    OPTION = 0xFF & ~T0CS; // Allow using GP2 as GPIO (disable T0CS)
    TRIS = 0b1100; // use GP3 and GP2 as input, GP1 and GP0 as output
    GPIObits.GP0 = 0; // disable optotriac
    GPIObits.GP1 = 1; // disable HX711

    GPIObits.GP0 = 1;
    _delay(6);
    GPIObits.GP0 = 0;

    GPIObits.GP0 = 1;
    _delay(7);
    GPIObits.GP0 = 0;

    GPIObits.GP0 = 1;
    _delay(8);
    GPIObits.GP0 = 0;

    GPIObits.GP0 = 1;
    _delay(9);
    GPIObits.GP0 = 0;

    GPIObits.GP0 = 1;
    _delay(10);
    GPIObits.GP0 = 0;

    GPIObits.GP0 = 1;
    _delay(11);
    GPIObits.GP0 = 0;
//    wait_for_data_low(); // conversion not ready while DOUT is HIGH



    // when PD_SCK is LOW, chip is active
    // when DOUT falls, data is ready
    // wait for T1 (+0.1 micros) before PD_SCK rises
    // wait for T2 (+0.1 micros) before reading DOUT
    // PD_SCK should stay HIGH for +0.2 to less than 50 micros
    // PD_SCK should go LOW for +0.2 micros
    // PD_SCK should do 24 cycles (data MSB first)
    // output is a 2's complement value, from min 0x800000 to max 0x7FFFFF
    // PD_SCK should do 1-3 more cycle for next channel select
    // 1 = channel A gain 128, 2 = channel B gain 32, 3 = channel A gain 64
    // chip powers down after 60 micros of PD_SCK being HIGH



    GPIObits.GP0 = 1; // enable optotriac (DEBUG: use as end-of-processing mark)
    GPIObits.GP1 = 1; // disable HX711

    uint8_t data = GPIO;
}
