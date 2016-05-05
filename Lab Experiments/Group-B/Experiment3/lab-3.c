#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#define PWM_FREQUENCY 55

enum State{Idle, Press, Release};
enum State swState[3] = {Idle,Idle,Idle};

volatile uint8_t ui8AdjustR;
volatile uint8_t ui8AdjustG;
volatile uint8_t ui8AdjustB;

int delay = 100000;

int sw1Count = 0;
int sw1Time = 0;
int mode = 0;

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

int main(void)
{
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	volatile int t = 0;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
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
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT|PWM_OUT_6_BIT|PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	// Auto Mode
	while(1)
	{
		if (t < 120) {
			ui8AdjustR = 120 - t;
			ui8AdjustG = t;
			ui8AdjustB = 0;
		} else if (t < 240) {
			ui8AdjustR = 0;
			ui8AdjustG = 240 - t;
			ui8AdjustB = t - 120;
		} else {
			ui8AdjustR = t - 240;
			ui8AdjustG = 0;
			ui8AdjustB = 360 - t;
		}
		// values between 10 and 160
		ui8AdjustR = 10 + (150*ui8AdjustR)/120;
		ui8AdjustG = 10 + (150*ui8AdjustG)/120;
		ui8AdjustB = 10 + (150*ui8AdjustB)/120;

		int f1 = detectKeyPress(GPIO_PIN_4,1);
		int f2 = detectKeyPress(GPIO_PIN_0,2);

		if (swState[1] != Idle && swState[2] != Idle) {
			while (swState[1] != Idle || swState[2] != Idle) {
				detectKeyPress(GPIO_PIN_4,1);
				detectKeyPress(GPIO_PIN_0,2);
			}
			break;
		} else if (f1) {
			delay = delay - 10000;
			if (delay < 10000) delay = 10000;
		} else if (f2) {
			delay = delay + 10000;
			if (delay > 200000) delay = 200000;
		}

		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);

		t++;
		if (t == 360) t = 0;

		SysCtlDelay(delay);
	}

	mode = 0;
	// Manual Mode
	while (1) {
		// Switch Mode
		while (1) {
			detectKeyPress(GPIO_PIN_0,2);
			if (swState[2] != Idle) {
				sw1Count = sw1Count + detectKeyPress(GPIO_PIN_4,1);
				if (swState[1] == Release) {
					sw1Time++;
				} else sw1Time = 0;
				if (sw1Time >= 10) {
					while (swState[1] != Idle || swState[2] != Idle) {
						detectKeyPress(GPIO_PIN_4,1);
						detectKeyPress(GPIO_PIN_0,2);
					}
					mode = 3;
					break;
				}
			} else if (swState[2] == Idle && sw1Count == 1) {
				mode = 1;
				break;
			} else if (swState[2] == Idle && sw1Count == 2) {
				mode = 2;
				break;
			}
			SysCtlDelay(SysCtlClockGet() / 50);
		}

		// Mode 1/2/3
		while (1) {
			int f1 = detectKeyPress(GPIO_PIN_4,1);
			int f2 = detectKeyPress(GPIO_PIN_0,2);
			if (swState[1] != Idle && swState[2] != Idle) {
				while (swState[1] != Idle || swState[2] != Idle) {
					detectKeyPress(GPIO_PIN_4,1);
					detectKeyPress(GPIO_PIN_0,2);
				}
				break;
			} else if (f1) {
				if (mode == 1) {
					if (ui8AdjustR < 25) ui8AdjustR = 10;
					else ui8AdjustR = ui8AdjustR - 15;
				} else if (mode == 2) {
					if (ui8AdjustB < 25) ui8AdjustB = 10;
					else ui8AdjustB = ui8AdjustB - 15;
				} else {
					if (ui8AdjustG < 25) ui8AdjustG = 10;
					else ui8AdjustG = ui8AdjustG - 15;
				}
			} else if (f2) {
				if (mode == 1) {
					ui8AdjustR = ui8AdjustR + 15;
					if (ui8AdjustR > 160) ui8AdjustR = 160;
				} else if (mode == 2) {
					ui8AdjustB = ui8AdjustB + 15;
					if (ui8AdjustB > 160) ui8AdjustB = 160;
				} else {
					ui8AdjustG = ui8AdjustG + 15;
					if (ui8AdjustG > 160) ui8AdjustG = 160;
				}
			}

			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);

			SysCtlDelay(SysCtlClockGet() / 50);
		}
		mode = 0;
		sw1Count = 0;
		sw1Time = 0;
	}
}
