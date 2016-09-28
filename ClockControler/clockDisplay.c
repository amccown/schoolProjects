/*
 * clockDisplay.c
 *
 *  Created on: Sep 13, 2016
 *      Author: alex
 */


#include "clockDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <inttypes.h>


#define CLOCK_TEXT_SIZE 3                                         // my clock size

#define TRIANGLE_COLOR DISPLAY_GREEN                                // triangle color
#define CENTER_OF_SCREEN_X (DISPLAY_WIDTH / 2)              // for geting to the middle of things
#define CENTER_OF_SCREEN_Y (DISPLAY_HEIGHT / 2)             // same but for y

#define TRIANGLE_HEIGHT  (50 - 6*(  6 - CLOCK_TEXT_SIZE))          // height of our triangles
#define TRIANGLE_BASE_HALF  (40 - 5*( 6 - CLOCK_TEXT_SIZE))       // half of the width (for calculations)
#define TRIANGLE_CORNER_Y_OFFSET  (TRIANGLE_HEIGHT * 2)             // positioning of our corners
#define TRIANGLE_SPACING (100 - 6*(6 - CLOCK_TEXT_SIZE))          // spacing in between any two triangles

#define COLON_1_X ((CENTER_OF_SCREEN_X ) - 3*(TRIANGLE_SPACING / 7) - TEXT_WIDTH )        // x coordinate of the left colon
#define COLON_2_X ((CENTER_OF_SCREEN_X ) + (TRIANGLE_SPACING / 2) - TEXT_WIDTH + 10)        // x coordinate of the right colon
#define CLOCKDISPLAY_NUM_OF_COLUMS 2                                // number of columsn in the display
#define CLOCKDISPLAY_NUM_OF_ROWS 3                                  // number of rows in the display
#define CLOCK_DISPLAY_SEC_ROW_NUM 2                                 // which row index is the seconds
#define TEXT_HEIGHT (24 - 4*(6 - CLOCK_TEXT_SIZE))                                              // height of my test.
#define TEXT_WIDTH  (24 - 3*(6 - CLOCK_TEXT_SIZE))
#define COLON_Y      ((CENTER_OF_SCREEN_Y) - TEXT_HEIGHT)             // where to draw the colons

#define NUM_OF_DIGITS 6 // the number of digits.  Boring.
#define NUM_OF_SEC_IN_A_MIN 60 // I feel dumb having to write this here.
#define NUM_OF_MIN_IN_AN_HOUR 60 // I feel dumb having to write this here.
#define NUM_OF_HOURS_IN_CLOCK 12 // This could be used to change to military time (ie, a 24 hour clock).

#define SELF_TEST_LOOPS 3600 // the number of loops to have the test() function go through
#define SELF_TEST_DELAY 10     // the time in ms to delay between test loop iterations

#define TEXT_Y_COORDINATE (DISPLAY_HEIGHT / 2 - TEXT_HEIGHT)      // this just says "write stuff in the middle of the screen" (vertically)
// Adjust this line for the formula for changing the font size.
#define TEXT_X_0 (CENTER_OF_SCREEN_X - 4*TEXT_WIDTH - 40)
#define TEXT_X_1 (CENTER_OF_SCREEN_X - 2*TEXT_WIDTH - 50)
#define TEXT_X_2 (CENTER_OF_SCREEN_X - (2*TEXT_WIDTH) + 10)
#define TEXT_X_3 (CENTER_OF_SCREEN_X)
#define TEXT_X_4 (CENTER_OF_SCREEN_X + (TEXT_WIDTH) + 50)
#define TEXT_X_5 (CENTER_OF_SCREEN_X + (5*TEXT_WIDTH/2) + 50)


/// MY variables. for this file only
static const uint16_t TEXT_X_COORDINATE[6] = {TEXT_X_0, TEXT_X_1, TEXT_X_2, TEXT_X_3, TEXT_X_4, TEXT_X_5};

static uint8_t seconds, minutes = 0;   // variables for keeping time.
static uint8_t hours = 12;             // my variables for keeping time.  sort of global, but heck.

static char time_str_cur[6];               // my string for the currently displayed time value
static char time_str_next[6] = {'1','2','0','0','0','0'};  // = {'j','o','m','a','m','a'};              // my string for the to-be- displayed time value


