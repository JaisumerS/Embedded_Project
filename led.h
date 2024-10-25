/* 
 * File:   led.h
 * Author: David Nguyen
 *
 * Created on October 15, 2024, 3:04 AM
 */

#ifndef LED_H
#define	LED_H

#include <stdio.h>
#include <stdlib.h>
#include "TimeDelay.h"
#include <xc.h>

void blinkLED(int milliseconds);
void LedOn();
void LedOff();

#endif	/* LED_H */

