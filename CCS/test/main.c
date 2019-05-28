/* DESCRIPTION
 * Sample code showing keypad's response to pushing buttons 1 and 2
 * Include pressedKey on debugger's "Expressions" to see the hexaKeys' value when you alternate between the two keys
 * Did not include button debouncer in this (releasing the button does not set pressedKey back to Value 0 '\x00')
 */


#include <msp430.h>
#include <stdio.h>
#include "driverlib\MSP430FR2xx_4xx\driverlib.h"
#include "Board.h"


char hexaKeys[4][3] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

void Key();
char pressedKey;

int main (void)
{
    WDT_A_hold(WDT_A_BASE);     // Stop watchdog timer

    // ROWS ARE OUTPUTS
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN3);                  // Row 3: Output direction pin 1.3
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN4);                  // Row 2: Output direction pin 1.4
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);                  // Row 1: Output direction pin 1.6
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2);                  // Row 4: Output direction pin 5.2
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);

    // COLUMNS ARE ISR TRIGGERS

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);     // Column 2: Input 1.7
    GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN7, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN7);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN7);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN7);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);     // Column 1: Input 5.0
    GPIO_selectInterruptEdge(GPIO_PORT_P5, GPIO_PIN0, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN0);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);     // Column 3: Input 5.3
    GPIO_selectInterruptEdge(GPIO_PORT_P5, GPIO_PIN3, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN3);

    _EINT();        // Start interrupt

    PMM_unlockLPM5();           // Need this for LED to turn on- in case of "abnormal off state"
    __bis_SR_register(LPM4_bits + GIE);     // Need this for interrupts or else "abnormal termination"
    __no_operation();           //For debugger
    return 0;
}

void Key()
{
/*
 * R1: 1.6
 * R2: 1.4
 * R3: 1.3
 * R4: 5.2
 * C1: 5.0
 * C2: 1.7
 * C3: 5.3
 */

        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6); // Row 1- LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0) == GPIO_INPUT_PIN_LOW)     // Column 1 to GND
            pressedKey = hexaKeys[0][0];        // Shows "1"
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6); // Row 1- HIGH

        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6); // Row 1- LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7) == GPIO_INPUT_PIN_LOW)     // Column 2
            pressedKey = hexaKeys[0][1];       // Shows "2"
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6); // Row 1- HIGH


        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6); // Row 1- LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN3) == GPIO_INPUT_PIN_LOW)     // Column 3
            pressedKey = hexaKeys[0][2];       // Shows "3"
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6); // Row 1- HIGH

        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6); // Row 1- LOW

        /*
         * R1: 1.6
         * R2: 1.4
         * R3: 1.3
         * R4: 5.2
         * C1: 5.0
         * C2: 1.7
         * C3: 5.3
         */
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4); // Row 2 - LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0) == GPIO_INPUT_PIN_LOW)     // Column 1 to GND
            pressedKey = hexaKeys[1][0];
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4); // Row 2 - HIGH

        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4); // Row 2 - LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P1,     GPIO_PIN7) == GPIO_INPUT_PIN_LOW)     // Column 2
            pressedKey = hexaKeys[1][1];
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4); // Row 2- HIGH


        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4); // Row 1- LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN3) == GPIO_INPUT_PIN_LOW)     // Column 3
            pressedKey = hexaKeys[1][2];
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN4); // Row 1- HIGH

        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN4); // Row 1- LOW

        /*
         * R1: 1.6
         * R2: 1.4
         * R3: 1.3
         * R4: 5.2
         * C1: 5.0
         * C2: 1.7
         * C3: 5.3
         */
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3); // Row 2 - LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0) == GPIO_INPUT_PIN_LOW)     // Column 1 to GND
            pressedKey = hexaKeys[2][0];
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3); // Row 2 - HIGH

        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3); // Row 2 - LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7) == GPIO_INPUT_PIN_LOW)     // Column 2
            pressedKey = hexaKeys[2][1];
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3); // Row 2- HIGH


        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3); // Row 1- LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN3) == GPIO_INPUT_PIN_LOW)     // Column 3
            pressedKey = hexaKeys[2][2];
        GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN3); // Row 1- HIGH

        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN3); // Row 1- LOW

        /*
         * R1: 1.6
         * R2: 1.4
         * R3: 1.3
         * R4: 5.2
         * C1: 5.0
         * C2: 1.7
         * C3: 5.3
         */
        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2); // Row 2 - LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN0) == GPIO_INPUT_PIN_LOW)     // Column 1 to GND
            pressedKey = hexaKeys[3][0];
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2); // Row 2 - HIGH

        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2); // Row 2 - LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN7) == GPIO_INPUT_PIN_LOW)     // Column 2
            pressedKey = hexaKeys[3][1];
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2); // Row 2- HIGH


        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2); // Row 1- LOW
        if (GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN3) == GPIO_INPUT_PIN_LOW)     // Column 3
            pressedKey = hexaKeys[3][2];
        GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2); // Row 1- HIGH

        GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2); // Row 1- LOW

}

#pragma vector = PORT1_VECTOR       // Using PORT1_VECTOR interrupt because P1.4 and P1.5 are in port 1
__interrupt void PORT1_ISR(void)
{
    Key();
    /*
     * R1: 1.6
     * R2: 1.4
     * R3: 1.3
     * R4: 5.2
     * C1: 5.0
     * C2: 1.7
     * C3: 5.3
     */
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN6);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN3);
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN7);
}

#pragma vector = PORT5_VECTOR       // Using PORT1_VECTOR interrupt because P1.4 and P1.5 are in port 1
__interrupt void PORT5_ISR(void)
{
    Key();
    /*
     * R1: 1.6
     * R2: 1.4
     * R3: 1.3
     * R4: 5.2
     * C1: 5.0
     * C2: 1.7
     * C3: 5.3
     */
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN2);
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN3);
}
//#include <stdio.h>
//#include <msp430.h>
//
//
///**
// * hello.c
// */
//int main(void)
//{
//    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
//
//    printf("Hello World!\n");
//
//    return 0;
//}
