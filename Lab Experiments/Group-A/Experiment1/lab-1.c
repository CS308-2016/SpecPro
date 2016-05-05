/*

* Author: Texas Instruments 

* Editted by: Saurav Shandilya, Vishwanathan Iyer 
	      ERTS Lab, CSE Department, IIT Bombay

* Description: This code will familiarize you with using GPIO on TMS4C123GXL microcontroller. 

* Filename: lab-1.c 

* Functions: setup(), ledPinConfig(), switchPinConfig(), main()  

* Global Variables: none

*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/
	

/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals  

* Example Call: setup();

*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

* Function Name: ledPinConfig()

* Input: none

* Output: none

* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

* Example Call: ledPinConfig();

*/

void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}

/*

* Function Name: switchPinConfig()

* Input: none

* Output: none

* Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.

* Example Call: switchPinConfig();

*/
void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;
	
	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

int main(void)
{

	//Do all the configuration
	setup();
	ledPinConfig();
	switchPinConfig();
	//ui8LED: represents colour of the led
	uint8_t ui8LED = 8;
	uint8_t sw1Status = 0;
	//sw@status: its the variable under observation: to be increased by 1 everytime switch 2 is pressed
	uint8_t sw2Status = 0;
	uint8_t sw2Help = 0;

	while(1) {
		//chech if switch 2 is pressed
		if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) {
			sw2Help=0;
		}
		else if (!sw2Help) {
			sw2Help=1;
			//increment variable by 1
			sw2Status++;
		}
		//check if switch 1 is pressed
		if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) {
			sw1Status = 0;
			//if not pressed switch off Led
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 , sw1Status);
		}
		else {
			//if switch 1 is pressed change ui8led value
			if(sw1Status==0) {
				sw1Status=1;
				if(ui8LED==8) ui8LED=2;
				else ui8LED*=2;
			}
			//Assign color corresponding to ui8LED to the LEDs
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 , ui8LED);
		}
	}
}
