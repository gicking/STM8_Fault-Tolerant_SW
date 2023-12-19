/**********************
  declaration of CRC16 checksum calculation routines.

  Use CRC16-CCITT checksum (little endian). For details see e.g.
  https://quickbirdstudios.com/blog/validate-data-with-crc/
**********************/

/*-----------------------------------------------------------------------------
    MODULE DEFINITION FOR MULTIPLE INCLUSION
-----------------------------------------------------------------------------*/
#ifndef _CHECKSUM_CRC16_H_
#define _CHECKSUM_CRC16_H_


/*-----------------------------------------------------------------------------
    INCLUDE FILES
-----------------------------------------------------------------------------*/

#include "stm8s.h"
#include "memory_access.h"


/*-----------------------------------------------------------------------------
    DECLARATION OF GLOBAL FUNCTIONS
-----------------------------------------------------------------------------*/

/// @brief initialize checksum value (CRC16_CCITT)
#define  crc16_ccitt_initialize()    ( 0xFFFF )

/// @brief update checksum value with next byte (CRC16_CCITT)
uint16_t crc16_ccitt_update(uint16_t Chk, uint8_t Data);

/// @brief finalize checksum value (CRC16_CCITT)
#define  crc16_ccitt_finalize(Chk)   ( Chk )

/// @brief calculate checksum over address range (CRC16_CCITT)
uint16_t crc16_ccitt_range(const uint32_t AddrStart, const uint32_t AddrEnd);


/*-----------------------------------------------------------------------------
    END OF MODULE DEFINITION FOR MULTIPLE INLUSION
-----------------------------------------------------------------------------*/
#endif // _CHECKSUM_CRC16_H_
