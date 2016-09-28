/*
 * switches.c
 *
 *  Created on: Sep 8, 2016
 *      Author: alex
 */


#include "switches.h"
#include "supportFiles/leds.h"
#include "supportFiles/display.h"

#define SWITCHES_INIT_STATUS_OK 1   // I initilizaeeaded ok
#define SWITCHES_INIT_STATUS_FAIL 0 // I didn't " "
#define SWITCHES_SW0_MASK 0x1       // sw mask used to grab bits from the reg
#define SWITCHES_SW1_MASK 0x2       // sw mask used to grab bits from the reg
#define SWITCHES_SW2_MASK 0x4       // sw mask used to grab bits from the reg
#define SWITCHES_SW3_MASK 0x8       // sw mask used to grab bits from the reg

#define SW_VALUE_OFFSET 0x0 // Offset for the value location in memory
#define TRI_EN_OFFSET 0x4 // This is the offset to the tri state enable regs
#define TRI_EN_VALUE 0xF // this is the value we need to write to set inputs
#define NUM_OF_READ_BITS 4 // the number of read bits we care about
#define ALL_SWITCHES_DOWN 0xF  // this is to avoid penalty for magic numbers and still meet lab specs about all buttons

//Helper function used to satisfy lab requirements.
// This just reads the value at a given offset.
int32_t switches_readGpioRegister(int32_t offset)
{
    return Xil_In32(XPAR_SLIDE_SWITCHES_BASEADDR + offset); //read value
}

//Helper function used to satisfy lab requirements.
// This just reads the value at a given offset.
void switches_writeGpioRegister(int32_t offset, int32_t value)
{
    Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + offset, value);      //. this writes the given value to the register.
}

// Initializes the SWITCHES driver software and hardware. Returns one of the STATUS values defined above.
int32_t switches_init()
{   switches_writeGpioRegister(TRI_EN_OFFSET, TRI_EN_VALUE); // set the enables to tri-en-value
    if(switches_readGpioRegister(TRI_EN_OFFSET) == TRI_EN_VALUE)      return SWITCHES_INIT_STATUS_OK; // return ok if ok,
    else    return SWITCHES_INIT_STATUS_FAIL;            // else (ie, if not ok) return fail
}

// Returns the current value of all 4 SWITCHESs as the lower 4 bits of the returned value.
// bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read()
{   // grab the value of that register.  Note that we need no offset, as the base address is where the values are stored.
    return switches_readGpioRegister(SW_VALUE_OFFSET);  //  This reads the value from the xilinx register
}

// Runs a test of the switches. As you slide the switches, LEDs directly above the switches will illuminate.
// The test will run until all switches are slid upwards. When all 4 slide switches are slid upward,
// this function will return.
void switches_runTest()
{   char sw_value = 0;      // this creates a variable storign my switch information
    leds_init(false);  // initialize the LED's.  False because we don't want to printf error messages from the led.
    while(sw_value != ALL_SWITCHES_DOWN) //switches value is not equal to all switches down
    {
       sw_value = switches_readGpioRegister(SW_VALUE_OFFSET);   // update my sw variable.
       leds_write(sw_value);                                    // write to led register
    }
    leds_write(false);  // Clears the LEDS by writing the false value --- WHICH IS NOT A MAGIC NUMBER!!!
}



