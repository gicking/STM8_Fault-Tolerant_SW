/**********************
  declaration and macros for CRC16 checksum calculation according to CCITT
  Source: https://www.embeddedrelated.com/showcode/295.php
**********************/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _CRC16_CCITT_H_
#define _CRC16_CCITT_H_


/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/

#include "stm8s.h"


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL MACROS
-----------------------------------------------------------------------------*/

/// CRC16 CCITT polynom (X.25, V.41, HDLC FCS, Bluetooth,...)
#define POLYNOME_CCITT        0x1021


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL VARIABLES
-----------------------------------------------------------------------------*/

// declare or reference to global variables, depending on '_MAIN_'
#if defined(_MAIN_)
  volatile uint32_t           addr_Chk_Start = 0x00;
  volatile uint32_t           addr_Chk_End   = 0x00;
  volatile uint32_t           addr_Chk_Curr  = 0x00;
#else // _MAIN_
  extern volatile uint32_t    addr_Chk_Start;
  extern volatile uint32_t    addr_Chk_End;
  extern volatile uint32_t    addr_Chk_Curr;
#endif // _MAIN_


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL FUNCTIONS
-----------------------------------------------------------------------------*/

/// @brief initialize CRC16 checksum
#if defined(__CSMC__)
  @inline uint16_t crc16_init(void)
#else // SDCC & IAR
  static inline uint16_t crc16_init(void)
#endif
{
  return 0xFFFF;
}


/// @brief update CRC16 checksum with next byte
uint16_t crc16_update(uint16_t crcValue, uint8_t newByte);


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _CRC16_CCITT_H_
