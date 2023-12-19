/**********************
  declaration of Fletcher's-16 checksum calculation routines.

  Fletcher-16 is a compromise between safer CRC16 (better, slow) and 
  XOR checksums (fast but low coverage). For details see
  https://www.tutorialspoint.com/fletcher-s-checksum
**********************/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _CHECKSUM_FLETCHER16_H_
#define _CHECKSUM_FLETCHER16_H_


/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/

#include "stm8s.h"
#include "memory_access.h"


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL FUNCTIONS
-----------------------------------------------------------------------------*/

/// @brief initialize checksum value (Fletcher-16)
#define fletcher16_chk_initialize()  ( 0x0000 )

/// @brief update checksum value with next byte (Fletcher-16)
uint16_t fletcher16_chk_update(uint16_t Chk, uint8_t Data);

/// @brief finalize checksum value (Fletcher-16)
#define fletcher16_chk_finalize(Chk) ( Chk )

/// @brief calculate checksum over specified range (Fletcher-16)
uint16_t fletcher16_chk_range(const uint32_t AddrStart, const uint32_t AddrEnd);


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _CHECKSUM_FLETCHER16_H_
