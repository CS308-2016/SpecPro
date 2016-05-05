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
volatile uint32_t ui32TempValueF;
uint8_t mode = 0;

/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: none

* Description: Interrupt Handler. It is called when timer interrupts occur by ISR automatically. Send Current Temperature whenever interrupt occurs

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
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'a');
		UARTCharPut(UART0_BASE, 't');
		UARTCharPut(UART0_BASE, 'u');
		UARTCharPut(UART0_BASE, 'r');
		UARTCharPut(UART0_BASE, 'e');
		UARTCharPut(UART0_BASE, ' ');
		UARTCharPut(UART0_BASE, first);
		UARTCharPut(UART0_BASE, second);
		UARTCharPut(UART0_BASE, ' ');
		char d = 176;
		UARTCharPut(UART0_BASE, d);
		UARTCharPut(UART0_BASE, 'C');
		UARTCharPut(UART0_BASE, '\r');
		UARTCharPut(UART0_BASE, '\n');
	}
}


int main(void)
{

	uint32_t ui32ADC0Value[4];
	uint32_t ui32Period;
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	//Configure ADC to read temperature values
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	//ESpecify the sampler and configure its various stages
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);
	//Enable peripheral of UART
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	///Configure pins for UART
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	//Set period of timer interrupts
	ui32Period = (SysCtlClockGet() / 1) / 2;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);

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
		//Update the value of temperature based on readings of ADC
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
	}
}
