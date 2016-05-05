/**
* Team Id: SPEC PRO
* Author List: Ramprakash K, Prateesh Goyal
* Filename: specPro.c
* Description: Gets reading from the proximity sensors and determines a speed violation
* 			which is printed to the UART output stream
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <driverlib/interrupt.h>
#include <driverlib/timer.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/tm4c123gh6pm.h>

#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

/**
 * Function Name : UARTStrPut
 * Logic : Prints the string to UART output stream
 */
void UARTStrPut(char a[])
{
	int i;
	for (i = 0; a[i] != '\0'; i++)
		UARTCharPut(UART0_BASE, a[i]);
}

/**
 * Function Name : UARTIntPut
 * Logic : Prints the integer to UART output stream
 * 		(Converts integer to string and calls UARTStrPut)
 */
void UARTIntPut(int i){
	char temp[20], a[20];
	int l = 0, m = 0, j;
	if (i < 0) {
		i = -i;
		a[0] = '-';
		m = 1;
	} else if (i == 0) {
		a[0] = '0';
		a[1] = '\0';
		UARTStrPut(a);
	}
	while (i > 0) {
		temp[l] = '0' + (i%10);
		i /= 10;
		l++;
	}
	a[l+m] = temp[l] = '\0';
	for (j = 0; j != l; j++)
		a[m+j] = temp[l-1-j];
	UARTStrPut(a);
}

uint32_t ui32ADC01Value[4];		//! ui32ADC01Value	: Array of 4 values obtained from ADC 1
uint32_t ui32ADC02Value[4];		//! ui32ADC02Value	: Array of 4 values obtained from ADC 2
volatile uint32_t t1,t2;		//! t1, t2 		: Average values obtained from ADC 1 and 2
int back, front;				//! back, front : De-Bounce timers of the 2 proximity sensors (5 x Clock cycles)
int curtime, chalu;				//! curtime		: Number of clock cycles that has passed since the vehicle passed the back proximity sensor
								//! chalu		: Flag whether a vehicle has passed the back proximity sensor or not

/**
 * Function Name : TimerIntHandler
 * Logic : Handles the timer interrupt
 * 		- Checks if a vehicle crossed the back proximity sensor, if yes, start timer "curtime"
 * 		- Checks if the vehicle crossed the front proximity sensor, if yes, calculate the speed (based on distance and curtime)
 * 		- Prints the speed of the moving vehicle to UART output stream
 */
void TimerIntHandler(void) {
	//! If vehicle is in between the 2 proximity sensors, increase the time.
	if (chalu == 1) {
		curtime++;
		if (curtime > 750) {
			curtime = 0;
			chalu = 0;
		}
	}

	//! Get value from ADC 1 (back proximity sensor)
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ROM_ADCIntClear(ADC0_BASE, 1);
	ROM_ADCProcessorTrigger(ADC0_BASE, 1);
	while(!ROM_ADCIntStatus(ADC0_BASE, 1, false)) {}
	ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC01Value);
	t1 = (ui32ADC01Value[0] + ui32ADC01Value[1] + ui32ADC01Value[2] + ui32ADC01Value[3] + 2)/4;
	//! If back proximity sensor records a reading, start waiting for the front to record
	if (chalu == 0) {
		if (t1 < 50) {
			back = -5;
		} else {
			if (back < 0) back++;
			else if (back == 0) {
				back = 1;
				curtime = 0;
				chalu = 1;
			}
		}
	}
	
	//! Get value from ADC 2 (front proximity sensor)
	ROM_ADCIntClear(ADC0_BASE, 2);
	ROM_ADCProcessorTrigger(ADC0_BASE, 2);
	while(!ROM_ADCIntStatus(ADC0_BASE, 2, false)) {}
	ROM_ADCSequenceDataGet(ADC0_BASE, 2, ui32ADC02Value);
	t2 = (ui32ADC02Value[0] + ui32ADC02Value[1] + ui32ADC02Value[2] + ui32ADC02Value[3] + 2)/4;
	//! If front sensor records a reading, calculate speed and print to UART
	if (chalu == 1) {
		if (t2 < 50) {
			front = -5;
		} else {
			if (front < 0) front++;
			else if (front == 0) {
				front = 1;
				curtime = 90000/curtime;
				UARTIntPut(curtime);
				UARTStrPut("\r\n");
				curtime = 0;
				chalu = 0;
			}
		}
	}
}

/**
 * Function Name : main()
 * Logic : Configure the timer, ADC pins and UART
 */
int main(void)
{
	chalu = 0;
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	//! Configure Timer
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	uint32_t ui32Period = (SysCtlClockGet() / 10) / 10;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -	1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE,TIMER_A);

	//! Configure ADC pins
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	GPIOPinTypeADC(GPIO_PORTB_BASE, GPIO_PIN_5);
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
	
	//! Configure ADC Sequencers
	ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ROM_ADCSequenceConfigure(ADC0_BASE, 2, ADC_TRIGGER_PROCESSOR, 0);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH11);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH11);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH11);
	ROM_ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH11|ADC_CTL_IE|ADC_CTL_END);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 0, ADC_CTL_CH7);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 1, ADC_CTL_CH7);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 2, 2, ADC_CTL_CH7);
	ROM_ADCSequenceStepConfigure(ADC0_BASE,2,3,ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
	ROM_ADCSequenceEnable(ADC0_BASE, 1);
	ROM_ADCSequenceEnable(ADC0_BASE, 2);
	ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	
	//! Configure UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	back = -5; front = -5;
	while(1) {}
}
