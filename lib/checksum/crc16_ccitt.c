/**********************
  implementation and macros for CRC16 checksum calculation according to CCITT
  Source: https://www.embeddedrelated.com/showcode/295.php
**********************/


/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "crc16_ccitt.h"


/*----------------------------------------------------------
    FUNCTIONS
----------------------------------------------------------*/

/**
  \fn uint16_t crc16_update(uint16_t crcValue, uint8_t newByte)
   
  \brief update CRC16 checksum with next byte

  \param crcValue   old CRC16 checksum value
  \param newByte    next byte to update with
   
  \return new CRC16 checksum value

  update CRC16 checksum with next byte.
*/
uint16_t crc16_update(uint16_t crcValue, uint8_t newByte)
{
  for (uint8_t i=0; i<8; i++)
  {
    if (((crcValue & 0x8000) >> 8) ^ (newByte & 0x80))
      crcValue = (crcValue << 1)  ^ POLYNOME_CCITT;
    else
      crcValue = (crcValue << 1);
		newByte <<= 1;
  }
	
  return crcValue;

} // crc16_update()

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
