#include <stdbool.h>
#include <stdint.h>
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/uart.h>
#include <inc/hw_memmap.h>
#include"inc/hw_ints.h"
#include"inc/hw_types.h"
#include"driverlib/interrupt.h"

#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

void UARTStrPut(char a[])
{
	int i;
	for (i = 0; a[i] != '\0'; i++)
		UARTCharPut(UART0_BASE, a[i]);
}

void UARTIntPut(int i){
	char temp[20], a[20];
	int l = 0, m = 0, j;
	if (i < 0) {
		i = -i;
		a[0] = '-';
		m = 1;
	} else if (i == 0) {
		UARTStrPut("0");
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

int setTemp = 25, newSet = 0, mode = 0;
void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
	while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
	{
		char x = UARTCharGetNonBlocking(UART0_BASE);
		if(mode == 0 && x == 's') {
			mode = 1;
			UARTStrPut("Enter the temperature : ");
			newSet = 0;
		} else if (mode == 1) {
			if (x == 127 && newSet > 0) { // backspace
				UARTCharPut(UART0_BASE, 127);
				newSet /= 10;
			} else if (x >= 48 && x <= 57) { // digit
				UARTCharPut(UART0_BASE, x);
				newSet = newSet*10 + (x - 48);
			} else if (x == 13) { // new line (enter)
				setTemp = newSet;
				UARTStrPut("\r\nSet Temperature updated to ");
				UARTIntPut(setTemp);
				UARTCharPut(UART0_BASE, 176);
				UARTStrPut("C\r\n");
				mode = 0;
			}
		}
	}
}

int main(void)
{
	uint32_t ui32ADC0Value[4];
	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueF;


	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
	ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ROM_ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ROM_ADCSequenceEnable(ADC0_BASE, 1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));


	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

	while(1)
	{
		if(mode == 0)
		{
			ROM_ADCIntClear(ADC0_BASE, 1);
			ROM_ADCProcessorTrigger(ADC0_BASE, 1);
			while(!ROM_ADCIntStatus(ADC0_BASE, 1, false))
			{
			}
			ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
			ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
			ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
			ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

			UARTStrPut("Current Temperature ");
			UARTIntPut(ui32TempValueC);
			UARTCharPut(UART0_BASE, 176);

			UARTStrPut("C, Set Temperature ");
			UARTIntPut(setTemp);
			UARTCharPut(UART0_BASE, 176);
			UARTStrPut("C\r\n");

			int temp;
			if (setTemp < ui32TempValueC) temp = 2; // red
			else if (setTemp > ui32TempValueC) temp = 8; // green
			else temp = 4; // blue if equal
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, temp);
			SysCtlDelay(SysCtlClockGet()/3);
		}
	}
}
