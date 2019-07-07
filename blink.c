/* DESCRIPTION
 * Sample code showing keypad's response to pushing buttons 1 and 2
 * Include pressedKey on debugger's "Expressions" to see the hexaKeys' value when you alternate between the two keys
 * Did not include button debouncer in this (releasing the button does not set pressedKey back to Value 0 '\x00')
 */

#include <msp430.h>
#include <stdio.h>
#include "driverlib\MSP430FR2xx_4xx\driverlib.h"
#include "Board.h"
#include "hal_LCD.h"


char hexaKeys[4][3] = {
  { '1','2','3' },
  { '4','5','6' },
  { '7','8','9' },
  { '*','0','#' }
};


#define COL1 GPIO_PORT_P1, GPIO_PIN3
#define COL2 GPIO_PORT_P1, GPIO_PIN4
#define COL3 GPIO_PORT_P1, GPIO_PIN5
#define ROW1 GPIO_PORT_P5, GPIO_PIN3
#define ROW2 GPIO_PORT_P5, GPIO_PIN2
#define ROW3 GPIO_PORT_P5, GPIO_PIN0
#define ROW4 GPIO_PORT_P1, GPIO_PIN6


void Key();
void setupKeypad();
void toggleLED(unsigned int i);
char pressedKey[2];

int main(void) {
  WDT_A_hold(WDT_A_BASE); // Stop watchdog timer
  Init_GPIO();
  setupKeypad();
  Init_Clock();
  Init_RTC();
  Init_LCD();
  _EINT(); // Start interrupt

  volatile unsigned int j = 0;
  for(;;){
      volatile unsigned int i;
      i = 20000;
      do i--;
      while(i != 0);
      toggleLED(j++);
  }

  PMM_unlockLPM5(); // Need this for LED to turn on- in case of "abnormal off state"
  __bis_SR_register(GIE); // Need this for interrupts or else "abnormal termination"
  __no_operation(); //For debugger
  return 0;
}

void toggleLED(unsigned int i){
    i = i%4;
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN5);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN3);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN2);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN2);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN3);
    switch(i){
        case 0:
            // green
            GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1);
            break;
        case 1:
            // yellow
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN5);
            break;
        case 2:
            // orange
            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN2);
            break;
        case 3:
            // red
            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN3);
            break;
    }
}

void setupKeypad(){
    /*
       * R1: 5.3
       * R2: 5.2
       * R3: 5.0
       * R4: 1.6
       * C1: 1.3
       * C2: 1.4
       * C3: 1.5
       */
      GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
      GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
      GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN2);
      GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
      GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
      GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
      GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
      GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);

      // COLUMNS ARE ISR TRIGGERS

      GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN3, GPIO_HIGH_TO_LOW_TRANSITION);
      GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN3);
      GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN3);

      GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);
      GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
      GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN4);

      GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);
      GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN5);
      GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN5);

      GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN3|GPIO_PIN4|GPIO_PIN5);
}

void Key() {
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey[0] = hexaKeys[0][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey[0] = hexaKeys[0][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey[0] = hexaKeys[0][2];
  }
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey[0] = hexaKeys[1][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey[0] = hexaKeys[1][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey[0] = hexaKeys[1][2];
  }
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey[0] = hexaKeys[2][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey[0] = hexaKeys[2][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey[0] = hexaKeys[2][2];
  }
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN2);
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey[0] = hexaKeys[3][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey[0] = hexaKeys[3][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey[0] = hexaKeys[3][2];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3); // Row 1- LOW
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2); // Row 2- LOW
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0); // Row 1- LOW
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6); // Row 2- LOW
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  Key();
  GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN5);
  GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN4);
  GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN3);
  LCD_E_selectDisplayMemory(LCD_E_BASE, LCD_E_DISPLAYSOURCE_MEMORY);
  clearLCD();
  displayScrollText(pressedKey);
}

#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
{
    switch(__even_in_range(RTCIV, RTCIV_RTCIF))
    {
        case RTCIV_RTCIF:
            __bic_SR_register_on_exit(LPM3_bits); // exit LPM3
            break;
    }


}
