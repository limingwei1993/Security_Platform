/*
 * flash.c
 *
 *  Created on: 2021Äê8ÔÂ3ÈÕ
 *      Author: 15755
 */


#include "flash.h"
#include "F021.h"

struct Sector_List gSector_List[SECTOR_NUM] =
{
     {Fapi_FlashBank0,    0,      0x00000000, 16*1024},
     {Fapi_FlashBank0,    1,      0x00004000, 16*1024},
     {Fapi_FlashBank0,    2,      0x00008000, 16*1024},
     {Fapi_FlashBank0,    3,      0x0000C000, 16*1024},
     {Fapi_FlashBank0,    4,      0x00010000, 16*1024},
     {Fapi_FlashBank0,    5,      0x00014000, 16*1024},
     {Fapi_FlashBank0,    6,      0x00018000, 32*1024},
     {Fapi_FlashBank0,    7,      0x00020000, 128*1024},
     {Fapi_FlashBank0,    8,      0x00040000, 128*1024},
     {Fapi_FlashBank0,    9,      0x00060000, 128*1024},
     {Fapi_FlashBank0,    10,     0x00080000, 256*1024},
     {Fapi_FlashBank0,    11,     0x000C0000, 256*1024},
     {Fapi_FlashBank0,    12,     0x00010000, 256*1024},
     {Fapi_FlashBank0,    13,     0x00014000, 256*1024},
     {Fapi_FlashBank0,    14,     0x00018000, 256*1024},
     {Fapi_FlashBank0,    15,     0x0001C000, 256*1024},
     {Fapi_FlashBank1,    0,      0x00020000, 128*1024},
     {Fapi_FlashBank1,    1,      0x00022000, 128*1024},
     {Fapi_FlashBank1,    2,      0x00024000, 128*1024},
     {Fapi_FlashBank1,    3,      0x00026000, 128*1024},
     {Fapi_FlashBank1,    4,      0x00028000, 128*1024},
     {Fapi_FlashBank1,    5,      0x0002A000, 128*1024},
     {Fapi_FlashBank1,    6,      0x0002C000, 128*1024},
     {Fapi_FlashBank1,    7,      0x0002E000, 128*1024},
     {Fapi_FlashBank1,    8,      0x00030000, 128*1024},
     {Fapi_FlashBank1,    9,      0x00032000, 128*1024},
     {Fapi_FlashBank1,    10,     0x00034000, 128*1024},
     {Fapi_FlashBank1,    11,     0x00036000, 128*1024},
     {Fapi_FlashBank1,    12,     0x00038000, 128*1024},
     {Fapi_FlashBank1,    13,     0x0003A000, 128*1024},
     {Fapi_FlashBank1,    14,     0x0003C000, 128*1024},
     {Fapi_FlashBank1,    15,     0x0003E000, 128*1024},
     {Fapi_FlashBank7,    0,      0xF0200000, 4*1024},
     {Fapi_FlashBank7,    1,      0xF0201000, 4*1024},
     {Fapi_FlashBank7,    2,      0xF0202000, 4*1024},
     {Fapi_FlashBank7,    3,      0xF0203000, 4*1024},
     {Fapi_FlashBank7,    4,      0xF0204000, 4*1024},
     {Fapi_FlashBank7,    5,      0xF0205000, 4*1024},
     {Fapi_FlashBank7,    6,      0xF0206000, 4*1024},
     {Fapi_FlashBank7,    7,      0xF0207000, 4*1024},
     {Fapi_FlashBank7,    8,      0xF0208000, 4*1024},
     {Fapi_FlashBank7,    9,      0xF0209000, 4*1024},
     {Fapi_FlashBank7,    10,      0xF020A000, 4*1024},
     {Fapi_FlashBank7,    11,      0xF020B000, 4*1024},
     {Fapi_FlashBank7,    12,      0xF020C000, 4*1024},
     {Fapi_FlashBank7,    13,      0xF020D000, 4*1024},
     {Fapi_FlashBank7,    14,      0xF020E000, 4*1024},
     {Fapi_FlashBank7,    15,      0xF020F000, 4*1024},
     {Fapi_FlashBank7,    16,      0xF0210000, 4*1024},
     {Fapi_FlashBank7,    17,      0xF0211000, 4*1024},
     {Fapi_FlashBank7,    18,      0xF0212000, 4*1024},
     {Fapi_FlashBank7,    19,      0xF0213000, 4*1024},
     {Fapi_FlashBank7,    20,      0xF0214000, 4*1024},
     {Fapi_FlashBank7,    21,      0xF0215000, 4*1024},
     {Fapi_FlashBank7,    22,      0xF0216000, 4*1024},
     {Fapi_FlashBank7,    23,      0xF0217000, 4*1024},
     {Fapi_FlashBank7,    24,      0xF0218000, 4*1024},
     {Fapi_FlashBank7,    25,      0xF0219000, 4*1024},
     {Fapi_FlashBank7,    26,      0xF021A000, 4*1024},
     {Fapi_FlashBank7,    27,      0xF021B000, 4*1024},
     {Fapi_FlashBank7,    28,      0xF021C000, 4*1024},
     {Fapi_FlashBank7,    29,      0xF021D000, 4*1024},
     {Fapi_FlashBank7,    30,      0xF021E000, 4*1024},
     {Fapi_FlashBank7,    31,      0xF021F000, 4*1024},

};

void Internal_FLASH_write(uint32 addr, uint8* buff, uint8 len)
{
    uint8 multiple=0;
    uint8 remainder=0;
    uint8 total=0;
    uint8 i=0;
        Fapi_StatusType oReturnCheck = Fapi_Status_Success;
        oReturnCheck = Fapi_initializeFlashBanks(150);
        if((oReturnCheck == Fapi_Status_Success) && (FLASH_CONTROL_REGISTER->FmStat.FMSTAT_BITS.BUSY != Fapi_Status_FsmBusy))
        {

            if((addr <= 0x003FFFFF) )
            {
                if(addr <= 0x003FFFFF)
                oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
                else
                    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank1);
                oReturnCheck = Fapi_enableMainBankSectors(0xffff);
            }
            else if( (addr >= 0xF0200000 ) && (addr<= 0xF021FFFF))
            {
                oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank7);
                oReturnCheck = Fapi_enableEepromBankSectors(0xffffffff ,0xffffffff);
            }
            multiple=len/8;
            remainder=len%8;
            total=multiple+(remainder>0? 1:0);
            for(i=0;i<total;i++)
            {
                while(FLASH_CONTROL_REGISTER->FmStat.FMSTAT_BITS.BUSY == Fapi_Status_FsmBusy);
                oReturnCheck = Fapi_issueProgrammingCommand((uint32_t*)(addr+8*(i)), (buff+8*(i)), 8, 0, 0, Fapi_AutoEccGeneration);
            }
            /* Place specific example code here */
            /* Wait for FSM to finish */
            while(FLASH_CONTROL_REGISTER->FmStat.FMSTAT_BITS.BUSY == Fapi_Status_FsmBusy);
        }
}

void Internal_FLASH_read(uint32 addr, uint8* buff, uint8 len)
{
    memcpy(buff, addr, len);
}


