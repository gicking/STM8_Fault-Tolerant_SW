/**********************
  implementation of routines for SW clock using timer TIM4
**********************/


/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "sw_clock.h"


/*----------------------------------------------------------
    FUNCTIONS
----------------------------------------------------------*/

/**
  \fn void init_SW_clock(void)
   
  \brief init 1ms master SW clock
   
  init 8-bit timer TIM4 with 1ms tick. Is used for SW master clock via interrupt.
*/
void init_SW_clock(void)
{
  // initialize global clock variables
  g_flagMilli = FALSE;
  g_millis    = 0;
  g_micros    = 0;
  
  // config 1ms clock
  TIM4_DeInit();
  TIM4_TimeBaseInit(TIM4_PRESCALER_64, 250);  // 16MHz/64 = 250kHz; Reload value = 250 -> 1ms overflow
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);
  TIM4_Cmd(ENABLE);

} // init_SW_clock



/**
  \fn uint32_t millis(void)
   
  \brief get milliseconds since start of program. Resolution is 1ms
  
  \return milliseconds from start of program

  Get milliseconds from start of program with 1ms resolution. 
  Requires TIM4 to be initialized and running, and TIM4 interrupt being active.
  Value overruns every ~49.7 days.
*/
uint32_t millis()
{
  // store TIM4 interrupt state and disable it to avoid race condition with TIM4 ISR
  uint8_t oldTIM4_IER = TIM4->IER;
  TIM4->IER &= ~TIM4_IT_UPDATE;

  // copy millis from global variable
  uint32_t ms = g_millis;

  // restore original TIM4 TIM4 interrupt state
  TIM4->IER = oldTIM4_IER;

  // return current millis
  return(ms);

} // millis()



/**
  \fn void delay(uint32_t ms)
   
  \brief delay code execution for 'ms'
  
  \param[in]  ms   duration[ms] to halt code
   
  delay code execution for 'ms'. 
  Requires TIM4 to be initialized and running, and TIM4 interrupt being active.
*/
void delay(uint32_t ms)
{
  // up to 1h use delayMicroseconds() for best resolution
  if (ms < 3600000L)
    delayMicroseconds(ms * 1000L);

  // above, use ms and TIM4 counter
  else
  {
    // get current TIM4 counter value
    uint8_t oldCntr = TIM4->CNTR;
    
    // wait until millis have (roughly) passed
    uint32_t start = millis();
    while (millis() - start < ms)
      nop();

    // wait for sub-ms remainder
    while (TIM4->CNTR - oldCntr)
      nop();

  } // ms > 1hr

} // delay()

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
