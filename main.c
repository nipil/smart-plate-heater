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

// #define _XTAL_FREQ 4000000L // oscillator frequency for _delay()

#define SET_HEATER GPIObits.GP0
#define SET_CLOCK GPIObits.GP1
#define GET_DATA GPIObits.GP2
#define GET_BUTTON GPIObits.GP3

void main(void) {
    OPTION = 0xFF & ~T0CS; // Allow using GP2 as GPIO (disable T0CS)
    TRIS = 0b1100; // use GP3 and GP2 as input, GP1 and GP0 as output

    while (1) {
        SET_HEATER = GET_BUTTON;
        SET_CLOCK = GET_DATA;
    }
}
