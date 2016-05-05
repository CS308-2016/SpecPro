/*

* Author: Texas Instruments 

* Editted by: Saurav Shandilya, Vishwanathan Iyer 
	      ERTS Lab, CSE Department, IIT Bombay

* Description: This code will familiarize you with using GPIO on TMS4C123GXL microcontroller. 

* Filename: lab-1.c 

* Functions: setup(), ledPinConfig(), switchPinConfig(), main()  

* Global Variables: none

*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

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
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // 4?
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

/* 0->idle, 1->press, 2->release */
uint8_t sw1state = 0;
uint8_t sw2state = 0;

uint8_t led = 8;
uint8_t sw2Status = 0;


/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: none

* Description: Interrupt Handler. It is called when timer interrupts occur by ISR automatically

* Example Call: Timer0IntHandler();

*/

void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	if (detectKeyPressSW1()) {
		if (led == 2) led = 4;
		else if (led == 4) led = 8;
		else if (led == 8) led = 2;
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 , led);
	}

	if (detectKeyPressSW2()) {
		sw2Status++;
	}
}

/*

* Function Name: detectKeyPressSW1()

* Input: none

* Output: Returns 1 if Switch 1 has been pressed

* Description: Detects if Switch 1 has been pressed

* Example Call: detectKeyPressSW1();

*/

unsigned char detectKeyPressSW1()
{
	unsigned char flag = 0;
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) {
		sw1state = 0;
		flag = 0;
	}
	else {
		if (sw1state == 1) {
			sw1state = 2;
			flag = 1;
		} else if (sw1state == 0) {
			sw1state = 1;
			flag = 0;
		}
	}

	return flag;
}


/*

* Function Name: detectKeyPressSW2()

* Input: none

* Output: Returns 1 if Switch 2 has been pressed

* Description: Detects if Switch 2 has been pressed

* Example Call: detectKeyPressSW2();

*/

unsigned char detectKeyPressSW2()
{
	unsigned char flag = 0;
	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) {
		sw2state = 0;
		flag = 0;
	}
	else {
		if (sw2state == 1) {
			sw2state = 2;
			flag = 1;
		} else if (sw2state == 0) {
			sw2state = 1;
			flag = 0;
		}
	}

	return flag;
}


int main(void)
{
	///Do All the Configuration
	setup();
	ledPinConfig();
	switchPinConfig();
	//Config for timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	//ui32period:governs the duration of timer interrupts
	uint32_t ui32Period;

	ui32Period = (SysCtlClockGet() / 100) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	//Enable timers
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

	while(1)
	{

	}


}
