/* 
 * File:   TimeDelay.c
 * Author: ducdu
 *
 * Created on September 20, 2024, 2:53 PM
 */

#include "TimeDelay.h"

volatile uint32_t system_time_ms = 0;

uint32_t get_system_time_ms(){
    uint32_t time;
    
    // Disable interrupts to prevent issues with concurrent access
    IEC0bits.T2IE = 0;  
    time = system_time_ms;  // Read the current system time
    IEC0bits.T2IE = 1;  // Re-enable interrupts
    
    return time;
}

void delay_ms(uint16_t time_ms){
    newClk(500); // Set the system clock to 500 kHz
    T2CONbits.TSIDL = 0; // Configure Timer 2 to operate in idle mode
    T2CONbits.T32 = 0; // Set Timer 2 to operate as a 16-bit timer
    T2CONbits.TCKPS = 2; // Set prescaler to 64 
    T2CONbits.TCS = 0; // Use internal clock for Timer 2
    TMR2 = 0; // Clear the TMR2 register to start counting from zero 
    PR2 = time_ms * 3.91; // Calculate the PR2 value based on the desired delay
    
    T2CONbits.TON = 1; // Start Timer 2  
    IPC1bits.T2IP = 7; // Set Timer 2 interrupt priority;
    IFS0bits.T2IF = 0; // Clear the Timer 2 interrupt flag to start fresh
    IEC0bits.T2IE = 1; // Enable Timer 2 interrupt
}

// Timer 2 interrupt service routine (ISR)
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    // Clear the Timer 2 interrupt flag to acknowledge the interrupt
    IFS0bits.T2IF = 0; // This prevents the ISR from being triggered repeatedly
    Nop(); // A no-operation instruction; can be used for debugging or timing
    return; // Exit the ISR
}




