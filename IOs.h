/* 
 * File:   IOs.h
 * Author: David Nguyen
 *
 * Created on September 20, 2024, 2:42 PM
 */

#ifndef IOS_H
#define	IOS_H

#include <stdio.h>
#include <stdlib.h>
#include "TimeDelay.h"
#include "UART2.h"
#include "led.h"
#include "CountDown.h"
#include <xc.h>

typedef enum{
    NO_PB_PRESSED,
    PB1_PRESSED,
    PB2_PRESSED,
    PB3_PRESSED
}state_t;

void timer();
void clear_alarm();
void IOinit();
void IOcheck();



#endif	/* IOS_H */

