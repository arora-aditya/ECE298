/* DESCRIPTION
 * Sample code structure when with ADC analog input and driving an LED with PWM
 * This example: Turns a LED on at two different brightness level depending on ADC value
*/


#include <msp430.h>
#include "driverlib.h"
#include "Board.h"

#define TIMER_PERIOD 511
int DUTY_CYCLE = 0;

void main (void)
{

     WDT_A_hold(WDT_A_BASE);



     // Start timer

     Timer_A_initUpModeParam param = {0};
     param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
     param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
     param.timerPeriod = TIMER_PERIOD;
     param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
     param.captureCompareInterruptEnable_CCR0_CCIE = TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
     param.timerClear = TIMER_A_DO_CLEAR;
     param.startTimer = true;
     Timer_A_initUpMode(TIMER_A0_BASE, &param);



     // PWM set-up

     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);     // Connect P1.6 to LED

     Timer_A_initCompareModeParam initComp2Param = {0};
     initComp2Param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
     initComp2Param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
     initComp2Param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;



     // ADC set-up

     GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_ADC8, GPIO_PIN_ADC8, GPIO_FUNCTION_ADC8); // ADC Anolog Input

     PMM_unlockLPM5();

     ADC_init(0x0700, ADC_SAMPLEHOLDSOURCE_SC, ADC_CLOCKSOURCE_ADCOSC, ADC_CLOCKDIVIDER_1);
     ADC_enable(0x0700);
     ADC_setupSamplingTimer(0x0700, ADC_CYCLEHOLD_16_CYCLES, ADC_MULTIPLESAMPLESDISABLE);        // timer trigger needed to start every ADC conversion
     ADC_configureMemory(0x0700, ADC_INPUT_A8, ADC_VREFPOS_INT, ADC_VREFNEG_AVSS);
     ADC_clearInterrupt(0x0700, ADC_COMPLETED_INTERRUPT);       //  bit mask of the interrupt flags to be cleared- for new conversion data in the memory buffer
     ADC_enableInterrupt(0x0700, ADC_COMPLETED_INTERRUPT);       //  enable source to reflected to the processor interrupt

     while (PMM_REFGEN_NOTREADY == PMM_getVariableReferenceVoltageStatus()) ;

     PMM_enableInternalReference();      // disabled by default

     __bis_SR_register(GIE);




     for (;;)
     {
         __delay_cycles(5000);
         ADC_startConversion(0x0700, ADC_SINGLECHANNEL);

         initComp2Param.compareValue = DUTY_CYCLE;
         Timer_A_initCompareMode(TIMER_A0_BASE, &initComp2Param);
     }
}



//ADC ISR
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(ADC_VECTOR)))
#endif
void ADC_ISR (void)
{
    switch (__even_in_range(ADCIV,12)){     // interrupt vector register never has a value that is odd or larger than 12 (stated)
        case  0: break; //No interrupt
        case  2: break; //conversion result overflow
        case  4: break; //conversion time overflow
        case  6: break; //ADCHI
        case  8: break; //ADCLO
        case 10: break; //ADCIN
        case 12:        //ADCIFG0 is ADC interrupt flag

            if ((ADC_getResults(0x0700) <= 0x155))      // 0x155 = 0.5V
                DUTY_CYCLE = 511;       // LED max brightness

            else
                DUTY_CYCLE = 100;       // LED dim

            break;
        default: break;
    }
}
