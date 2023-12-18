/**********************
  declaration of functions for stdio input/output
**********************/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _UART_STDIO_H_
#define _UART_STDIO_H_


/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/

#include "stm8s.h"
#include "stm8s_uart1.h"      // only for typedefs 
#include <stdio.h>


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL VARIABLES
-----------------------------------------------------------------------------*/

// declare or reference to global variables, depending on '_MAIN_'
#if defined(_MAIN_)
  volatile void       (*g_UART_SendData8) (uint8_t) = NULL;                            //!< UART send byte function
  volatile uint8_t    (*g_UART_ReceiveData8)(void) = NULL;                             //!< UART read byte function
  volatile FlagStatus (*g_UART_GetFlagStatus) (UART1_Flag_TypeDef UART_FLAG) = NULL;   //!< UART check status function
#else // _MAIN_
  extern volatile void       (*g_UART_SendData8) (uint8_t);
  extern volatile uint8_t    (*g_UART_ReceiveData8)(void);
  extern volatile FlagStatus (*g_UART_GetFlagStatus) (UART1_Flag_TypeDef UART_FLAG);
#endif // _MAIN_


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _UART_STDIO_H_
