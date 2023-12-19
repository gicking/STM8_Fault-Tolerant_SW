/**********************
  
  Demonstrate treatment of unhandled interrupts.

  Functionality:
    - initialization:
      - configure LED pin to output
      - initialize SW clock
      - configure UART @ 115.2kBaud / 8N1 
      - print reset source via UART
    - main loop
      - blink LED periodically
      - if UART receives
        - 'i': trigger TIM1_UPD interrupt -> call ISR 

  Supported Hardware:
    - Nucleo 8S207K8
  
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"     // required here by SDCC for ISR
#include "stm8s_clk.h"
#include "stm8s_uart3.h"
#include "stm8s_tim1.h"
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
  #define PORT_TEST       (GPIOC)                       ///< LED port 
  #define PIN_LED         (GPIO_PIN_5)                  ///< LED pin = board D13 = STM8 PC5
#else
  #error Board not supported
#endif

/// LED blink period [ms]
#define LED_PERIOD      500

/// communication speed [Baud]
#define BAUDRATE        115200L


/*----------------------------------------------------------
    GLOBAL FUNCTIONS
----------------------------------------------------------*/

/////////////////
//  main routine
/////////////////
void main(void)
{
  uint32_t  lastLED=0;              // for SW scheduler


  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // Initialize LED pins as output high
  GPIO_Init(PORT_TEST, PIN_LED, GPIO_MODE_OUT_PP_HIGH_FAST);

  // start 1ms clock via TIM4
  init_SW_clock();

  // Configure UART3 for 115kBaud, 8N1
  UART3_Init(BAUDRATE, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO, UART3_MODE_TXRX_ENABLE);

  // bind stdio input/output to UART3
  g_UART_SendData8 = &UART3_SendData8;
  g_UART_ReceiveData8 = &UART3_ReceiveData8;
  g_UART_GetFlagStatus = &UART3_GetFlagStatus;

  // enable interrupts
  enableInterrupts();

  // print reset source
  printf("\nreset source: ");
  if (!RST_GetFlagStatus(RST_FLAG_EMCF | RST_FLAG_SWIMF | RST_FLAG_ILLOPF | RST_FLAG_IWDGF | RST_FLAG_WWDGF))
    printf("HW / BOR\n");
  if (RST_GetFlagStatus(RST_FLAG_EMCF))
    printf("EMC\n");
  if (RST_GetFlagStatus(RST_FLAG_SWIMF))
    printf("SWIM\n");
  if (RST_GetFlagStatus(RST_FLAG_ILLOPF))
    printf("ILLOP\n");
  if (RST_GetFlagStatus(RST_FLAG_IWDGF))
    printf("IWDG\n");
  if (RST_GetFlagStatus(RST_FLAG_WWDGF))
    printf("WWDG\n");
  RST_ClearFlag(RST_FLAG_EMCF | RST_FLAG_SWIMF | RST_FLAG_ILLOPF | RST_FLAG_IWDGF | RST_FLAG_WWDGF);
  

  /////////////
  // main loop
  /////////////
  while (1)
  {
    // blink LED to indicate activity
    if (millis() - lastLED > LED_PERIOD)
    {
      lastLED = millis();
      GPIO_WriteReverse(PORT_TEST, PIN_LED);
    } // task LED


    // if byte received from serial monitor
    if (UART3_GetFlagStatus(UART3_FLAG_RXNE))
    {
      char c = getchar();
      
      // if 'i' received, activate TIM1 UPD interrupt
      if (c == 'i')
      {
        // start TIM1 with UPD interrupt enabled
        TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, 1, 0);
        TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
        TIM1_Cmd(ENABLE);

      } // received 'i'

    } // byte received
 
  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
