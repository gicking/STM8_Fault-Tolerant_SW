/**********************
  declaration and macros for external clock (HSE) including supervision (CSS)
**********************/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _HSE_CLOCK_H_
#define _HSE_CLOCK_H_


/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/

#include "stm8s.h"


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL VARIABLES
-----------------------------------------------------------------------------*/

// declare or reference to global variables, depending on '_MAIN_'
#if defined(_MAIN_)
  volatile bool               error_CSS = FALSE;
#else // _MAIN_
  extern volatile bool        error_CSS;
#endif // _MAIN_


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL FUNCTIONS
-----------------------------------------------------------------------------*/

/// @brief switch to external HSE clock and enable CSS
ErrorStatus switch_HSE_clock(uint16_t timeout);


/**
  \fn void ISR_CSS_handler(void)
   
  \brief inline handler for CCS
  
  CCS handler. Is called from within actual CLK ISR in stm8s_it.c.
  Inline implementation for minimal latency.
*/
#if defined(__CSMC__)
  @inline void ISR_CSS_handler(void)
#else // SDCC & IAR
  static inline void ISR_CSS_handler(void)
#endif
{
  // clear CSS interrupt flag
  CLK_ClearITPendingBit(CLK_IT_CSSD);

	// disable CSS interrupt (HSE is disabled)
	CLK_ITConfig(CLK_IT_CSSD, DISABLE);

  // set HSI clock to 16MHz (default is 2MHz) 
  CLK->CKDIVR = 0x00;

  // set CSS error flag
  error_CSS = TRUE;

} // ISR_CSS_handler

/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _HSE_CLOCK_H_
