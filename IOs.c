/* 
 * File:   IOs.c
 * Author: David Nguyen
 *
 * Created on September 20, 2024, 2:38 PM
 */

#include "IOs.h"

// CN flag definitions for push buttons
uint16_t CN30flag;  // RA2 (PB1)
uint16_t CN0flag;   // RA4 (PB3)
uint16_t CN1flag;   // RB4 (PB2)

static int timer_start_flag = 0; // Flag to indicate if the timer is running
int buttonPressDetected = 0; // Flag for detecting button presses
int twoButtonPressDetected = 0; // Flag for detecting simultaneous button presses
state_t state = NO_PB_PRESSED; // Current state of the buttons
static int increment_amount = 1; // Initial increment amount for timer
int hold_duration = 0; // Duration button is held down
int pb3_short_press_flag; // Flag for PB3 short press
int pb3_held = 0; // Flag for PB3 being held down

static int alarm_initiated = 0; // Flag to indicate if the alarm is active
static uint32_t startTime = 0; // Variable for tracking time
static int blinkFlag = 0; // Flag for LED blinking

// IO initialization function
void IOinit(){
    AD1PCFG = 0xFFFF; /* Set I/O pins that can also be analog to be digital */
    
    newClk(500); // Set system clock

    // Initialize LED pin
    TRISBbits.TRISB8 = 0; // Set LED pin as output
    LATBbits.LATB8 = 0; // Turn off LED
    
    // Initialize push button PB3 (RA4) and enable interrupts
    TRISAbits.TRISA4 = 1; // Set PB3 as input
    CNPU1bits.CN0PUE = 1; // Enable pull-up resistor
    CNEN1bits.CN0IE = 1; // Enable change notification interrupt
    
    // Initialize push button PB2 (RB4) and enable interrupts
    TRISBbits.TRISB4 = 1; // Set PB2 as input
    CNPU1bits.CN1PUE = 1; // Enable pull-up resistor
    CNEN1bits.CN1IE = 1; // Enable change notification interrupt
    
    // Initialize push button PB1 (RA2) and enable interrupts
    TRISAbits.TRISA2 = 1; // Set PB1 as input
    CNPU2bits.CN30PUE = 1; // Enable pull-up resistor
    CNEN2bits.CN30IE = 1; // Enable change notification interrupt
    
    IPC4bits.CNIP = 6;   // Set priority for interrupts
    IFS1bits.CNIF = 0;   // Clear change notification interrupt flag
    IEC1bits.CNIE = 1;   // Enable change notification interrupt globally
    
    // Initialize UART
    InitUART2(); 
}

