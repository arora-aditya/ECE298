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


// #define COL1 GPIO_PORT_P1, GPIO_PIN3
// #define COL2 GPIO_PORT_P1, GPIO_PIN4
// #define COL3 GPIO_PORT_P1, GPIO_PIN5
// #define ROW1 GPIO_PORT_P5, GPIO_PIN3
// #define ROW2 GPIO_PORT_P5, GPIO_PIN2
// #define ROW3 GPIO_PORT_P5, GPIO_PIN0
// #define ROW4 GPIO_PORT_P1, GPIO_PIN6

typedef enum LED_t {GREEN, YELLOW, ORANGE, RED, NUM_LEDS} LED;

typedef enum KEYPAD_STATE_t {RANGE_1, RANGE_2, RANGE_3, RANGE_4, NUM_RANGES} KEYPAD_STATE;

void Key();
void setupKeypad();
void toggleLED(unsigned int i);
char pressedKey;
void measure_Distance();
int distance_cm;

const char distance_ranges_format_string[30] = "R1: %d R2: %d R3: %d R4: %d";

typedef struct distance_ranges {
  int r1;
  int r2;
  int r3;
  int r4;
} distance_r;

distance_r dist;

volatile unsigned int j = 0;

char output[30];
//dist.r1 = 0;
//dist.r2 = 0;
//dist.r3 = 0;
//dist.r4 = 0;
KEYPAD_STATE keypad_state = RANGE_1;
LED led_state = NUM_LEDS;

int digit_num = 0;

void get_num(){
  digit_num += 1;
  switch(keypad_state){
    case RANGE_1: dist.r1 = dist.r1*10 + atoi(&pressedKey); break;
    case RANGE_2: dist.r2 = dist.r2*10 + atoi(&pressedKey); break;
    case RANGE_3: dist.r3 = dist.r3*10 + atoi(&pressedKey); break;
    case RANGE_4: dist.r4 = dist.r4*10 + atoi(&pressedKey); break;
  }
  if(digit_num%3 == 0){
    switch(keypad_state){
      case RANGE_1: keypad_state=RANGE_2; break;
      case RANGE_2: keypad_state=RANGE_3; break;
      case RANGE_3: keypad_state=RANGE_4; break;
      case RANGE_4: keypad_state=NUM_RANGES; break;
    }
  }
}

void toggle_LED_distance(){
  if(distance_cm > dist.r4){
      led_state = GREEN;
  }else if(distance_cm > dist.r3){
      led_state = YELLOW;
  } else if(distance_cm > dist.r2){
      led_state = ORANGE;
  } else if(distance_cm > dist.r1){
      led_state = RED;
  } else{
      led_state = NUM_LEDS;
  }
  toggleLED(led_state);
}

int main(void) {
  WDT_A_hold(WDT_A_BASE); // Stop watchdog timer
  PMM_unlockLPM5();
  Init_GPIO();
  Init_Clock();
  Init_RTC();
  Init_LCD();
  LCD_E_selectDisplayMemory(LCD_E_BASE, LCD_E_DISPLAYSOURCE_MEMORY);
  distance_cm = -1;
  setupKeypad();
  _EINT(); // Start interrupt


  while(keypad_state != NUM_RANGES){
      toggleLED((j++)%4); __delay_cycles(100000);
  }
  P1IE &= 0x00;
  led_state = RED;
  sprintf(output, distance_ranges_format_string, dist.r1, dist.r2, dist.r3, dist.r4);
  displayScrollText(output);
  for(;;){
    measure_Distance();
    toggle_LED_distance();
  }


   // Need this for LED to turn on- in case of "abnormal off state"
  __bis_SR_register(GIE); // Need this for interrupts or else "abnormal termination"
  __no_operation(); //For debugger
  return 0;
}

void measure_Distance(){
    int echo_pulse_duration;      // time in us

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
}

void toggleLED(LED i){
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0);
    switch(i){
        case GREEN:
            // green
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
            break;
        case YELLOW:
            // yellow
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            break;
        case ORANGE:
            // orange
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
            break;
        case RED:
            // red
            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
            break;
        default:
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN1);
            GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
            GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN0);
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
      pressedKey = hexaKeys[0][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey = hexaKeys[0][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey = hexaKeys[0][2];
  }

  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey = hexaKeys[1][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey = hexaKeys[1][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey = hexaKeys[1][2];
  }

  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey = hexaKeys[2][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey = hexaKeys[2][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey = hexaKeys[2][2];
  }

  GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6);
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3) == GPIO_INPUT_PIN_LOW) {     // Column 1 to GND
      pressedKey = hexaKeys[3][0];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4) == GPIO_INPUT_PIN_LOW) {     // Column 2 to GND
      pressedKey = hexaKeys[3][1];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN2);
  if (GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN5) == GPIO_INPUT_PIN_LOW) {     // Column 3 to GND
      pressedKey = hexaKeys[3][2];
  }
  GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN3 | GPIO_PIN2 | GPIO_PIN0);
  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6);
}

#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  Key();
  GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN5 | GPIO_PIN4 | GPIO_PIN3);
  get_num();
  // measure_Distance();
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
