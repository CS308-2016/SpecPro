#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/debug.h"
#include"driverlib/sysctl.h"
#include"driverlib/adc.h"
#include"driverlib/uart.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"


volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t SetTemp;
volatile uint32_t ui32TempValueF;

uint8_t mode = 0;

/*

* Function Name: UARTIntHandler()

* Input: none

* Output: none

* Description: Interrupt Handler. Is called automatically when TIVA board receives characters from UART. Based on input change mode and ask to set temperature.

* Example Call: UARTIntHandler();

*/

void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ui32Status);
	char p;
	uint8_t set = 0;
	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		p = UARTCharGetNonBlocking(UART0_BASE);
		UARTCharPutNonBlocking(UART0_BASE, p);
		set = set*10 + (p -'0');
		if(p=='\n') break;
		if(mode == 1) SysCtlDelay(3000000);
		if(p=='S' && mode==0)
		{
			UARTCharPut(UART0_BASE, '\r');
			UARTCharPut(UART0_BASE, '\n');
			mode = 1;
			UARTCharPut(UART0_BASE, 'E');
			UARTCharPut(UART0_BASE, 'n');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'r');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'h');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, 'T');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'm');
			UARTCharPut(UART0_BASE, 'p');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, 'r');
			UARTCharPut(UART0_BASE, 'a');
			UARTCharPut(UART0_BASE, 't');
			UARTCharPut(UART0_BASE, 'u');
			UARTCharPut(UART0_BASE, 'r');
			UARTCharPut(UART0_BASE, 'e');
			UARTCharPut(UART0_BASE, ' ');
			UARTCharPut(UART0_BASE, ':');
			UARTCharPut(UART0_BASE, ' ');
			return;
		}
	}
	if(mode == 1)
	{
	SetTemp = set;
	UARTCharPut(UART0_BASE, '\r');
	UARTCharPut(UART0_BASE, '\n');
	UARTCharPut(UART0_BASE, 'S');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 'T');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'm');
	UARTCharPut(UART0_BASE, 'p');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, 'a');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'u');
	UARTCharPut(UART0_BASE, 'r');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 'u');
	UARTCharPut(UART0_BASE, 'p');
	UARTCharPut(UART0_BASE, 'd');
	UARTCharPut(UART0_BASE, 'a');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'e');
	UARTCharPut(UART0_BASE, 'd');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, 't');
	UARTCharPut(UART0_BASE, 'o');
	UARTCharPut(UART0_BASE, ' ');
	UARTCharPut(UART0_BASE, ' ');
		char first,second;
		if(SetTemp >= 10)
		{
			first = SetTemp/10 + '0';
			UARTCharPut(UART0_BASE, first);
		}
		second = SetTemp%10 + '0';
		UARTCharPut(UART0_BASE, second);
		UARTCharPut(UART0_BASE, ' ');
		char d = 176;
		UARTCharPut(UART0_BASE, d);
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, '\n');
		mode = 0;
	}
}

/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: none

* Description: Interrupt Handler. It is called when timer interrupts occur by ISR automatically. Send Current Temperature and set temperature whenever interrupt occurs

* Example Call: Timer0IntHandler();

*/

void Timer0IntHandler(void)
{
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	if(mode == 0)
	{
		char first = ui32TempValueC/10 + '0';
		char second = ui32TempValueC%10 + '0';
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'n');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'T');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'm');
		UARTCharPut(UART0_BASE, 'p');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, '=');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, first);
		UARTCharPut(UART0_BASE, second);
		UARTCharPut(UART0_BASE, ' ');
		char d = 176;
		UARTCharPut(UART0_BASE, d);
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, ',');
		UARTCharPut(UART0_BASE, ' ');


		UARTCharPut(UART0_BASE, 'S');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, 'T');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'm');
		UARTCharPut(UART0_BASE, 'p');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, '=');
		UARTCharPut(UART0_BASE, ' ');
		if(SetTemp >= 10)
		{
			first = SetTemp/10 + '0';
			UARTCharPut(UART0_BASE, first);
		}
		second = SetTemp%10 + '0';
		UARTCharPut(UART0_BASE, second);
		UARTCharPut(UART0_BASE, ' ');
		d = 176;
		UARTCharPut(UART0_BASE, d);
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, '\n');
	}
}


int main(void)
{
	SetTemp = 25;
	uint32_t ui32ADC0Value[4];
	uint32_t ui32Period;
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	//Enable ADC peripherals and configure ADC
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ADCHardwareOversampleConfigure(ADC0_BASE, 64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
	//Enable UART Peripherals and  configure UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	//Enable TIMER
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = (SysCtlClockGet() / 1) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();

	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		//UPdate Current Temperature values
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
		if(ui32TempValueC < SetTemp) {GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);}
		else {GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1); GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);}
	}
}
