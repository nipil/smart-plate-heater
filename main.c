/* 
 * File:   main.c
 * Author: nipil
 *
 * Created on 17 janvier 2026, 22:03
 */

#include <stdio.h>
#include <stdlib.h>

// PIC10F202 Configuration Bit Settings

// 'C' source line config statements

// CONFIG
#pragma config WDTE = OFF       // Watchdog Timer (WDT disabled)
#pragma config CP = OFF         // Code Protect (Code protection off)
#pragma config MCLRE = OFF      // Master Clear Enable (GP3/MCLR pin fuction is digital I/O, MCLR internally tied to VDD)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

/*
 * 
 */
int main(/*int argc, char** argv*/) {
    uint8_t a = 0x69;
    a++;
    uint8_t b = 0x07;
    b--;
    uint8_t c = a - b;
    uint8_t e = c++;

    return (EXIT_SUCCESS);
}
