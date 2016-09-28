/*
 * buttons.c
 *
 *  Created on: Sep 7, 2016
 *      Author: alex
 */

#include "buttons.h"
#include "supportFiles/display.h"

#define BTN_VALUE_OFFSET 0x0  //null value for the offset where the button
#define TRI_EN_OFFSET 0x4 // This is the offset to the tri state enable regs
#define TRI_EN_VALUE 0xF // this is the value we need to write to set inputs
#define ALL_BUTTONS_DOWN 0xF  // this is to avoid penalty for magic numbers and still meet lab specs about all buttons

#define ORIGIN_COORDINATE 0         // This is for zeroing out the x and y of the screen
#define DISPLAY_WIDTH 320           // num of x pixels
#define DISPLAY_HEIGHT 240          // num of y pixels

#define WIDTH_OF_BUTTON_DISPLAYS (DISPLAY_WIDTH / 4)  // how wide we want the displays.  Here we have 4, so I figured 1/4 was good.
#define HEIGHT_OF_BUTTON_DISPLAYS (DISPLAY_HEIGHT / 2)  // again, how tall we want them.  it looks like Hutch's stuff is about half
#define BTN2_CORNER WIDTH_OF_BUTTON_DISPLAYS            // this is the top left corner of the the button2 display
#define BTN1_CORNER (2*WIDTH_OF_BUTTON_DISPLAYS)        // this is the top left corner of the button 1 display
#define BTN0_CORNER (3*WIDTH_OF_BUTTON_DISPLAYS)        // top left corner of btn 0
#define TEXT_OFFSET_Y (HEIGHT_OF_BUTTON_DISPLAYS / 2)                     // This is how many lines down we want our text
#define BTN3_TEXT_OFFSET_X (WIDTH_OF_BUTTON_DISPLAYS / 4)                // how many char's over btn3 text is
#define BTN2_TEXT_OFFSET_X (5* WIDTH_OF_BUTTON_DISPLAYS / 4)                // how many char's over btn3 text is
#define BTN1_TEXT_OFFSET_X (9* WIDTH_OF_BUTTON_DISPLAYS / 4)                // how many char's over btn3 text is
#define BTN0_TEXT_OFFSET_X (13* WIDTH_OF_BUTTON_DISPLAYS / 4)                 // how many char's over btn3 text is

#define BUTTONS_INIT_STATUS_OK 1   // ok status
#define BUTTONS_INIT_STATUS_FAIL 0  // fail status
#define BUTTONS_BTN0_MASK 0x1   //  mask for getting button 0
#define BUTTONS_BTN1_MASK 0x2   //  mask for getting button 1
#define BUTTONS_BTN2_MASK 0x4   //  mask for getting button 2
#define BUTTONS_BTN3_MASK 0x8   // mask for getting button 3
#define BUTTONS_ALL_MASK 0xF   // Mask used fo getting only the button values


//Helper function used to satisfy lab requirements.
// This just reads the value at a given offset.
int32_t buttons_readGpioRegister(int32_t offset)
{
    return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset); //read value
}

//Helper function used to satisfy lab requirements.
// This just reads the value at a given offset.
void buttons_writeGpioRegister(int32_t offset, int32_t value)
{
    Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);      //. this writes the given value to the register.
}

// Initializes the button driver software and hardware. Returns one of the defined status values (above).
int32_t buttons_init()
{   buttons_writeGpioRegister(TRI_EN_OFFSET, TRI_EN_VALUE); // set the enables to tri-en-value
    if(buttons_readGpioRegister(TRI_EN_OFFSET) == TRI_EN_VALUE)      return BUTTONS_INIT_STATUS_OK; // return ok if ok,
    else    return BUTTONS_INIT_STATUS_FAIL;            // else (ie, if not ok) return fail
}

// Returns the current value of all 4 buttons as the lower 4 bits of the returned value.
// bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read()
{
    return buttons_readGpioRegister(BTN_VALUE_OFFSET); //Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR);  // This part calls a xilinx function for reading registers
}

