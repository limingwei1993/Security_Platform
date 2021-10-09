/*
 * flash.h
 *
 *  Created on: 2021Äê8ÔÂ3ÈÕ
 *      Author: 15755
 */

#ifndef COMMUNICATION_FLASH_H_
#define COMMUNICATION_FLASH_H_
#include "HL_hal_stdtypes.h"
#include "F021.h"

enum e_flash_status
{
    flash_succeed,
    flash_failure,
};

struct Sector_List
{
    Fapi_FlashBankType banks_num;
    uint32_t sector_num;
    uint32_t low_addr;
    uint32_t size;
};
#define SECTOR_NUM (16+16+32)

enum e_flash_status Flash_EraseSector(Fapi_FlashBankType oNewFlashBank, uint32_t sector);
void Internal_FLASH_write(uint32 addr, uint8* buff, uint8 len);

void Internal_FLASH_read(uint32 addr, uint8* buff, uint8 len);
#endif /* COMMUNICATION_FLASH_H_ */
