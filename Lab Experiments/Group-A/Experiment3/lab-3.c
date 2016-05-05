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
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))
#define GPIO_PF1_M1PWM5         0x00050405
#define GPIO_PF2_M1PWM6         0x00050805
#define GPIO_PF3_M1PWM7         0x00050C05
#define PWM_FREQUENCY 55
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
volatile uint32_t ui32Load;
volatile uint32_t ui32PWMClock;
volatile uint8_t ui8Adjust_red = 254;
volatile uint8_t ui8Adjust_blue = 254;
volatile uint8_t ui8Adjust_green = 254;

/* 0->idle, 1->press, 2->release */
uint8_t sw1state = 0;
uint8_t sw2state = 0;

uint8_t led = 2;
uint8_t sw2Status = 0;

/*Variable for delay in auto made*/
uint8_t autoDelay = 20;

/*Mode 1->Auto 0->Manual*/
uint8_t mode = 1;

uint8_t manualMode = 1;
uint8_t manualState = 0;
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

* Function Name: manualSetup()

* Input: none

* Output: none

* Description: Enable PWM peripherals, generate three different Pulses and configure PWM output to drive LEDs

* Example Call: manualSetup();

*/

void manualSetup(void)
{
	led = 0;
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, led);

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust_red * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust_blue * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust_green * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);
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
			return flag;
		} else if (sw1state == 0) {
			sw1state = 1;
			flag = 0;
		}
		else if (sw1state>1 && sw1state<200)
		{
			sw1state++;
		}
		else if (sw1state>=200)
		{
			flag=2;
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
			return flag;
		} else if (sw2state == 0) {
			sw2state = 1;
			flag = 0;
		}
		else if (sw2state>1 && sw2state<200)
		{
			sw2state++;

		}
		else if (sw2state>=200)
		{
			flag=2;
		}
	}
	return flag;
}



/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: none

* Description: Interrupt Handler. It is called when timer interrupts occur by ISR automatically. Intensity and value of LED's are also changed here

* Example Call: Timer0IntHandler();

*/


void Timer0IntHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	char sw1val = detectKeyPressSW1();
	char sw2val = detectKeyPressSW2();
	if (sw1val == 1) {
		autoDelay--;
		if(autoDelay<5) autoDelay=5;
		if(manualMode == 1 && !mode)
		{
			ui8Adjust_red-=10;
			if (ui8Adjust_red < 11)
			{
				ui8Adjust_red = 11;
			}
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust_red * ui32Load / 1000);
		}
		if(manualMode == 2 && !mode)
		{
			ui8Adjust_blue-=10;
			if (ui8Adjust_blue < 11)
			{
				ui8Adjust_blue = 11;
			}
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust_blue * ui32Load / 1000);
		}
		if(manualMode == 3 && !mode)
		{
			ui8Adjust_green-=10;
			if (ui8Adjust_green < 11)
			{
				ui8Adjust_green = 11;
			}
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust_green * ui32Load / 1000);
		}
	}

	if (sw2val == 1) {
		autoDelay++;
		if(autoDelay>50) autoDelay=50;
		if(manualMode == 1 && !mode)
		{
			ui8Adjust_red+=10;
			if (ui8Adjust_red > 240)
			{
				ui8Adjust_red = 240;
			}
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust_red * ui32Load / 1000);
		}
		if(manualMode == 2 && !mode)
		{
			ui8Adjust_blue+=10;
			if (ui8Adjust_blue > 240)
			{
				ui8Adjust_blue = 240;
			}
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust_blue * ui32Load / 1000);
		}
		if(manualMode == 3 && !mode)
		{
			ui8Adjust_green+=10;
			if (ui8Adjust_green > 240)
			{
				ui8Adjust_green = 240;
			}
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust_green * ui32Load / 1000);
		}
	}

	if (sw1val == 2 && mode && sw2val==2) {
	manualSetup();
		mode = 0;
		sw1state=0;sw2state=0;
	}
	else if(!mode && sw2val == 2 && sw1val == 2) {manualMode = 3;manualState=0;}
	else if (!mode && sw2val == 2)
	{
		if(sw1val==1)
		{
			manualState++;
			if(manualState == 1) { manualMode = 1;}
			else if(manualState == 2) {manualMode = 2;}
		}
	}
	else if(!mode) manualState=0;
}



int main(void)
{
	//Do setup
	setup();
	ledPinConfig();
	switchPinConfig();
	//Enable timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	uint32_t ui32Period;

	ui32Period = (SysCtlClockGet() / 100) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, led);

	while(1)
	{
		//vode for automode
		if(mode)
		{
			SysCtlDelay(autoDelay*200000);
			if(led == 2) led = 10;
			else if(led == 10) led = 8;
			else if(led == 8) led = 12;
			else if(led == 12) led = 4;
			else if (led == 4) led = 6;
			else if(led == 6) led = 2;
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 , led);
		}
	}


}
