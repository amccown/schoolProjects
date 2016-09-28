/*
 * clockControl.c
 *
 *  Created on: Sep 13, 2016
 *      Author: alex
 */


#include <stdio.h>
#include "clockControl.h"
#include "clockDisplay.h"
#include "supportFiles/leds.h"
#include "supportFiles/display.h"



#define AUTO_COUNTER_MAX 9             // 10 -1 for determining delay before auto incDec-ing
static uint8_t autoCounter = 0;        // our counter for the 'waiting before auto inc' business

#define RATE_COUNTER_MAX 1              // 2 - 1number of ticks for each IndDec call
static uint8_t rateCounter = 0;        // our counter for after autoCounter's been hit.

#define MS_COUNTER_MAX 19               // 20-1 for 1 full second
static uint8_t msCounter = 0;           // our counter for watching the second tick over.

// States for the controller state machine.
enum clockControl_st_t {
    init_st,                 // Start here, stay in this state for just one tick.
    never_touched_st,        // Wait here until the first touch - clock is disabled until set.
    waiting_for_touch_st,    // waiting for touch, clock is enabled and running.
    ad_timer_running_st,     // waiting for the touch-controller ADC to settle.
    auto_timer_running_st,   // waiting for the auto-update delay to expire
                                 // (user is holding down button for auto-inc/dec)
    rate_timer_running_st,   // waiting for the rate-timer to expire to know when to perform the auto inc/dec.
    rate_timer_expired_st,   // when the rate-timer expires, perform the inc/dec function.
    add_second_to_clock_st   // add a second to the clock time and reset the ms counter.
} currentState = init_st;

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void clockControl_debugStatePrint() {
  static clockControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = currentState;     // keep track of the last state that you were in.
//    printf("msCounter:%d\n\r", msCounter);
    switch(currentState) {            // This prints messages based upon the state that you were in.
      case init_st:                                 // name of state
        printf("init_st\n\r");                      // Message to print
        break;
      case never_touched_st:                                 // name of state
        printf("never_touched_st\n\r");                      // Message to print
        break;
      case waiting_for_touch_st:                                 // name of state
        printf("waiting_for_touch_st\n\r");                      // Message to print
        break;
      case ad_timer_running_st:                                 // name of state
        printf("ad_timer_running_st\n\r");                      // Message to print
        break;
      case auto_timer_running_st:                                 // name of state
        printf("auto_timer_running_st\n\r");                      // Message to print
        break;
      case rate_timer_running_st:                                 // name of state
        printf("rate_timer_running_st\n\r");                      // Message to print
        break;
      case rate_timer_expired_st:                                 // name of state
        printf("rate_timer_expired_st\n\r");                      // Message to print
        break;
      case add_second_to_clock_st:                                 // If we're in state...
        break;                                                      // Here we don't printf, cause that'd suck.
     }
  }
}


/// Pull the lever, Kronk!
void clockControl_tick() {
  // Perform state action first.
  switch(currentState) {
    case init_st:                                 // If we're in state...
      break;
    case never_touched_st:                                 // If we're in state...
        autoCounter = 0;                                   // initialize the autoCounter
        rateCounter = 0;                                   // initialize the rate counter
      break;
    case waiting_for_touch_st:                                 // If we're in state...
        autoCounter = 0;                                        //reset the counter
        rateCounter = 0;                                        // reset rate counter
        // increment the msCounter bob by one, ie, counting number of ticks
        msCounter++;
      break;
    case ad_timer_running_st:                                 // If we're in state...
      break;
    case auto_timer_running_st:                                 // If we're in state...
        autoCounter++;                                          // increment the auto counter
      break;
    case rate_timer_running_st:                                 // If we're in state...
        rateCounter++;                                          // increment the rate counter
      break;
    case rate_timer_expired_st:                                 // If we're in state...
        rateCounter = 0;                                        // re-initialized the counter
      break;
    case add_second_to_clock_st:                                 // If we're in state...
        msCounter = 0;                                          // re-initialize the ms counter
      break;
     default:                                 // The default, 'just in case'
      printf("clockControl_tick state action: hit default\n\r");    //print an error
      break;
  }

  // Perform state update next.
  switch(currentState) {
    case init_st:                                 // If we're in state init
        currentState = never_touched_st;        // here we go to the waiting for touch state
      break;
    case never_touched_st:                                 // If we're in state...
        if(display_isTouched())                             // if someone's pushed a button
        {
            display_clearOldTouchData();                // get ready for the first touch data
            currentState = ad_timer_running_st;         // not a real timer, but didn't want to change it
        }
      break;
    case waiting_for_touch_st:                              // If we're in state...
        if(msCounter >= MS_COUNTER_MAX)
            currentState = add_second_to_clock_st;          // go to the add a second state
        if(display_isTouched())                             // if we've been hit
        {
            display_clearOldTouchData();                    // get ready for another touch
            currentState = ad_timer_running_st;             // go to the counter running state
        }
      break;
      //NOTE: there is no delay here because my tick happens with enough delay to allow the ADC to settle
    case ad_timer_running_st:                               // If we're in state...
        if(!display_isTouched())                            // if they let go of the button press
        {
            clockDisplay_performIncDec();                   // call my button sensing function
            currentState = waiting_for_touch_st;            // go back to wait state
        }
        else
        {
            // go to the waiting for the auto-update delay to expire
            currentState = auto_timer_running_st;
        }
      break;
    case auto_timer_running_st:                                 // If we're in state...
        if(display_isTouched() && (autoCounter >= AUTO_COUNTER_MAX))    // if it's maxed out
        {
            clockDisplay_performIncDec();                       // call my updater
            currentState = rate_timer_running_st;               // go into the rate timer state
        }
        else if(!display_isTouched())                           // ie, if no longer touched
        {
            clockDisplay_performIncDec();                       // so we update at least once
            currentState = waiting_for_touch_st;                // go back to waiting
        }
      break;
    case rate_timer_running_st:                                 // If we're in state...
        if(display_isTouched() && rateCounter >= RATE_COUNTER_MAX)      // if we're maxed
        {
            currentState = rate_timer_expired_st;               // go to update state
        }
        else if(!display_isTouched())                           // if no longer touched
        {
            currentState = waiting_for_touch_st;                // go back to waiting
        }
      break;
    case rate_timer_expired_st:                                 // If we're in state...
        if(display_isTouched())
        {
            clockDisplay_performIncDec();                       // call with the still-pressed button
            currentState = rate_timer_running_st;               // go back to counting
        }
        else                                                    // if no longer pressed
            currentState= waiting_for_touch_st;                 // go do some more waiting
      break;
    case add_second_to_clock_st:                                // If we're in state...
        clockDisplay_advanceTimeOneSecond();                    // add our one second
        currentState = waiting_for_touch_st;                    // go back to the waiting state
      break;
    default:                                                    // The default, 'just in case'
      printf("clockControl_tick state update: hit default\n\r");    //print an error
      break;
  }
}
