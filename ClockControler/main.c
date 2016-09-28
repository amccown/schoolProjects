
// Print out "hello world" on both the console and the LCD screen, and draw some cute lines

#define LCD_WIDTH  320.0 // This is the the width of the screen, ie, the long way.  presumably the y
#define LCD_HEIGHT 240.0 // This is the height of the screen, across the short dimension.  presumably the x
#define ORIGIN_X 0
#define ORIGIN_Y 0


#include <stdio.h>
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include "supportFiles/leds.h"
#include "supportFiles/globalTimer.h"
#include "supportFiles/interrupts.h"
#include "clockControl.h"
#include "clockDisplay.h"
#include "sw/switches.h"


#define TOTAL_SECONDS 60
// The formula for computing the load value is based upon the formula from 4.1.1 (calculating timer intervals)
// in the Cortex-A9 MPCore Technical Reference Manual 4-2.
// Assuming that the prescaler = 0, the formula for computing the load value based upon the desired period is:
// load-value = (period * timer-clock) - 1
#define TIMER_PERIOD 50.0E-3
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define TIMER_LOAD_VALUE ((TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)

int main() {


    printf("Initializing \n\r");
    switches_init();            // initialize my switches, to use in killing process
    // Initialize the GPIO LED driver and print out an error message if it fails (argument = true).
    // You need to init the LEDs so that LD4 can function as a heartbeat.
    leds_init(true);
    // Init all interrupts (but does not enable the interrupts at the devices).
    // Prints an error message if an internal failure occurs because the argument = true.
    interrupts_initAll(true);
    interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
    printf("timer load value:%ld\n\r", (int32_t) TIMER_LOAD_VALUE);
    u32 privateTimerTicksPerSecond = interrupts_getPrivateTimerTicksPerSecond();
    printf("private timer ticks per second: %ld\n\r", privateTimerTicksPerSecond);
    interrupts_enableTimerGlobalInts();
    // Initialization of the clock display is not time-dependent, do it outside of the state machine.
    clockDisplay_init();
    // Start the private ARM timer running.
    interrupts_startArmPrivateTimer();
    // Enable interrupts at the ARM.
    interrupts_enableArmInts();

    while((switches_read() & 0x3) != 0)
    {

    }
    // All done, now disable interrupts and print out the interrupt counts.
    interrupts_disableArmInts();
    printf("All Done\n\r");

}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// The clockControl_tick() function is now called directly by the timer interrupt service routine.
void isr_function() {
   clockControl_tick();
   clockControl_debugStatePrint();
}
