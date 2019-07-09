/* DESCRIPTION
 * Sample code showing keypad's response to pushing buttons 1 and 2
 * Include pressedKey on debugger's "Expressions" to see the hexaKeys' value when you alternate between the two keys
 * Did not include button debouncer in this (releasing the button does not set pressedKey back to Value 0 '\x00')
 */

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

// enum LED {GREEN, YELLOW, ORANGE, RED}

void Key();
void setupKeypad();
void toggleLED(unsigned int i);
char pressedKey[2];
void measure_Distance();
int distance_cm;

int main(void) {
  WDT_A_hold(WDT_A_BASE); // Stop watchdog timer
  PMM_unlockLPM5();
  Init_GPIO();
 setupKeypad();
  Init_Clock();
  Init_RTC();
  Init_LCD();
  _EINT(); // Start interrupt
  distance_cm = -1;
  volatile unsigned int j = 0;
  for(;;){
      volatile unsigned int i;
      i = 20000;
      do i--;
      while(i != 0);
      toggleLED(j++);
  }

   // Need this for LED to turn on- in case of "abnormal off state"
  __bis_SR_register(GIE); // Need this for interrupts or else "abnormal termination"
  __no_operation(); //For debugger
  return 0;
}

void measure_Distance(){
    int echo_pulse_duration;      // time in us

    P1DIR = BIT0+BIT7;
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);
    GPIO_setAsInputPin(GPIO_PORT_P8, GPIO_PIN1);

    // Set up Timer_A1: SMCLK clock, input divider=1,
    // "continuous" mode. It counts from 0 to 65535,
    // incrementing once per clock cycle (i.e. every 1us).
    TA1CTL = TASSEL_2 + ID_0 + MC_2;
    // Delay by half a second in order to get better view of the pulses on the oscilloscope
    __delay_cycles(50000);
    // Send a 10us trigger pulse
    P1OUT |= BIT7;                // trigger high
    __delay_cycles(10);           // 10us delay
    P1OUT &= ~BIT7;               // trigger low

    // Measure duration of echo pulse
    while ((P8IN & BIT1) == 0);   // Wait for start of echo pulse
    TA1R = 0;                     // Reset timer at start of pulse
    while ((P8IN & BIT1) > 0);    // Wait for end of echo pulse
    echo_pulse_duration = TA1R;   // Current timer value is pulse length
    if(echo_pulse_duration < 0){
        echo_pulse_duration = 5800;
    }
    distance_cm =  echo_pulse_duration/58; // Convert from us to cm

    if (distance_cm < 50) P1OUT |= BIT0; // LED on
    else P1OUT &= ~BIT0;                 // LED off
//        displayScrollText(itoa(distance_cm));
}

void toggleLED(unsigned int i){
    i = i%4;
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0);
    switch(i){
        case 0:
            // green
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
            break;
        case 1:
            // yellow
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            break;
        case 2:
            // orange
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
            break;
        case 3:
            // red
            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
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
      GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN0, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
      GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
      GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
      GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);

      // COLUMNS ARE ISR TRIGGERS

      GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN3, GPIO_HIGH_TO_LOW_TRANSITION);
      GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN3);
      GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN4, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);
      GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN4);
      GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
      GPIO_selectInterruptEdge(GPIO_PORT_P1, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);
      GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN5);

      GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5);
      GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5);
}

void Key() {
  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
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

  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey[0] = hexaKeys[1][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey[0] = hexaKeys[1][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey[0] = hexaKeys[1][2];
  }

  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
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

  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey[0] = hexaKeys[3][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey[0] = hexaKeys[3][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey[0] = hexaKeys[3][2];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  Key();
  GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN5 | GPIO_PIN4 | GPIO_PIN3);
  LCD_E_selectDisplayMemory(LCD_E_BASE, LCD_E_DISPLAYSOURCE_MEMORY);
  clearLCD();
  displayScrollText(pressedKey);
  measure_Distance();
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
