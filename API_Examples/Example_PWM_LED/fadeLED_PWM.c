/* DESCRIPTION
 * Sample code structure for PWM Control of an LED
*/


#include "driverlib.h"
#include "stdbool.h"

#define completePeriod 511
bool increasing;
int highPeriod;



void main (void)
{
    increasing = true;          // INCREASE
    highPeriod = 0; // NO LIGHT IN BEGINNING

    //Stop WDT
    WDT_A_hold(WDT_A_BASE);

    //P8.3 as output with module function
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P8, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    PMM_unlockLPM5();

    //Start timer
    Timer_A_initUpModeParam param = {0};
    param.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    param.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    param.timerPeriod = completePeriod;
    param.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_DISABLE;
    param.captureCompareInterruptEnable_CCR0_CCIE =
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE;
    param.timerClear = TIMER_A_DO_CLEAR;
    param.startTimer = true;
    Timer_A_initUpMode(TIMER_A1_BASE, &param);


    //Initialize compare mode to generate PWM
    Timer_A_initCompareModeParam initComp2Param = {0};
    initComp2Param.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    initComp2Param.compareInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE;
    initComp2Param.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;

    for (;;) {
        while (increasing == true) {
            if (highPeriod == 511){
                increasing = false;
            }
            highPeriod++;
            initComp2Param.compareValue = highPeriod;
            Timer_A_initCompareMode(TIMER_A1_BASE, &initComp2Param);
            _delay_cycles(2000);
        }
        while (increasing == false) {
            if (highPeriod == 0){
                increasing = true;
            }
            highPeriod--;
            initComp2Param.compareValue = highPeriod;
            Timer_A_initCompareMode(TIMER_A1_BASE, &initComp2Param);
            _delay_cycles(2000);
        }
    }
}
