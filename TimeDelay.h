/* 
 * File:   TimeDelay.h
 * Author: ducdu
 *
 * Created on September 20, 2024, 2:54 PM
 */

#ifndef TIMEDELAY_H
#define	TIMEDELAY_H

#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA101.h>
#include "clkChange.h"

void delay_ms(uint16_t time_ms);
uint32_t get_system_time_ms();


#endif	/* TIMEDELAY_H */

