/**********************
  
  Dummy test project to try out things
    
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
#include "stm8s_uart3.h"
#include "stm8s_wwdg.h"
#include "stdio.h"
#define _MAIN_            // required for global variables
  #include "sw_clock.h"
  #include "uart_stdio.h"
#undef _MAIN_


/*----------------------------------------------------------
    MACROS / DEFINES
----------------------------------------------------------*/

// define LED pin
#if defined(STM8S_NUCLEO_207K8)
  #define PORT_TEST       (GPIOC)         // LED port 
  #define PIN_LED         (GPIO_PIN_5)    // LED pin = board D13 = STM8 PC5
#else
  #error Board not supported
#endif

// LED blink period [ms]
#define LED_PERIOD      500

// communication speed [Baud]
#define BAUDRATE        115200L


// see e.g. http://www.gtoal.com/compilers101/small_c/gbdk/sdcc/doc/sdccman.html/node31.html
// return 0: initialize global variables
unsigned char _sdcc_external_startup()
{
    TIM1->ARRH = 100;
    TIM1->ARRL = 101;
    WWDG->CR = WWDG_CR_WDGA;

    return 1;
}


/////////////////
//  main routine
/////////////////
void main(void)
{
  uint32_t  lastLED=0, timeStart;
  char      c;

  /*
  __asm
    ldw x, _val
    incw x
    ldw _val, x
  __endasm;
  */


  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // Initialize LED pin as output low
  GPIO_Init(PORT_TEST, PIN_LED, GPIO_MODE_OUT_PP_LOW_FAST);

  // Configure UART3 for 115kBaud, 8N1
  UART3_Init(BAUDRATE, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO, UART3_MODE_TXRX_ENABLE);

  // bind stdio input/output to UART3
  g_UART_SendData8 = &UART3_SendData8;
  g_UART_ReceiveData8 = &UART3_ReceiveData8;
  g_UART_GetFlagStatus = &UART3_GetFlagStatus;

  // start 1ms clock via TIM4
  init_SW_clock();

  // enable interrupts
  enableInterrupts();


  /////////////
  // main loop
  /////////////
  while (1)
  {
    // task for LED blink
    if (millis() - lastLED >= LED_PERIOD)
    {
      lastLED = millis();
      GPIO_WriteReverse(PORT_TEST, PIN_LED);
      
      int i = (int) TIM1->ARRH;
      int j = (int) TIM1->ARRL;
      printf("%d  %d\n", i, j);

    } // task LED

  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
