/**********************
  macros for triggering SW reset
**********************/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _SW_RESET_H_
#define _SW_RESET_H_


/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/

#include "stm8s.h"


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL MACROS
-----------------------------------------------------------------------------*/

/// reset via illegal opcode (works for all devices). Opcode from SPL example "RST_IllegalOpcode". Note: 0x00 doesn't work reliably
#if defined(__CSMC__)
  #define SW_RESET_ILLOP()          _asm("dc.b $75")                    // Cosmic           
#elif defined(__ICCSTM8__)
  #define SW_RESET_ILLOP()          __asm("dc8 0x75")                   // IAR
#elif defined(__SDCC)
  #define SW_RESET_ILLOP()          __asm__(".db 0x75")                 // SDCC
#else
  #error: compiler not supported
#endif


/// reset via WWDG timeout watchdog (not supported by all STM8 devices)
#define SW_RESET_WWDG()             (WWDG->CR = WWDG_CR_WDGA)


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _SW_RESET_H_
