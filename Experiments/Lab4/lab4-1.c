#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/timer.h"
#include <time.h>
#include <inc/hw_gpio.h>
#include "driverlib/ssi.h"
#include "driverlib/uart.h"

uint32_t ui32ADC0Value[4];

volatile uint32_t ui32Avg0;

void uart_init()
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

	GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	GPIOPinConfigure(GPIO_PORTA_BASE|GPIO_PCTL_PA0_U0RX);
	//GPIOPinConfigure(GPIO_PORTD_BASE|GPIO_PCTL_PD0_U0RX);

	GPIOPinConfigure(GPIO_PORTA_BASE|GPIO_PCTL_PA1_U0TX);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    SysCtlDelay(3);

    // For making PE2 as ADC input pin
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_1 | GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTD_BASE,GPIO_PIN_1,GPIO_FALLING_EDGE);
        // Make PF4 a trigger for ADC
        GPIOADCTriggerEnable(GPIO_PORTF_BASE, GPIO_PIN_4);


	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	IntMasterEnable();
	IntEnable(INT_UART0);
	UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void UARTIntHandler(void)
{
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, ui32Status);
	while(UARTCharsAvail(UART0_BASE))//loop while char
	{
		UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE)+1);
		//echo char
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //blink LED

		SysCtlDelay(SysCtlClockGet() / (1000 * 3));
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn off LED
	}
}

int main(void)
{
	GPIOADCTriggerEnable(GPIO_PORTD_BASE, GPIO_PIN_0);
	GPIOADCTriggerEnable(GPIO_PORTD_BASE, GPIO_PIN_1);

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

	//ADCHardwareOversampleConfigure(ADC0_BASE, 64);

	//For ADC0 Base
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH6 | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);

	while(1){
		SysCtlDelay(5000000);
		ADCIntClear(ADC0_BASE, 3);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32Avg0 = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;



	}

}