// Helper function who draws my triangle given a top corner.  Heights and bases
// are defined above, based on font size
void clockDisplay_drawTriangle(uint32_t x, uint32_t y, bool is_top)
{
    if(is_top){  // if we're drawing a top row triangle.
    display_fillTriangle(x, y, x - TRIANGLE_BASE_HALF, y - TRIANGLE_HEIGHT, x + TRIANGLE_BASE_HALF, // draw stuff
            y - TRIANGLE_HEIGHT, TRIANGLE_COLOR); } // finish stuff
    else {   // ie, if we're drawing a bottom row triangle
    display_fillTriangle(x, y, x - TRIANGLE_BASE_HALF, y + TRIANGLE_HEIGHT, x + TRIANGLE_BASE_HALF, // draw stuff
               y + TRIANGLE_HEIGHT, TRIANGLE_COLOR);} // finish stuff
}

// Helper function for changing the seconds value when the buttons are pressed.
// Note that I've used a < and a > sign to check bounds.  This will be slightly slower
// Than saying == 59 or == 0.  However, it will also guarantee that in the event of a bug
// where the value glitched to some number OVER the max, it will get reset to 0 instead of
// constantly incrementing.
void incDec_seconds(bool increment_it)          // function dec.  param is "up or not"
{
    if(increment_it)        // it needs to be incremented.
    {
        if(seconds < (NUM_OF_SEC_IN_A_MIN - 1))         // if it's less than the max value
            seconds++;                                  // increment.
        else                                            // if it's not, ie, it's 59 or higher
            seconds = 0;                                // we reset it.
    }
    else                                                // if we're not incrementing.
    {
        if(seconds > 0)                                 // if it's greater than 0
            seconds--;                                  // decrement (I think that's a word) it
        else
            seconds = (NUM_OF_SEC_IN_A_MIN - 1);        // reset it to the max value, since we've rolled over
    }
}


// Helper function for changing the seconds value when the buttons are pressed.
// Note that I've used a < and a > sign to check bounds.  This will be slightly slower
// Than saying == 59 or == 0.  However, it will also guarantee that in the event of a bug
// where the value glitched to some number OVER the max, it will get reset to 0 instead of
// constantly incrementing.
void incDec_minutes(bool increment_it)          // function dec.  param is "up or not"
{
    if(increment_it)        // it needs to be incremented.
    {
        if(minutes < (NUM_OF_MIN_IN_AN_HOUR - 1))         // if it's less than the max value
            minutes++;                                  // increment.
        else                                            // if it's not, ie, it's 59 or higher
            minutes = 0;                                // we reset it.
    }
    else                                                // if we're not incrementing.
    {
        if(minutes > 0)                                 // if it's greater than 0
            minutes--;                                  // decrement (I think that's a word) it
        else
            minutes = (NUM_OF_MIN_IN_AN_HOUR - 1);        // reset it to the max value, since we've rolled over
    }
}


// Helper function for changing the seconds value when the buttons are pressed.
// Note that I've used a < and a > sign to check bounds.  This will be slightly slower
// Than saying == 59 or == 0.  However, it will also guarantee that in the event of a bug
// where the value glitched to some number OVER the max, it will get reset to 0 instead of
// constantly incrementing.
void incDec_hours(bool increment_it)          // function dec.  param is "up or not"
{
    if(increment_it)        // it needs to be incremented.
    {
        if(hours < (NUM_OF_HOURS_IN_CLOCK))         // if it's less than the max value
            hours++;                                  // increment.
        else                                            // if it's not, ie, it's 59 or higher
            hours = 1;                                // we reset it.
    }
    else                                                // if we're not incrementing.
    {
        if(hours > 1)                                 // if it's greater than 1
            hours--;                                  // decrement (I think that's a word) it
        else
            hours = (NUM_OF_HOURS_IN_CLOCK);        // reset it to the max value, since we've rolled over
    }
}



// Called only once - performs any necessary inits.
// This is a good place to draw the triangles and any other
// parts of the clock display that will never change.
void clockDisplay_init()
{
    display_init(); // initialize the screen
    display_fillScreen(DISPLAY_BLACK);          // blank my screen!
    // Draw a triangle based on font size
    for(int i = -1; i <= 1; i++)            // here is a for loop for drawing all the triangles.
    {
        uint32_t x_value = (CENTER_OF_SCREEN_X + TRIANGLE_SPACING * i);          // tell where we are at
        uint32_t y_value = (CENTER_OF_SCREEN_Y + TRIANGLE_CORNER_Y_OFFSET);     // calculate the y value for upper triangles
        clockDisplay_drawTriangle(x_value, y_value, true);                      // draw the triangle

        uint32_t y_down_value = (CENTER_OF_SCREEN_Y - TRIANGLE_CORNER_Y_OFFSET);     // calculate the y value for lower triangles
        clockDisplay_drawTriangle(x_value, y_down_value, false);                      // draw the triangle
    }
    // write colons of the given font size
    display_setTextColor(DISPLAY_GREEN);                    // we're writing in green
    display_setTextSize(CLOCK_TEXT_SIZE);                   // clock size
    display_setCursor(COLON_1_X, COLON_Y);       // set for the left colon
    display_println(":\n\r");                               // print out a colon
    display_setCursor(COLON_2_X, COLON_Y);       // set for the right colon
    display_println(":\n\r");                               // print out a colon
    clockDisplay_updateTimeDisplay(false);                  // update our screen to default values
}