// Runs a test of the buttons. As you push the buttons, graphics and messages will be written to the LCD
// panel. The test will until all 4 pushbuttons are simultaneously pressed.
void buttons_runTest()
{   int32_t button_value = buttons_read();      // create a variable with button info.
    int32_t button0_value;  // create a variable who holds my button values
    int32_t button0_value_last = 0;                              // create a variable who holds my last button values
    int32_t button1_value;  // create a variable who holds my button values
    int32_t button1_value_last = 0;                              // create a variable who holds my last button values
    int32_t button2_value;  // create a variable who holds my button values
    int32_t button2_value_last = 0;                              // create a variable who holds my last button values
    int32_t button3_value;  // create a variable who holds my button values
    int32_t button3_value_last = 0;                              // create a variable who holds my last button values

    display_setTextColor(DISPLAY_WHITE);                    // Make the text white.
    display_fillScreen(DISPLAY_BLACK);                     // Blank the screen.

    while(button_value!= ALL_BUTTONS_DOWN)                          // while I have not put all buttons down
    {   // This is a pseudo case statement block to detect button presses.  It uses masks to identify which buttons are pressed
        button_value = buttons_read();                              // get some data!  Used to re-sync the value
        button0_value = button_value& BUTTONS_BTN0_MASK;            // set the button value for the individual button flag
        button1_value = button_value& BUTTONS_BTN1_MASK;            // set the button value for the individual button flag
        button2_value = button_value& BUTTONS_BTN2_MASK;            // set the button value for the individual button flag
        button3_value = button_value& BUTTONS_BTN3_MASK;            // set the button value for the individual button flag

        if(button0_value != button0_value_last)                       // "If there's been a change in button values..."
        {
            button0_value_last = button0_value;                           // Re-sync the button variable for the last while loop iteration.

            if(button0_value)                              // Applies a mask and compares to the mask to see if that button is pressed.
            {   display_fillRect(BTN0_CORNER, ORIGIN_COORDINATE, WIDTH_OF_BUTTON_DISPLAYS, HEIGHT_OF_BUTTON_DISPLAYS, DISPLAY_YELLOW);      //Draw our colored brick
                display_setCursor(BTN0_TEXT_OFFSET_X, TEXT_OFFSET_Y);           // set cursor for writing button identifier
                display_println("BTN0");                                        // print out the name of the button
            }
            else
            {
                display_fillRect(BTN0_CORNER, ORIGIN_COORDINATE, WIDTH_OF_BUTTON_DISPLAYS, HEIGHT_OF_BUTTON_DISPLAYS, DISPLAY_BLACK);      //Draw our colored brick
            }
        }

        if(button1_value != button1_value_last)     // check if there's been a change in the button value
        {
            button1_value_last = button1_value;                           // Re-sync the button variable
            if(button1_value)                               // Applies a mask and compares to the mask to see if that button is pressed.
            {   display_fillRect(BTN1_CORNER, ORIGIN_COORDINATE, WIDTH_OF_BUTTON_DISPLAYS, HEIGHT_OF_BUTTON_DISPLAYS, DISPLAY_GREEN);      //Draw our colored brick
                display_setCursor(BTN1_TEXT_OFFSET_X, TEXT_OFFSET_Y);           // set cursor for writing button identifier
                display_println("BTN1");                                        // print out the name of the button
            }
            else
            {

                 display_fillRect(BTN1_CORNER, ORIGIN_COORDINATE, WIDTH_OF_BUTTON_DISPLAYS, HEIGHT_OF_BUTTON_DISPLAYS, DISPLAY_BLACK);      //Draw our colored brick
            }
        }

        if(button2_value != button2_value_last)        // check if there's been a change in the button value
        {
            button2_value_last = button2_value;                           // Re-sync the button variable
            if(button2_value)                               // Applies a mask and compares to the mask to see if that button is pressed.
            {   display_fillRect(BTN2_CORNER, ORIGIN_COORDINATE, WIDTH_OF_BUTTON_DISPLAYS, HEIGHT_OF_BUTTON_DISPLAYS, DISPLAY_RED);      //Draw our colored brick
                display_setCursor(BTN2_TEXT_OFFSET_X, TEXT_OFFSET_Y);           // set cursor for writing button identifier
                display_println("BTN2");                                        // print out the name of the button
            }
            else
            {
                display_fillRect(BTN2_CORNER, ORIGIN_COORDINATE, WIDTH_OF_BUTTON_DISPLAYS, HEIGHT_OF_BUTTON_DISPLAYS, DISPLAY_BLACK);      //Erase our colored brick
            }
        }

        if(button3_value != button3_value_last)         // check if there's been a change in the button value
        {
            button3_value_last = button3_value;                           // Re-sync the button variable
            if(button3_value)                               // Applies a mask and compares to the mask to see if that button is pressed.
            {   display_fillRect(ORIGIN_COORDINATE,ORIGIN_COORDINATE, WIDTH_OF_BUTTON_DISPLAYS, HEIGHT_OF_BUTTON_DISPLAYS,  DISPLAY_BLUE);      //Draw our colored brick
                display_setCursor(BTN3_TEXT_OFFSET_X, TEXT_OFFSET_Y);           // set cursor for writing button identifier
                display_println("BTN3");                                        // print out the name of the button
            }
            else
            {
                display_fillRect(ORIGIN_COORDINATE,ORIGIN_COORDINATE, WIDTH_OF_BUTTON_DISPLAYS, HEIGHT_OF_BUTTON_DISPLAYS,  DISPLAY_BLACK);      //Draw our colored brick
            }
        }
    }
    display_fillScreen(DISPLAY_BLACK); // this re-blanks the screen after we're done.
}


