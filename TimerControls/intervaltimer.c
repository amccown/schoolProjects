/*
 * intervaltimer.c
 *
 *  Created on: Sep 10, 2016
 *      Author: alex
 *      Notes:
 *
 *
 * This is the C file for my interval timer controller.
 *
 */
// my includes
#include <inttypes.h>
#include "supportFiles/utils.h"
#include "intervaltimer.h"
#include <stdio.h>
#include <xparameters.h>
#include <xil_io.h>


#define CASC_MASK 0x800  // This is 1000_0000_0000, ie, the 11th bit for CASC
#define UDT0_MASK 0x2  // This is 0010, ie, the location of the one bit we need to clear
#define LOAD_MASK 0x20  // this is to set the load bit
#define ENT0_MASK 0x80  // this is the EN bit
#define TLR1_CONCAT_OFFSET 32 // this is the offset for concatinating the TLR counter values
#define MAX_COUNT 0xFFFFFFFFFFFFFFFF // This is the max number of counts in a 64 bit number.

#define TCSR_INITIAL_VALUE 0x10 // inital value for the TCSR registers.  Here we're starting from 0
#define TLR_INITIAL_VALUE 0 // inital value for the TCSR registers.  Here we're starting from 0


#define TCSR0_OFFSET 0x00       // given offset for TCSR0 register
#define TLR0_OFFSET 0x04       // given offset for TLR0 register
#define TCR0_OFFSET 0x08       // given offset for TCR0 register
#define TCSR1_OFFSET 0x10       // given offset for TCSR1 register
#define TLR1_OFFSET 0x14       // given offset for TLR1 register
#define TCR1_OFFSET 0x18       // given offset for TCSR1 register


//Helper function for getting the Base address for the timer in question.
// This was initially done as a case statement, but the case statement was giving
// the default resulf when I compared the timerNumber constraint to the various
// DEVICE_ID's, so I switched to a set of ifs and it worked.
uint32_t intervalTimer_getBaseAddress(uint32_t timerNumber)        //funct. decl.
{
    uint32_t myReturnValue = 0;                     // create a variable large enough to hold address
    if(timerNumber == XPAR_AXI_TIMER_0_DEVICE_ID )             // if it's timer 0
        myReturnValue = XPAR_AXI_TIMER_0_BASEADDR;  // give back timer 0's address.
    else if(timerNumber == XPAR_AXI_TIMER_1_DEVICE_ID)               // if it's timer 1
        myReturnValue = XPAR_AXI_TIMER_1_BASEADDR;  // give timer 1's addr.
    else if(timerNumber == XPAR_AXI_TIMER_2_DEVICE_ID)               // if it's timer 2
        myReturnValue = XPAR_AXI_TIMER_2_BASEADDR;  // give timer 2's addr.
    else
        myReturnValue = XPAR_AXI_TIMER_0_BASEADDR;  // just as a 'default case'
    return myReturnValue;
}


// You must initialize the timers before you use them.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber)
{
    // Here we need to write a 0 to TCSR0 and a 0 to TCSR1
    // We also need to set the CASC bit and clear the UDT0 bit in TCSR0.
    if(timerNumber >=  XPAR_AXI_TIMER_0_DEVICE_ID && timerNumber <= TIMER_MAX_ID) // basic check
    {
        uint32_t base_addr = intervalTimer_getBaseAddress(timerNumber);     // find my address       // I think we should read what's there (+ 0 for TCSR0)
        Xil_Out32(base_addr + TCSR0_OFFSET, TCSR_INITIAL_VALUE);            // initialize TCSR0
        Xil_Out32(base_addr + TCSR1_OFFSET, TCSR_INITIAL_VALUE);            // write a blank to TCSR1
        uint32_t value_cur = (TCSR_INITIAL_VALUE | CASC_MASK);              // set CASC high, set UDT0 low
        Xil_Out32(base_addr + TCSR0_OFFSET, value_cur);                     // write it back

        return INTERVAL_TIMER_STATUS_OK;                                    // say we're ok
    }
    else                                                                    // if not between 0 and max
        return INTERVAL_TIMER_STATUS_FAIL;                                  // or not ok.
}


// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, some other value otherwise.
intervalTimer_status_t intervalTimer_initAll()
{
    intervalTimer_status_t andedSumSoFar = INTERVAL_TIMER_STATUS_OK;
    //here we make a counter that goes from 0 to 2, and call _init for each
    // all the while anding our running total with the next iteration, so as to
    // produce a 'did they all return positive' scenario.
    for(uint32_t i = 0; i <= TIMER_MAX_ID; i++)                 // set up the loop
    {
        andedSumSoFar = andedSumSoFar && intervalTimer_init(i);               // call our init function
    }
    return andedSumSoFar;
}


// This function starts the interval timer running.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber)
{
    if(timerNumber >=  XPAR_AXI_TIMER_0_DEVICE_ID && timerNumber <= TIMER_MAX_ID)         // check if they're lying to us
    {
        uint32_t base_addr = intervalTimer_getBaseAddress(timerNumber);     // find my address       // I think we should read what's there (+ 0 for TCSR0)
        uint32_t TCSR0_value_prev = Xil_In32(base_addr + TCSR0_OFFSET);     // read what's in TCSR0
        uint32_t TCSR0_value_cur  = TCSR0_value_prev | ENT0_MASK;          // set our en flag
        Xil_Out32(base_addr + TCSR0_OFFSET, TCSR0_value_cur);               // write our new value
    }
}


// This function stops the interval timer running.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber)
{
    if(timerNumber >=  XPAR_AXI_TIMER_0_DEVICE_ID && timerNumber <= TIMER_MAX_ID)       // check if they're lying to us
    {
        uint32_t base_addr = intervalTimer_getBaseAddress(timerNumber);     // find my address
        uint32_t TCSR0_value_prev = Xil_In32(base_addr + TCSR0_OFFSET);     // read what's in TCSR0
        uint32_t TCSR0_value_cur  = TCSR0_value_prev & (~ENT0_MASK);          // clear our en flag
        Xil_Out32(base_addr + TCSR0_OFFSET, TCSR0_value_cur);               // write our new value
    }
}


// This function resets the interval timer.
// timerNumber indicates which timer should reset.
// NOTE : I'm assuming this means load in 0, then re-initialize (to clear the load flag)
void intervalTimer_reset(uint32_t timerNumber)
{
    if(timerNumber >=  XPAR_AXI_TIMER_0_DEVICE_ID && timerNumber <= TIMER_MAX_ID)         // check if they're lying to us
    {
        uint32_t base_addr = intervalTimer_getBaseAddress(timerNumber);     // find my address       // I think we should read what's there (+ 0 for TCSR0)
        Xil_Out32(base_addr + TLR0_OFFSET, TLR_INITIAL_VALUE);              // clear the TLR0 reg
        uint32_t TCSR0_value_prev = Xil_In32(base_addr + TCSR0_OFFSET);     // read what's in TCSR0
        uint32_t TCSR0_value_cur  = TCSR0_value_prev | LOAD_MASK;          // set our load flag
        Xil_Out32(base_addr + TCSR0_OFFSET, TCSR0_value_cur);               // write our new value

        Xil_Out32(base_addr + TLR1_OFFSET, TLR_INITIAL_VALUE);              // clear the TLR1 reg

        uint32_t TCSR1_value_prev = Xil_In32(base_addr + TCSR1_OFFSET);     // read what's in TCSR1
        uint32_t TCSR1_value_cur  = TCSR1_value_prev | LOAD_MASK;          // set our load flag
        Xil_Out32(base_addr + TCSR1_OFFSET, TCSR1_value_cur);               // write our new value
        intervalTimer_init(timerNumber);                                    // re-initialize the timer
    }
}


// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll()
{
    //here we make a counter that goes from 0 to 2, and call _reset for each
    for(uint32_t i = 0; i <= TIMER_MAX_ID; i++)       // set up the loop
    {
        intervalTimer_reset(i);  // call our init function
    }
}

// Runs a test on a single timer as indicated by the timerNumber argument.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber)
{
    if(timerNumber >=  XPAR_AXI_TIMER_0_DEVICE_ID && timerNumber <= TIMER_MAX_ID)         // check if they're lying to us
    {
        if(intervalTimer_init(timerNumber))                                 // if our init returns true
        {
             intervalTimer_start(timerNumber);
             uint32_t base_addr = intervalTimer_getBaseAddress(timerNumber);     // find my address       // I think we should read what's there (+ 0 for TCSR0)

             uint32_t tcr0_unsure = Xil_In32(base_addr + TCR0_OFFSET);  // read the value
             uint32_t tcr0 = Xil_In32(base_addr + TCR0_OFFSET); // read again
             if(tcr0_unsure != tcr0)
             {
                 return INTERVAL_TIMER_STATUS_OK;
             }
        }
    }
    return INTERVAL_TIMER_STATUS_FAIL;
}

// Convenience function that invokes test on all interval timers.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
intervalTimer_status_t intervalTimer_testAll()
{
    intervalTimer_status_t intervalTimer_test_results = INTERVAL_TIMER_STATUS_OK;        // initialize a variable to the "i'm good" status
    //here we make a counter that goes from 0 to 2, and call _reset for each
    for(uint32_t i = 0; i <= TIMER_MAX_ID; i++)       // set up the loop
    {
        // call our init function, and and the results for a running total like the init funct.
        intervalTimer_test_results = intervalTimer_test_results && intervalTimer_test(i);
    }
    return intervalTimer_test_results;                  //return what we got.
}


// Once the interval timer has stopped running, use this function to
// ascertain how long the timer was running.
// The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber)
{

    if(timerNumber >=  XPAR_AXI_TIMER_0_DEVICE_ID && timerNumber <= TIMER_MAX_ID)
    {
        uint32_t base_addr = intervalTimer_getBaseAddress(timerNumber);     // find my address
        uint64_t tcr1_unsure = Xil_In32(base_addr + TCR1_OFFSET);       // my unsure value for the upper 32 bits
        uint32_t tcr0 = Xil_In32(base_addr + TCR0_OFFSET);              // bring in my value (lower 32)
        uint64_t tcr1 = Xil_In32(base_addr + TCR1_OFFSET);              // bring in my other value (upper 32)
        if(tcr1_unsure != tcr1)
            tcr0 = Xil_In32(base_addr + TCR0_OFFSET);                   // Re-read the lower 32 bits
        uint64_t tcr_concat = (tcr1 << TLR1_CONCAT_OFFSET)  | tcr0;  // concatenate those two values in one big number

        // get my clock rate from the #defines in the xparam file
        uint32_t axi_clock_rate;
        switch(timerNumber)                             // based on what timer number we're working on
        {
        case XPAR_AXI_TIMER_0_DEVICE_ID :               // if it's timer 0
            axi_clock_rate = XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ;  // give back timer 0's rate.
        case XPAR_AXI_TIMER_1_DEVICE_ID :               // if it's timer 1
            axi_clock_rate = XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;  // give timer 1's rate.
        case XPAR_AXI_TIMER_2_DEVICE_ID :               // if it's timer 2
            axi_clock_rate = XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ;  // give timer 2's rate.
        }

        // THIS IS NOT A MAGIC NUMBER.  I'm just using 1.0 to cast to a double so it can perform the division.
        double axi_clock_period = 1.0 / axi_clock_rate;         // calculate our clock period (ie, 10ns for 100_000_00hz)

        double time_value = (tcr_concat)*axi_clock_period;  //  our time value
        return time_value;                                  // return that value
    }
    else
        return TIMER_NUM_ERROR;                             // this is just so we know if it didn't work .
}

