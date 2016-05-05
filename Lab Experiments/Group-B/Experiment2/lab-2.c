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
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
*/
enum State{Idle, Press, Release};
uint8_t led = 4;
int sw2Count = 0;
enum State swState[3] = {Idle,Idle,Idle};

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

void clockConfig(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	uint32_t ui32Period = SysCtlClockGet() / 100;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -	1);
}

void interruptConfig(void) {
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE,TIMER_A);
}

unsigned char detectKeyPress(uint8_t pin, int sw) {
	switch(swState[sw]) {
	case Idle:
		if (!GPIOPinRead(GPIO_PORTF_BASE,pin)) swState[sw] = Press;
		return 0;
	case Press:
		if (GPIOPinRead(GPIO_PORTF_BASE,pin)) {
			swState[sw] = Idle;
			return 0;
		}
		else {
			swState[sw] = Release;
			return 1;
		}
	case Release:
		if (GPIOPinRead(GPIO_PORTF_BASE,pin)) swState[sw] = Idle;
		return 0;
	}
	return 0;
}

void Timer0IntHandler(void) {
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
	if (detectKeyPress(GPIO_PIN_4,1)) {
		led = (led == 2 ? 8 : led/2);
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, led);
	}
	
	if (detectKeyPress(GPIO_PIN_0,2)) {
		sw2Count++;
	}
}

int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	clockConfig();
	interruptConfig();
	
	while(1) {
	}
}
