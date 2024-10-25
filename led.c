/* 
 * File:   led.c
 * Author: David Nguyen
 *
 * Created on October 15, 2024, 3:04 AM
 */

#include "led.h"

void blinkLED(int milliseconds){
    LATBbits.LATB8 = !LATBbits.LATB8;
    delay_ms(milliseconds);
}

void LedOn(){
    LATBbits.LATB8 = 1;
}

void LedOff(){
    LATBbits.LATB8 = 0;
}


