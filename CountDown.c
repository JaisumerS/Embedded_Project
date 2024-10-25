/* 
 * File:   CountDown.c
 * Author: ducdu
 *
 * Created on October 15, 2024, 3:18 AM
 */


#include <stdio.h>
#include <stdlib.h>
#include "CountDown.h"

// Timer variables to store minutes and seconds
int timer_minutes = 0; // Minutes component of the timer
int timer_seconds = 0; // Seconds component of the timer

// Function to increment timer by a specified number of minutes
void increment_timer_by_min(int minutes){
    if(timer_minutes + minutes < 60){
        timer_minutes += minutes; // Add minutes if total is less than 60
    }
    else{
        // Wrap around if total minutes exceed 60
        int temp = timer_minutes + minutes; // Calculate total minutes
        timer_minutes = temp % 60; // Set minutes to remainder after dividing by 60
    }
}

// Function to increment timer by a specified number of seconds
void increment_timer_by_sec(int seconds){
    if(timer_seconds + seconds < 60){
        timer_seconds += seconds; // Add seconds if total is less than 60
    }
    else{
        // Wrap around if total seconds exceed 60
        int temp = timer_seconds + seconds; // Calculate total seconds
        timer_seconds = temp % 60; // Set seconds to remainder after dividing by 60
        increment_timer_by_min(1); // Increment minutes if seconds exceeded 60
    }
}

// Function to decrement timer by a specified number of seconds
void decrement_timer_by_sec(int seconds){
    if(timer_seconds - seconds >= 0){
        timer_seconds -= seconds; // Subtract seconds if non-negative
    }
    else{
        // Handle underflow in seconds
        int temp = timer_seconds - seconds; // Calculate total seconds after subtraction
        timer_seconds = 60 + temp; // Wrap around to remaining seconds
        decrement_timer_by_min(1); // Decrement minutes if seconds went below 0
    }
}

// Function to decrement timer by a specified number of minutes
void decrement_timer_by_min(int minutes){
    if(timer_minutes - minutes >= 0){
        timer_minutes -= minutes; // Subtract minutes if non-negative
    }
    else{
        // Handle underflow in minutes
        int temp = timer_minutes - minutes; // Calculate total minutes after subtraction
        timer_minutes = 60 + temp; // Wrap around to remaining minutes
    }
}

// Function to reset timer to zero
void reset_timer(){
    timer_minutes = 0; // Set minutes to 0
    timer_seconds = 0; // Set seconds to 0
}

// Function to get the current minutes value
int getMin(){
    return timer_minutes; // Return current minutes
}

// Function to get the current seconds value
int getSec(){
    return timer_seconds; // Return current seconds
}


