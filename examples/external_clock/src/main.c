/**********************
  
  Check external HSE clock operation

  Functionality:
    - initialization:
      - configure LED pin to output
      - initialize SW clock
      - switch to HSE, activate CSS w/ interrupt
    - main loop
      - blink LED periodically
      - blink LED periodically, based on HSE state (1s: ok; 0.5s: startup failed; 0.1s: CSS fail)

  Supported Hardware:
    - STM8S Discovery
    - Sduino Uno (https://github.com/roybaer/sduino_uno)
  
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
  #include "hse_clock.h"
#undef _MAIN_


/*----------------------------------------------------------
    MACROS / DEFINES
----------------------------------------------------------*/

// define LED pin
#if defined(STM8S_DISCOVERY)
  #define PORT_LED        (GPIOD)         // LED port 
  #define PIN_LED         (GPIO_PIN_0)    // LED pin = STM8 PD0
#elif defined(STM8S_UNO)
  #define PORT_LED        (GPIOC)         // LED port 
  #define PIN_LED         (GPIO_PIN_5)    // LED pin = STM8 PC5
#else
  #error Board not supported
#endif



/////////////////
//  main routine
/////////////////
void main(void)
{
  uint32_t  period_LED;     // LED blink period [ms]. Depends on HSE state
  uint32_t  lastLED=0;      // for SW scheduler


  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // Initialize LED pins as output low
  GPIO_Init(PORT_LED, PIN_LED, GPIO_MODE_OUT_PP_LOW_FAST);

  // switch to external HSE clock with ~6ms timeout. Set LED blinking period based on status
  if (switch_HSE_clock(10000) == SUCCESS)
    period_LED = 1000;
  else
    period_LED = 500;
  
  // start 1ms SW clock via TIM4
  init_SW_clock();

  // enable interrupts
  enableInterrupts();


  /////////////
  // main loop
  /////////////
  while (1)
  {
    // task for LED blink
    if (millis() - lastLED >= period_LED)
    {
      lastLED = millis();
      GPIO_WriteReverse(PORT_LED, PIN_LED);
    } // task LED

    // if CSS triggered, set new LED period [ms]
    if (error_CSS == TRUE)
      period_LED = 100;

  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
