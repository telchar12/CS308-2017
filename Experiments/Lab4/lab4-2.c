#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include "driverlib/debug.h"
#include <time.h>
#include <inc/hw_gpio.h>
#include "driverlib/ssi.h"
#include "driverlib/uart.h"


uint32_t ui32ADC0Value[4];


volatile uint32_t ui32Avg0;
volatile uint32_t temp0;


volatile uint8_t digit0;
volatile uint8_t digit1;
volatile uint8_t digit2;
volatile uint8_t digit3;

void uart_char(char data)
{
	UARTCharPut(UART0_BASE, data);
}

int main(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

	//ADCHardwareOversampleConfigure(ADC0_BASE, 64);

	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);


	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	//GPIOPinConfigure(GPIO_PA0_U0RX);
	//GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	while (1)
	{
		//if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

		ui32Avg0 = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		temp0=ui32Avg0;
		//converting to ASCII
		digit0=temp0%10+48;
		temp0=temp0/10;
		digit1=temp0%10+48;
		temp0=temp0/10;
		digit2=temp0%10+48;
		temp0=temp0/10;
		digit3=temp0%10+48;
		temp0=temp0/10;

		uart_char(digit0);
		uart_char(digit1);
		uart_char(digit2);
		uart_char(digit3);
		uart_char('\n');

	}
}
