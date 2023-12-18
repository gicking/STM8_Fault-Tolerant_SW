/**********************
  implementation of routines for external clock (HSE) including supervision (CSS)
**********************/


/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "hse_clock.h"
#include "stm8s_gpio.h"


/*----------------------------------------------------------
    FUNCTIONS
----------------------------------------------------------*/

/**
  \fn ErrorStatus switch_HSE_clock(uint16_t timeout)
   
  \brief init external HSE clock

  \param timeout  HSE activation timeout (10000 ~ 6.2ms)
   
  \return switch to HSE ok(=SUCCESS) or failed(=ERROR)

  switch to external HSE clock with timeout and CSS enabled with interrupt
*/
ErrorStatus switch_HSE_clock(uint16_t timeout)
{
  // reset CLK to default settings
  CLK_DeInit();
  
  // set HSI and HSE prescaler to 1 and fCPU=fMaster
  CLK->CKDIVR = 0x00;
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
    
  // disable clock switch interrupt (just to be sure)
  CLK_ITConfig(CLK_IT_SWIF, DISABLE);

  // reset clk ISR flag
  CLK->SWCR &= ~((uint8_t) CLK_SWCR_SWIF);          
  
  // set HSE as new clock
  CLK->SWR = (uint8_t) CLK_SOURCE_HSE;
    
  // wait until HSW is ready or timeout occurred (1e4~6.2ms)
  while ((!(CLK->SWCR & CLK_SWCR_SWIF)) && (timeout--))
    nop();
  
  // HSE is stable -> enable CSS and switch to HSE 
  if (CLK->SWCR & CLK_SWCR_SWIF)
  {
    // clear CSS error flag (set in CSS ISR handler)
    error_CSS = FALSE;
    
    // clear CSS interrupt flag (just to be sure)
    CLK_ClearITPendingBit(CLK_IT_CSSD);
      
    // enable CSS interrupt
    CLK_ITConfig(CLK_IT_CSSD, ENABLE);
      
    // enable CCS
    CLK_ClockSecuritySystemEnable();

    // switch to HSE
    CLK->SWCR |= CLK_SWCR_SWEN;
    
    // return success
    return SUCCESS;

  } // if HSE stable 

  // HSE start-up timeout .> return error
  return ERROR;
  
} // switch_HSE_clock

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
