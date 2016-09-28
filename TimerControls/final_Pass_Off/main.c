/*
 * main.c
 *
 *  Created on: Sep 11, 2016
 *      Author: alex
 */

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

#define TEST_ITERATION_COUNT 4
#define ONE_SECOND_DELAY 1000

int main() {
  double duration0, duration1, duration2;  // Will hold the duration values for the various timers.
  buttons_init();           // init the buttons package.
  intervalTimer_initAll();  // init all of the interval timers.
  intervalTimer_resetAll(); // reset all of the interval timers.
  // Poll the push-buttons waiting for BTN0 to be pushed.
  printf("Interval Timer Accuracy Test\n\r");     // User status message.
  printf("waiting until BTN0 is pressed.\n\r");   // Tell user what you are waiting for.
  while (!(buttons_read() & BUTTONS_BTN0_MASK));  // Loop here until BTN0 pressed.
  // Start all of the interval timers.
  intervalTimer_start(INTERVAL_TIMER_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_TIMER_1);
  intervalTimer_start(INTERVAL_TIMER_TIMER_2);
  printf("started timers.\n\r");
  printf("waiting until BTN1 is pressed.\n\r");  // Poll BTN1.
  while (!(buttons_read() & BUTTONS_BTN1_MASK)); // Loop here until BTN1 pressed.
  // Stop all of the timers.
  intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
  intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
  intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
  printf("stopped timers.\n\r");
  // Get the duration values for all of the timers.
  duration0 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0);
  duration1 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1);
  duration2 = intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2);
  // Print the duration values for all of the timers.
  printf("Time Duration 0: %6.2e seconds.\n\r", duration0);
  printf("Time Duration 1: %6.2e seconds.\n\r", duration1);
  printf("Time Duration 2: %6.2e seconds.\n\r", duration2);
  // Now, test to see that all timers can be restarted multiple times.
  printf("Iterating over fixed delay tests\n\r");
  printf("Delays should approximately be: 1, 2, 3, 4 seconds.\n\r");
  for (int i=0; i<TEST_ITERATION_COUNT; i++) {
    // Reset all the timers.
    intervalTimer_resetAll();
    // Start all the timers.
    intervalTimer_start(INTERVAL_TIMER_TIMER_0);
    intervalTimer_start(INTERVAL_TIMER_TIMER_1);
    intervalTimer_start(INTERVAL_TIMER_TIMER_2);
    // Delay is based on the loop count.
    utils_msDelay((i+1)*ONE_SECOND_DELAY);
    // Stop all of the timers.
    intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
    // Print the duration of all of the timers. The delays should be approximately 1, 2, 3, and 4 seconds.
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_0, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_1, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_2, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));
  }
  // Now, test for increment timing (start-stop-start-stop...)
  // Reset all the timers.
  intervalTimer_resetAll();
  printf("Delays should approximately be: 1, 3, 6, 10 seconds.\n\r");
  for (int i=0; i<TEST_ITERATION_COUNT; i++) {
    // Start all the timers.
    intervalTimer_start(INTERVAL_TIMER_TIMER_0);
    intervalTimer_start(INTERVAL_TIMER_TIMER_1);
    intervalTimer_start(INTERVAL_TIMER_TIMER_2);
    // Delay is based on the loop count.
    utils_msDelay((i+1)*ONE_SECOND_DELAY);
    // Stop all of the timers.
    intervalTimer_stop(INTERVAL_TIMER_TIMER_0);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_1);
    intervalTimer_stop(INTERVAL_TIMER_TIMER_2);
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_0, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_1, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));
    printf("timer:(%d) duration:%f\n\r", INTERVAL_TIMER_TIMER_2, intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));
  }

  printf("intervalTimer Test Complete.\n\r");
}


// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
