/**********************
  implementation of Fletcher's-16 checksum calculation routines.

  Fletcher-16 is a compromise between safer CRC16 (better, slow) and 
  XOR checksums (fast but low coverage). For details see
  https://www.tutorialspoint.com/fletcher-s-checksum
**********************/

/*----------------------------------------------------------
    INCLUDE FILES
----------------------------------------------------------*/
#include "checksum_fletcher16.h"


/*----------------------------------------------------------
    FUNCTIONS
----------------------------------------------------------*/

/**
  \fn uint16_t fletcher16_chk_update(uint16_t Chk, uint8_t Data)
   
  \brief update checksum value with next byte (Fletcher-16)

  \param[in]  Chk   old checksum value
  \param[in]  Data  new data byte

  \return updated checksum value

  Update Fletcher-16 checksum value with new data byte.
  CPU runtime @ 16MHz for SDCC: 6.9us
*/
uint16_t fletcher16_chk_update(uint16_t Chk, uint8_t Data)
{
  // get individual checksums
  uint16_t sum1 = (uint8_t)(Chk);
  uint16_t sum2 = (uint8_t)(Chk >> 8);
  
  // update individual checksums
  sum1 = (sum1 + Data) % 255;
  sum2 = (sum2 + sum1) % 255;

  // return combined result
  return (uint16_t)((sum2 << 8) | sum1);

} // fletcher16_chk_update()


/**
  \fn uint16_t fletcher16_chk_range(const uint32_t AddrStart, const uint32_t AddrEnd)

  \brief calculate Fletcher-16 checksum

  \param[in] AddrStart  starting address (inclusive)
  \param[in] AddrEnd    last address (inclusive)

  \return Fletcher-16 checksum
  
  Calculate Fletcher-16 checksum over specified memory range. 
*/
uint16_t fletcher16_chk_range(const uint32_t AddrStart, const uint32_t AddrEnd)
{
	uint32_t  addr = AddrStart;
	uint16_t  val;
	uint16_t  sum1 = 0;
  uint16_t  sum2 = 0;

  while (addr <= AddrEnd)
  {
    val = (uint16_t) read_1B_far(addr++);
    sum1 = (sum1 + val) % 255;
    sum2 = (sum2 + sum1) % 255;
  }

  return (sum2 << 8) | sum1;

} // fletcher16_chk_range()

/*-----------------------------------------------------------------------------
    END OF MODULE
-----------------------------------------------------------------------------*/
