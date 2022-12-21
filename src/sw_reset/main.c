/**********************
  
  Trigger SW reset via illegal opcode or WWDG watchdog 

  Functionality:
    - initialization:
      - configure LED pin to output
      - initialize SW clock
      - configure UART @ 115.2kBaud / 8N1 
      - print reset source via UART
    - main loop
      - blink LED periodically
      - if UART receives
        - 'r': trigger illegal opcode reset (ILLOP)
        - 'R': trigger window watchdog reset (WWDG)

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
#include "stm8s_rst.h"
#include "stdio.h"
#define _MAIN_            // required for global variables
  #include "sw_clock.h"
  #include "uart_stdio.h"
  #include "sw_reset.h"
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



/////////////////
//  main routine
/////////////////
void main(void)
{
  // for SW scheduler
  uint32_t  lastLED=0;
  

  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // Initialize LED pins as output low
  GPIO_Init(PORT_TEST, PIN_LED, GPIO_MODE_OUT_PP_LOW_FAST);

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
  printf("\nreset source (0x%02x): ", (int) RST->SR);
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
    
    
    // execute UART command
    if (UART3_GetFlagStatus(UART3_FLAG_RXNE))
    {
      char c = getchar();
      
      // if 'r' received, trigger SW reset via illegal opcode
      if (c == 'r')
      {
        // print message
        printf("trigger ILLOP reset\n");

        // trigger SW reset via illegal opcode
        SW_RESET_ILLOP();

      } // received 'r'

      // if 'R' received, trigger SW reset via WWDG
      else if (c == 'R')
      {
        // print message
        printf("trigger WWDG reset\n");

        // trigger SW reset via window watchdog
        SW_RESET_WWDG();

      } // received 'R'

    } // byte received
 
  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