// Updates the time display with latest time, making sure to update only those digits that
// have changed since the last update.
// if forceUpdateAll is true, update all digits.
void clockDisplay_updateTimeDisplay(bool forceUpdateAll)
{
    for (uint32_t i = 0; i < NUM_OF_DIGITS ; i++)                           // make a loop through our index-es-es-es (indexies?) (windixie's?)
    {
        if ((time_str_cur[i] != time_str_next[i]) || forceUpdateAll)        // check if our value needs some up-datin'
        {
            display_setCursor(TEXT_X_COORDINATE[i], TEXT_Y_COORDINATE);     // set our cursor at the values needed
            display_setTextColor(DISPLAY_BLACK);                            // we're writing in black
            display_println(time_str_cur[i]);                               // blank out by writing a black char over top.

            time_str_cur[i] = time_str_next[i];                             // load in the new value (for updating purposes)
            display_setTextColor(DISPLAY_GREEN);                            // we're writing in green
            display_setCursor(TEXT_X_COORDINATE[i], TEXT_Y_COORDINATE);     // set our cursor at the values needed
            display_println(time_str_cur[i]);                               // write the new char
        }
    }
}

// Reads the touched coordinates and performs the increment or decrement,
// depending upon the touched region.
void clockDisplay_performIncDec()
{
    bool increment_it = false;                              // this is just the 'add or subtract?' clause
    int16_t x = 0;                                          //variables to hold x touch data
    int16_t y = 0;                                          //variables to hold y touch data
    uint8_t z;                                              //variables to hold touch pressure
    display_getTouchedPoint(&x, &y, &z);                    // call the low-level display function
    if(y < (DISPLAY_HEIGHT / CLOCKDISPLAY_NUM_OF_COLUMS)) increment_it = true;        // top half of screen
    if(x > (CLOCK_DISPLAY_SEC_ROW_NUM * DISPLAY_WIDTH / CLOCKDISPLAY_NUM_OF_ROWS))    // seconds pressed
        incDec_seconds(increment_it);                                       // call incdec seconds
    else if(x > (DISPLAY_WIDTH / CLOCKDISPLAY_NUM_OF_ROWS))                 // if it's in the minute region
        incDec_minutes(increment_it);                                       // if it's not high enough to be in seconds...
    else incDec_hours(increment_it);                                        // it too low for both call hours funct.

    sprintf(time_str_next, "%2hd%02hd%02hd", hours, minutes, seconds);      // do some magic. I mean concat. the time values
    clockDisplay_updateTimeDisplay(false);                                  //update our clock screen
}

// Advances the time forward by 1 second and update the display.
void clockDisplay_advanceTimeOneSecond()
{
    if(seconds < (NUM_OF_SEC_IN_A_MIN - 1))         // if it's less than the max value
        seconds++;                                  // increment.
    else                                                    // There was one in the bed
    {
        seconds = 0;                                // we reset it.
        if(minutes < (NUM_OF_MIN_IN_AN_HOUR - 1))   // check if our minutes roll over.
            minutes ++;                             // reset it
        else                                                // And the Little one said, "Roll Over, Roll Over"
        {
            minutes = 0;                            // reset it
            if(hours < NUM_OF_HOURS_IN_CLOCK)       // check our hours reg
                hours ++;                           // increment hours
            else                                            // They all rolled over and one fell off...
                hours = 1;                          // not a magic number, right?
        }
    }
    sprintf(time_str_next, "%2hd%02hd%02hd", hours, minutes, seconds);      // do some magic. I mean concat. the time values

    clockDisplay_updateTimeDisplay(false);          // false because we don't want to force update all.
}

// Run a test of clock-display functions.
void clockDisplay_runTest()
{
    for(uint32_t i = 0; i < SELF_TEST_LOOPS; i++)       //while my loop-age keeps runnin'
    {
        clockDisplay_advanceTimeOneSecond();            // test our incrementor-inator!
        utils_msDelay(SELF_TEST_DELAY);                 // introduce a fake-time delay to test it.
    }
}
