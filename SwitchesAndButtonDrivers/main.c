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
#include "supportFiles/display.h"
#include "supportFiles/leds.h"
#include "buttons.h"
#include "switches.h"
int main() {
    leds_init(false);  // initialize the LED's.  False because we don't want to printf error messages from the led.

    display_init();  // Must init all of the software and underlying hardware for LCD.
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    display_setCursor(0, 0);            // The upper left of the LCD screen.
    display_setTextColor(DISPLAY_RED);  // Make the text red.
    display_setTextSize(2);             // Make the text a little larger.

    switches_init();            // This initializes the switches stuff, ie tri enable dudes.
    buttons_init();             // Same, but for buttons
    buttons_runTest();          // Pull the lever, Kronk!
    switches_runTest();         // And that lever too...


    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    display_setCursor(0, 0);            // The upper left of the LCD screen.
    display_setTextColor(DISPLAY_RED);  // Make the text red.

    display_println("All done!!!");     // this is an add in to show the program completed.

}

// This function must be defined but can be left empty for now.
// You will use this function in a later lab.
// It is called in the timer interrupt service routine (see interrupts.c in supportFiles).
void isr_function() {
    // Empty for now.
}
