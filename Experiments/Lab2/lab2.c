/*

 * Author: Priyank Gupta (09005034)

 * Filename: lab2.c

 * Functions: setup(), ledPinConfig(), switchPinConfig(), main()

 * Global Variables: sw2count

 */

//Had to replace Default Handler by MyTimerIntHandler in tm4c123gh6pm_startup_ccs.c

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include <time.h>
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"


// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
 */
int sw1Status = 0;
int sw2Status = 0;
uint8_t ui8LED = 2;
int sw2count = 0;

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
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

/*

 * Function Name: switchPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 0 and Pin 4 as input.

 * Example Call: switchPinConfig();

 */
void switchPinConfig(void)
{
	// Unlock switches
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_DIR_MODE_IN);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4 |GPIO_PIN_0 ,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

/*

 * Function Name: clockConfig()

 * Input: none

 * Output: none

 * Description: Configure Timer 0A with??

 * Example Call: clockConfig();

 */
void clockConfig(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
	uint32_t ui32Period = (SysCtlClockGet() * 0.05);
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -	1);
}

/*

 * Function Name: interruptConfig()

 * Input: none

 * Output: none

 * Description: Configure interrupt for Timer 0A timeout.

 * Example Call: interruptConfig();

 */
void interruptConfig(void)
{
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
}


/*

 * Function Name: detectSW1Press()

 * Input: none

 * Output: 1 if switch press detected, 0 if not

 * Description: State machine for switch 1 press.

 * Example Call: detectSW1Press();

 */
unsigned char detectSW1Press(void) {
	switch (sw1Status) {
	case 0:
		sw1Status = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4) ? 0 : 1;
		break;
	case 1:
		sw1Status = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4) ? 0 : 2;
		if(sw1Status == 2) return 1;
		break;
	case 2:
		sw1Status = (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)) ? 0 : 2;
		break;
	default:
		break;
	}

	return 0;
}

/*

 * Function Name: detectSW2Press()

 * Input: none

 * Output: 1 if switch press detected, 0 if not

 * Description: State machine for switch 2 press.

 * Example Call: detectSW2Press();

 */
unsigned char detectSW2Press(void) {
	switch (sw2Status) {
	case 0:
		sw2Status = (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)) ? 0 : 1;
		break;
	case 1:
		sw2Status = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0) ? 0 : 2;
		if(sw2Status == 2) return 1;
		break;
	case 2:
		sw2Status = (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)) ? 0 : 2;
		break;
	default:
		break;
	}

	return 0;
}

/*

 * Function Name: MyTimerIntHandler()

 * Input: none

 * Output: none

 * Description: Interrupt handler which executes every timer interrupt. Actions are taken depending on states of switches.
 * Example Call: MyTimerIntHandler();

 */
void MyTimerIntHandler(void)
{
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);

	// Switch 1 - change LEDs
	if(detectSW1Press() == 1) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
        if (ui8LED == 8){
                ui8LED = 2;
            }
            else{
                ui8LED = ui8LED*2;
            }
	}
/*if (ui8LED == 14){
                ui8LED = 2;
            }
            else{
                ui8LED = ui8LED+2;
            }
*/


	// Switch 2 - increment counter
	if(detectSW2Press() == 1) {
		sw2count++;
	}
}

int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	clockConfig();
	interruptConfig();
	TimerEnable(TIMER0_BASE,TIMER_A);
	while(1)
	{
	}
}
