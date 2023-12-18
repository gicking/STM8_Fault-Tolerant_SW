/**********************
  implementation of functions for stdio input/output
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "uart_stdio.h"


/*----------------------------------------------------------
    FUNCTIONS
----------------------------------------------------------*/

/**
  \fn char putchar(char c)
   
  \brief output function for printf() via UART
  \param[in]  c   byte to output to UART
   
  Output function for printf() via UART. 
  Pointers to UART functions need to be configured first, see "uart_stdio.h"  
  Type is compiler and version dependent
*/
#if defined (_COSMIC_)
  char putchar(char c)
#elif defined (_SDCC_)
  int putchar(int c)
#endif 
{
  // send byte via UART
  (*g_UART_SendData8)(c);

  // wait for end of transmission
  while ((*g_UART_GetFlagStatus)(UART1_FLAG_TXE) == RESET);

  // return sent byte
  return (c);

} // putchar()



/**
  \fn char getchar(void)
   
  \brief input function for getchar() via UART
  \return byte read from UART
   
  Input function for getchar() via UART. 
  Pointers to UART functions need to be configured first, see "uart_stdio.h"  
  Type is compiler and version dependent
*/
#if defined (_COSMIC_)
  char getchar(void)
#elif defined (_SDCC_)
  int getchar(void)
#endif 
{
  // wait for byte received via UART
  while ((*g_UART_GetFlagStatus)(UART1_FLAG_RXNE) == RESET);

  // read and return byte
  return ((*g_UART_ReceiveData8)());

} // getchar()

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
