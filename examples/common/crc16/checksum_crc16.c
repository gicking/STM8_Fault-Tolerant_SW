/**********************
  implementation of CRC16 checksum calculation routines.

  Use CRC16-CCITT checksum (little endian). For details see e.g.
  https://quickbirdstudios.com/blog/validate-data-with-crc/
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "checksum_crc16.h"


/*----------------------------------------------------------
    FUNCTIONS
----------------------------------------------------------*/

/**
  \fn uint16_t crc16_ccitt_update(uint16_t Chk, uint8_t Data)
   
  \brief update CRC16 checksum value with next byte

  \param[in]  Chk   old checksum value
  \param[in]  Data  new data byte

  \return updated CRC16 value

  Update CRC16-CCITT checksum value with new data byte.
*/
uint16_t crc16_ccitt_update(uint16_t Chk, uint8_t Data)
{
  // for little endian shift 8 to left
  Chk = Chk ^ (((uint16_t) (Data)) << 8);

  for (uint8_t i = 0; i < 8; i++)
  {
    if (Chk & 0x8000)
      Chk = (Chk << 1) ^ 0x1021;  // CCITT polynom
    else
      Chk <<= 1;
  }

  return Chk;

} // crc16_ccitt_update()


/**
  \fn uint16_t crc16_ccitt_range(const uint32_t AddrStart, const uint32_t AddrEnd)

  \brief calculate CRC16 checksum over range

  \param[in] AddrStart  first address (inclusive)
  \param[in] AddrEnd    last address (inclusive)

  \return CRC16 checksum
  
  Calculate CRC16-CCITT checksum over address range. 
*/
uint16_t crc16_ccitt_range(const uint32_t AddrStart, const uint32_t AddrEnd)
{
  uint32_t  addr = AddrStart;
  uint16_t  chk;
  uint8_t   val;

  chk = crc16_ccitt_initialize();
  while (addr <= AddrEnd)
  {
    val = read_1B_far(addr++);
    chk = crc16_ccitt_update(chk, val); 
  }
  chk = crc16_ccitt_finalize(chk);

  return chk;

} // crc16_ccitt_range()

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
