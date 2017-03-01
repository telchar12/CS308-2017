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
uint32_t ui32ADC1Value[4];
volatile uint32_t ui32Avg0;
volatile uint32_t ui32Avg1;

void setup(void)       // set crystal freq and enable GPIO pins
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;

    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 );
    //GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3);
    //GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 |GPIO_PIN_1|GPIO_PIN_2 |GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 );
    //GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);*/

}

void adc_init(void){
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //  enable the ADC0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //configure ADC sequencer - use ADC 0, sample sequencer 1, want the processor to trigger sequence, use highest priority
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    //Configure ADC Sequencer Steps 0 - 2 on sequencer 1 to sample the temperature sensor
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6 );
    //final sequencer step: need to sample temp sensor(ADC_CTL_TS), configure interrupt flag(ADC_CTL_IE) to be set when sample is done
    //tell ADC logic that this is the last conversion on sequencer (ADC_CTL_END)
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH6 |ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);
    //ADC 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    //configure ADC sequencer - use ADC 0, sample sequencer 1, want the processor to trigger sequence, use highest priority
    ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    //Configure ADC Sequencer Steps 0 - 2 on sequencer 1 to sample the temperature sensor
    ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH6 );
    //final sequencer step: need to sample temp sensor(ADC_CTL_TS), configure interrupt flag(ADC_CTL_IE) to be set when sample is done
    //tell ADC logic that this is the last conversion on sequencer (ADC_CTL_END)
    ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH6 |ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC1_BASE, 1);
}


int main(void)
{
    setup();
    adc_init();
    GPIOADCTriggerEnable(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOADCTriggerEnable(GPIO_PORTD_BASE, GPIO_PIN_1);


    //For ADC1 Base
    while(1)
    {
        //clear interrupt flag
        SysCtlDelay(7000000);
        ADCIntClear(ADC0_BASE, 3);
        ADCProcessorTrigger(ADC0_BASE, 1);

        ADCIntClear(ADC1_BASE, 1);
        ADCProcessorTrigger(ADC1_BASE, 1);
        while(!ADCIntStatus(ADC0_BASE|ADC1_BASE, 1, false))
        {
    }
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
        ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);
        ui32Avg0 = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3])/4;
        ui32Avg1 = (ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3])/4;



    }

}

