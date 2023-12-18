/**********************
  
  Serial monitor input/output (for debugging)

  Functionality:
    - initialization:
      - configure UART @ 115.2kBaud / 8N1 
    - main loop
      - read byte from UART and echo it. For convenience
        - use printf() and getchar()
        - skip interrupts and FIFOs
    
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
#include "stdio.h"
#define _MAIN_            // required for global variables
  #include "uart_stdio.h"
#undef _MAIN_


/*----------------------------------------------------------
    MACROS / DEFINES
----------------------------------------------------------*/

// communication speed [Baud]
#define BAUDRATE        115200L


/*----------------------------------------------------------
    FUNCTIONS
----------------------------------------------------------*/

/////////////////
//  main routine
/////////////////
void main(void)
{
  char      c;


  /////////////
  // initialization
  /////////////

  // disable interrupts
  disableInterrupts();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

  // Configure UART3 for 115kBaud, 8N1
  UART3_Init(BAUDRATE, UART3_WORDLENGTH_8D, UART3_STOPBITS_1, UART3_PARITY_NO, UART3_MODE_TXRX_ENABLE);

  // bind stdio input/output to UART3
  g_UART_SendData8 = &UART3_SendData8;
  g_UART_ReceiveData8 = &UART3_ReceiveData8;
  g_UART_GetFlagStatus = &UART3_GetFlagStatus;

  // enable interrupts
  enableInterrupts();

  // print greeting message
  printf("\npress any key\n\n");

  /////////////
  // main loop
  /////////////
  while (1)
  {
    // if byte was received, echo response
    if (UART3_GetFlagStatus(UART3_FLAG_RXNE))
    {
      c = getchar();
      printf("key '%c' pressed (code %d)\n", c, (int) c);  
    }
   
  } // main loop
  
} // main()


/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