// Function to check the status of buttons and handle their actions
void IOcheck(){
    // Debounce button presses
    if(buttonPressDetected){
        delay_ms(50); // Delay for debounce
        buttonPressDetected = 0; // Reset button press detection flag
    }
    if(twoButtonPressDetected){
        delay_ms(50); // Delay for debounce
        twoButtonPressDetected = 0; // Reset two button press detection flag
    }

    // Check if PB1 (RA2) is pressed
    if(CN30flag && !CN0flag && !CN1flag && !buttonPressDetected){
        state = PB1_PRESSED; // Update state to PB1 pressed
        if(alarm_initiated){
            clear_alarm(); // Clear alarm if active
            alarm_initiated = 0; // Reset alarm flag
        }
        
        increment_timer_by_min(1); // Increment timer by one minute
        char buffer[20]; // Buffer for UART message
        sprintf(buffer, "\rSET %02dm : %02ds", getMin(), getSec()); // Format message
        Disp2String(buffer); // Send message to terminal
        delay_ms(INCREMENT_DELAY); // Delay after increment
    }

    // Check if PB2 (RB4) is pressed
    if(!CN30flag && !CN0flag && CN1flag && !buttonPressDetected){
        state = PB2_PRESSED; // Update state to PB2 pressed
        if(alarm_initiated){
            clear_alarm(); // Clear alarm if active
            alarm_initiated = 0; // Reset alarm flag
        }
        
        hold_duration++; // Increment hold duration
        if(hold_duration >= 6){
            increment_amount = 5; // Set increment amount to 5 if held for 2 seconds
        }
        else{
            increment_amount = 1; // Set increment amount to 1 if held for less than 2 seconds
        }
        
        increment_timer_by_sec(increment_amount); // Increment timer
        char buffer[20]; // Buffer for UART message
        sprintf(buffer, "\rSET %02dm : %02ds", getMin(), getSec()); // Format message
        Disp2String(buffer); // Send message to terminal
        delay_ms(INCREMENT_DELAY); // Delay after increment
    }

    // Check if PB3 (RA4) is pressed
    if(!CN30flag && CN0flag && !CN1flag && !buttonPressDetected){
        // Record the button hold start time if it's the first pressed
        if(!pb3_held){
            pb3_held = 1; // Set flag indicating PB3 is held
            hold_duration = 0; // Reset hold duration
        }
        hold_duration++; // Increment hold duration
    }
    else if (pb3_held && !CN30flag && !CN0flag && !CN1flag) {
        pb3_held = 0;  // Button released: reset the holding flag

        // Check if button was held long enough for a long press (greater than 3 seconds)
        if (hold_duration >= 2000) {
            // Long press: reset the timer and clear alarm
            if (alarm_initiated) {
                clear_alarm(); // Clear alarm if active
                alarm_initiated = 0; // Reset alarm flag
            }
            reset_timer(); // Reset the timer
            
            // Display "CLR 00m : 00s" on the terminal
            char buffer[20];
            sprintf(buffer, "\rCLR %02dm : %02ds", getMin(), getSec()); // Format message
            Disp2String(buffer); // Send message to terminal
            
            // Stop the timer
            timer_start_flag = 0; 
        } 
        // Short press (< 3 sec): toggle start/pause
        else if (hold_duration < 2000) {
            timer_start_flag = !timer_start_flag;  // Toggle start/pause state
            state = PB3_PRESSED; // Update state to PB3 pressed
            
            // Check if the timer is starting
            if (timer_start_flag) {
                pb3_short_press_flag = 1; // Set flag for countdown
            } else {
                pb3_short_press_flag = 0; // Clear flag to pause countdown
            }
        }
    }

    // Check if no buttons are pressed
    if(!CN30flag && !CN0flag && !CN1flag){
        hold_duration = 0; // Reset hold duration
        increment_amount = 1; // Reset increment amount
        state = NO_PB_PRESSED; // Update state to no button pressed
        Idle(); // Idle state
    } 
    
    // Handle timer countdown if running
    if(timer_start_flag){
        timer(); // Call timer function
    }
}

// Timer function to handle countdown logic
void timer(){
    if(getMin() == 0 && getSec() == 0){
        char buffer[20]; // Buffer for UART message
        sprintf(buffer, "\rFIN %02dm : %02ds -- ALARM ", getMin(), getSec()); // Format message
        Disp2String(buffer); // Send message to terminal
        LedOn(); // Turn on LED for alarm
        alarm_initiated = 1; // Set alarm flag
        timer_start_flag = 0; // Stop the timer
    } 
    else if(!pb3_held){ // Do not decrement if PB3 is held
        char buffer[20]; // Buffer for UART message
        sprintf(buffer, "\rCNT %02dm : %02ds", getMin(), getSec()); // Format message
        Disp2String(buffer); // Send message to terminal
        blinkFlag = 1; // Set flag for LED blinking
        decrement_timer_by_sec(1); // Decrement timer by one second
    }

    if(blinkFlag){
        blinkLED(750); // Blink LED for visual feedback
        blinkFlag = 0; // Reset blink flag
    }
}

// Function to clear the alarm message and turn off LED
void clear_alarm(){
    Disp2String("\r                       "); // Clear display
    LedOff(); // Turn off LED
}

// Change Notification Interrupt Service Routine
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0; // Clear CN interrupt flag
    buttonPressDetected = 1; // Set button press detection flag

    // Check if two buttons are pressed simultaneously
    if((!PORTAbits.RA2) && (!PORTBbits.RB4) || 
       (!PORTBbits.RB4) && (!PORTAbits.RA4) ||
       (!PORTAbits.RA2) && (!PORTAbits.RA4)) {
        twoButtonPressDetected = 1; // Set flag for two button presses
    }
    CN30flag = (PORTAbits.RA2 == 0); // Update flag for PB1
    CN1flag = (PORTBbits.RB4 == 0); // Update flag for PB2
    CN0flag = (PORTAbits.RA4 == 0); // Update flag for PB3
}

