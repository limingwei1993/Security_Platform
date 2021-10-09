/*
 * EMIF.h
 *
 *  Created on: 2021Äê8ÔÂ3ÈÕ
 *      Author: 15755
 */

#ifndef COMMUNICATION_EMIF_H_
#define COMMUNICATION_EMIF_H_
#include "HL_hal_stdtypes.h"
#define EMIF_BASE_ADDRESS ((volatile uint32 *)(0x80000000U))
enum EMIF_PAGE_SIZE
{
    elements_256 = 0U,
    elements_512 = 1U,
    elements_1024 = 2U,
    elements_2048 = 3U
};
void EMIF_IO_Configure(void);
void EMIF_init(void);
void EMIF_write(uint32 write_addr,uint8 *write_buff,uint32 write_len);
void EMIF_read(uint32 read_addr,uint8 * read_buff,uint32 read_len);
#endif /* COMMUNICATION_EMIF_H_ */
