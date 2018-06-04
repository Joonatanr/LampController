/*
 * flashmem.h
 *
 *  Created on: 31. mai 2018
 *      Author: JRE
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "typedefs.h"


#define FLASH_INFO_AREA_BEGIN_ADDR 0x1080u /* Start of INFOB area */
#define FLASH_INFO_AREA_SIZE 0x40u         /* 64 bytes - Should be more than enough */


extern void flash_erase(void);
extern void flash_write(U8 data_begin_slot, const U8 * data_ptr, U8 data_size);
extern U8   flash_read_byte(U8 slot);



#endif /* FLASH_H_ */
