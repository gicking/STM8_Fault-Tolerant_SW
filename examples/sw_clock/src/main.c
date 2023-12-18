/**********************
  
  Simple SW clock & scheduler (for convenience)

  Functionality:
    - initialization:
      - configure LED pin to output
      - initialize SW clock
    - main loop
      - periodically:
        - blink LED
        - generate long pulse on test pin 1
        - generate short pulse on test pin 2

  Functionality:
    - set HSI prescaler to 16MHz (default is 2MHz) 
    - blink LED (=D13) every 500ms
    - periodically generate 1ms high pulse on testpin D5
    - periodically generate 100us high pulse on testpin D6
    
  Supported Hardware:
    - Nucleo 8S207K8
  
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"     // required here by SDCC for ISR
#include "stm8s_clk.h"
#include "stm8s_gpio.h"
#define _MAIN_            // required for global variables
  #include "sw_clock.h"
#undef _MAIN_


/*----------------------------------------------------------
    MACROS / DEFINES
----------------------------------------------------------*/

// define LED pin
#if defined(STM8S_NUCLEO_207K8)
  #define PORT_TEST       (GPIOC)         // common port for below testpins 
  #define PIN_LED         (GPIO_PIN_5)    // LED pin = board D13 = STM8 PC5
  #define PIN_TEST1       (GPIO_PIN_2)    // testpin 1 = board D5 = STM8 PC2
  #define PIN_TEST2       (GPIO_PIN_3)    // testpin 2 = board D6 = STM8 PC3
#else
  #error Board not supported
#endif

// LED blink period [ms]
#define LED_PERIOD      500

// test pulse period [ms]
#define PULSE_PERIOD      100



/////////////////
//  main routine
/////////////////
void main(void)
{
  // for SW scheduler. Stagger calls
  uint32_t  lastLED=0, lastLongPulse=PULSE_PERIOD/4, lastShortPulse=PULSE_PERIOD/2;


  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // Initialize test pins as output low
  GPIO_Init(PORT_TEST, (PIN_LED | PIN_TEST1 | PIN_TEST2), GPIO_MODE_OUT_PP_LOW_FAST);

  // start 1ms clock via TIM4
  init_SW_clock();

  // enable interrupts
  enableInterrupts();


  /////////////
  // main loop
  /////////////
  while (1)
  {
    // check if 1ms has passed (reduce CPU load for multiple schedulers)
    if (flagMilli() == TRUE)
    {
      // clear 1ms flag
      clearFlagMilli();

      // store current ms only once for consistency
      uint32_t currTime = millis();


      //////////
      // task for LED blink
      //////////
      if (currTime - lastLED >= LED_PERIOD)
      {
        lastLED = currTime;
        GPIO_WriteReverse(PORT_TEST, PIN_LED);
      } // task LED


      //////////
      // task for long pulse (blocking delay [ms])
      //////////
      if (currTime - lastLongPulse > PULSE_PERIOD)
      {
        lastLongPulse = currTime;
        GPIO_WriteHigh(PORT_TEST, PIN_TEST1);
        delay(10);
        GPIO_WriteLow(PORT_TEST, PIN_TEST1);
      } // task long pulse


      //////////
      // task for short pulse (blocking delay [us])
      //////////
      if (currTime - lastShortPulse > PULSE_PERIOD)
      {
        lastShortPulse = currTime;
        GPIO_WriteHigh(PORT_TEST, PIN_TEST2);
        delayMicroseconds(100);
        GPIO_WriteLow(PORT_TEST, PIN_TEST2);
      } // task short pulse
   
    } // 1ms has passed

  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
