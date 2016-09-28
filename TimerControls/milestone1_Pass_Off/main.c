/*
 * main.c
 *
 *  Created on: Sep 7, 2016
 *      Author: alex
 */

// Print out "hello world" on both the console and the LCD screen, and draw some cute lines

#define LCD_WIDTH  320.0 // This is the the width of the screen, ie, the long way.  presumably the y
#define LCD_HEIGHT 240.0 // This is the height of the screen, across the short dimension.  presumably the x
#define ORIGIN_X 0
#define ORIGIN_Y 0


#include <stdio.h>
#include "supportFiles/utils.h"
#include "supportFiles/display.h"
#include "supportFiles/leds.h"
#include "../btn/buttons.h"
#include "../intervalTimer.h"

#include "xil_io.h"
#include <stdio.h>

#define TCR0_OFFSET 0x08  // register offset for TCR0
#define TCR1_OFFSET 0x18  // register offset for TCR1

// Reads the timer1 registers based upon the offset.
uint32_t readTimer1Register(uint32_t registerOffset) {
    uint32_t address = XPAR_AXI_TIMER_0_BASEADDR + registerOffset;  // Add the offset to the base address.
    return Xil_In32(address);  // Read the register at that address.
}

#define DELAY_COUNT 3
// Simple busy-wait function.
void waitALongTime() {
    volatile int32_t a = 0;        // Use volatile so that optimizer doesn't mess things up.
    int32_t i, j;  // Simple index variables.
    for (i=0; i<DELAY_COUNT; i++)        // Outer delay-loop.
        for (j=0; j<INT32_MAX; j++)  // Inner delay-loop.
            a++;
}

int main() {
  intervalTimer_init(INTERVAL_TIMER_TIMER_0);  // Init timer 0.
  intervalTimer_reset(INTERVAL_TIMER_TIMER_0); // Reset timer 0.
  // Show that the timer is reset.
  // Check lower register.
  printf("timer_0 TCR0 should be 0 at this point:%ld\n\r", readTimer1Register(TCR0_OFFSET));
  // Check upper register.
  printf("timer_0 TCR1 should be 0 at this point:%ld\n\r", readTimer1Register(TCR1_OFFSET));
  intervalTimer_start(INTERVAL_TIMER_TIMER_0);  // Start timer 0.
  // Show that the timer is running.
  printf("The following register values should be changing while reading them.\n\r");
  // Just checking multiple times to see if the timer is running.
  printf("timer_0 TCR0 should be changing at this point:%ld\n\r", readTimer1Register(TCR0_OFFSET));
  printf("timer_0 TCR0 should be changing at this point:%ld\n\r", readTimer1Register(TCR0_OFFSET));
  printf("timer_0 TCR0 should be changing at this point:%ld\n\r", readTimer1Register(TCR0_OFFSET));
  printf("timer_0 TCR0 should be changing at this point:%ld\n\r", readTimer1Register(TCR0_OFFSET));
  printf("timer_0 TCR0 should be changing at this point:%ld\n\r", readTimer1Register(TCR0_OFFSET));

  // Wait about 2 minutes so that you roll over to TCR1.
  // If you don't see a '1' in TCR1 after this long wait you probably haven't programmed the timer correctly.
  waitALongTime();
  // Check lower register.
  printf("timer_0 TCR0 value after wait:%lx\n\r", readTimer1Register(TCR0_OFFSET));
  // Check upper register.
  printf("timer_0 TCR1 should have changed at this point:%ld\n\r", readTimer1Register(TCR1_OFFSET));
}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
