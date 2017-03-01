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
uint32_t ui32ADC1Value[4];

volatile uint32_t ui32Avg0;

volatile uint32_t ui32Avg1;

volatile uint8_t digit0;
volatile uint8_t digit1;
volatile uint8_t digit2;
volatile uint8_t digit3;

void uart_char(char data)
{
    UARTCharPut(UART0_BASE, data);
}

void setup(void)       // set crystal freq and enable GPIO pins
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

}

void adc_init(void){
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //  enable the ADC0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH6 |ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);
    //ADC 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH6 );
    ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH6 |ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC1_BASE, 1);
}


int main(void) {
    setup();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    adc_init();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ////GPIOPinConfigure(GPIO_PA0_U0RX);
    //GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    while (1)
    {
        //if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
        SysCtlDelay(7000000);
        ADCIntClear(ADC0_BASE, 3);
        ADCProcessorTrigger(ADC0_BASE, 1);
        ADCIntClear(ADC1_BASE, 1);
        ADCProcessorTrigger(ADC1_BASE, 1);
        while(!ADCIntStatus(ADC0_BASE, 1, false))
        {
        }
        while(!ADCIntStatus(ADC1_BASE, 1, false))
        {
        }
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
        ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);

        ui32Avg0 = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3])/4;
        ui32Avg1 = (ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3])/4;

        digit0=(ui32ADC0Value[0]+ui32ADC1Value[0])/10;
        digit1=(ui32ADC0Value[1]+ui32ADC1Value[1])/10;
        digit2=(ui32ADC0Value[2]+ui32ADC1Value[2])/10;
        digit3=(ui32ADC0Value[3]+ui32ADC1Value[3])/10;


        uart_char(digit0);
        uart_char(digit1);
        uart_char(digit2);
        uart_char(digit3);
        uart_char('\n');

    }
}

