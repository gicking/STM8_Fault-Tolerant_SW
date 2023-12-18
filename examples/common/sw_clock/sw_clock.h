/**********************
  declaration and macros for SW clock using timer TIM4
**********************/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _SW_CLOCK_H_
#define _SW_CLOCK_H_


/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/

#include "stm8s.h"


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL VARIABLES
-----------------------------------------------------------------------------*/

// declare or reference to global variables, depending on '_MAIN_'
#if defined(_MAIN_)
  volatile bool               g_flagMilli = FALSE;         ///< flag for 1ms timer interrupt. Set in TIM4 ISR
  volatile uint32_t           g_millis = 0;                ///< 1ms counter. Increased in TIM4 ISR
  volatile uint32_t           g_micros = 0;                ///< 1000us counter. Increased in TIM4 ISR
#else // _MAIN_
  extern volatile bool        g_flagMilli;
  extern volatile uint32_t    g_millis;
  extern volatile uint32_t    g_micros;
#endif // _MAIN_


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL MACROS
-----------------------------------------------------------------------------*/

#define flagMilli()           g_flagMilli                   ///< 1ms flag. Set in 1ms ISR
#define clearFlagMilli()      g_flagMilli=FALSE             ///< clear 1ms flag



/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL FUNCTIONS
-----------------------------------------------------------------------------*/

/// init 1ms master SW clock
void init_SW_clock(void);

/// get milliseconds since start of program
uint32_t millis(void);

/// delay code execution for specified milliseconds
void delay(uint32_t ms);


/**
  \fn uint32_t micros(void)
   
  \brief get microseconds since start of program. Resolution is 4us
  
  \return microseconds from start of program

  Get microseconds from start of program with 4us resolution. 
  Requires TIM4 to be initialized and running, and TIM4 interrupt being active.
  Value overruns every ~1.2 hours.
  Inline implementation for minimal latency.
*/
#if defined(__CSMC__)
  @inline uint32_t micros(void)
#else // SDCC & IAR
  static inline uint32_t micros(void)
#endif
{
  // for consistency of CNT ans SR briefly stop timer. Use direct register access for low latency
  TIM4->CR1 &= TIM4_CR1_CEN;

  // get current us value, TIM4 counter, and TIM4 overflow flag
  uint8_t cnt = TIM4->CNTR;
  #if defined(TIM4_SR1_RESET_VALUE)
    uint8_t uif = TIM4->SR1;
  #else
    uint8_t uif = TIM4->SR;
  #endif
  
  // restart timer immediately to minimize time gap. Use direct register access for low latency
  TIM4->CR1 |= TIM4_CR1_CEN;
  

  // store TIM4 interrupt state and disable it to avoid race condition with TIM4 ISR
  uint8_t oldTIM4_IER = TIM4->IER;
  TIM4->IER &= ~TIM4_IT_UPDATE;

  // copy micros from global variable
  uint32_t us = g_micros;

  // restore original TIM4 TIM4 interrupt state
  TIM4->IER = oldTIM4_IER;


  // calculate current time [us], including global variable (1000us steps) and counter value (4us steps)
  #if defined(__CSMC__)     // Cosmic compiler has a re-entrance bug with bitshift
    us += 4 * (uint16_t) cnt;
  #else
    us += ((uint16_t) cnt) << 2;
  #endif
  
  // account for possible overflow of TIM4 --> check UIF (= bit 0)
  if ((uif & 0x01) && (cnt != 250))
    us += 1000L;

  // return current micros
  return(us);
  
} // micros()



/**
  \fn void delayMicroseconds(uint32_t us)
   
  \brief delay code execution for 'us'. Resolution is 4us
  
  \param[in]  us   duration[us] to halt code
   
  Delay code execution for 'us'. Resolution is 4us, offset/latency is ~10us.
  Requires TIM4 to be initialized and running, and TIM4 interrupt being active.
  Inline implementation for minimal latency.
*/
#if defined(__CSMC__)
  @inline void delayMicroseconds(uint32_t us)
#else // SDCC & IAR
  static inline void delayMicroseconds(uint32_t us)
#endif
{
  // wait until time [us] has passed
  uint32_t start = micros();
  while (micros() - start < us)
    nop();

} // delayMicroseconds()



/**
  \fn void ISR_TIM4_handler(void)
   
  \brief inline overflow handler for TIM4
  
  TIM4 overflow handler. Is called from within actual TIM4 ISR in stm8s_it.c.
  Inline implementation for minimal latency.
*/
#if defined(__CSMC__)
  @inline void ISR_TIM4_handler(void)
#else // SDCC & IAR
  static inline void ISR_TIM4_handler(void)
#endif
{
  // clear timer 4 interrupt flag
  TIM4_ClearITPendingBit(TIM4_IT_UPDATE);

  // set/increase global variables
  g_micros += 1000L;
  g_millis++;
  g_flagMilli = TRUE;

} // ISR_TIM4_handler

/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _SW_CLOCK_H_
