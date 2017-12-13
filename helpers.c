#include "helpers.h"
#include "types.h"
#include <pic32mx.h>

/**
 * Setup port D as button input
 */
void enableButtons() {
    TRISD |= (0x3f << 5);
}

/**
 * Enables multi vector mode which allows
 * multiple distinct interrupt handlers.
 */
void enableMultiVectorMode() {
    INTCONSET = 0x1000;
}

/**
 *  Set up and start timer 2
 */
void enableTimer2(int period, int priority, int prescaling, int interrupts) {
    T2CON = 0x0;            // stop timer
    TMR2 = 0;               // clear timer
    PR2 = period;           // set period
    IPCSET(2) = priority;   // set priority
    
    IFSCLR(0) = 0x100;      // reset timer interrupt status flag
    
    // enable timer interrupts
    if (interrupts == 1) { IECSET(0) = 0x100; }
    
    // set prescaling
    T2CONSET = prescaling << 4;
    
    // enable
    T2CONSET = 0x8000;
}
