#ifndef COUNTDOWN_H
#define	COUNTDOWN_H

#include "UART2.h"

#define INCREMENT_DELAY 300


void increment_timer_by_min(int minutes);
void increment_timer_by_sec(int seconds);
void decrement_timer_by_sec(int seconds);
void decrement_timer_by_min(int minutes);
void reset_timer();


int getMin();
int getSec();



#endif	/* COUNTDOWN_H */

