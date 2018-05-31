/*
 * flashmem.c
 *
 *  Created on: 31. mai 2018
 *      Author: JRE
 */

#include <flash.h>
#include "msp430g2553.h"

//*************************************************************************

// Erase Information Flash

//*************************************************************************

Public void flash_erase(void)
{
  int * addr;
  addr = (int*)FLASH_INFO_AREA_BEGIN_ADDR;

  __disable_interrupt();               // Disable interrupts. This is important, otherwise,
                                       // a flash operation in progress while interrupt may
                                       // crash the system.
  while(BUSY & FCTL3);                 // Check if Flash being used
  FCTL2 = FWKEY + FSSEL_1 + FN3;       // Clk = SMCLK/4
  FCTL1 = FWKEY + ERASE;               // Set Erase bit
  FCTL3 = FWKEY;                       // Clear Lock bit
  *addr = 0;                           // Dummy write to erase Flash segment
  while(BUSY & FCTL3);                 // Check if Flash being used
  FCTL1 = FWKEY;                       // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                // Set LOCK bit
  __enable_interrupt();
}


//*************************************************************************

// Write stored Information flash

//*************************************************************************

void flash_write(U8 data_begin_slot, const U8 * data, U8 data_size)
{
  U8 ix;
  U8 * flash_ptr;

  if ((data_begin_slot + data_size) > FLASH_INFO_AREA_SIZE)
  {
      return;
  }

  __disable_interrupt();
  FCTL2 = FWKEY + FSSEL_1 + FN0;       // Clk = SMCLK/4
  FCTL3 = FWKEY;                       // Clear Lock bit
  FCTL1 = FWKEY + WRT;                 // Set WRT bit for write operation

  flash_ptr = (U8 *)(FLASH_INFO_AREA_BEGIN_ADDR + data_begin_slot);

  for (ix = 0u; ix < data_size; ix++)
  {
      *flash_ptr = data[ix]; //Copy value to flash.

      flash_ptr++;
  }

  FCTL1 = FWKEY;                        // Clear WRT bit
  FCTL3 = FWKEY + LOCK;                 // Set LOCK bit
  while(BUSY & FCTL3);
  __enable_interrupt();
}

U8 flash_read_byte(U8 slot)
{
    U8 * flash_ptr = (U8*)(FLASH_INFO_AREA_BEGIN_ADDR + slot);
    return * flash_ptr;
}

