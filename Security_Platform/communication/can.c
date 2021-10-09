/*
 * can.c
 *
 *  Created on: 2021Äê7ÔÂ29ÈÕ
 *      Author: 15755
 */

#include"can.h"
#include"HL_can.h"
#include"HL_reg_can.h"
#include "HL_sys_dma.h"

uint8 static s_canByteOrder[8U] = {3U, 2U, 1U, 0U, 7U, 6U, 5U, 4U};
uint8 CAN1_RX_DATA[CAN1_RX_DATA_LEN];
uint8 CAN2_RX_DATA[CAN2_RX_DATA_LEN];
uint8 CAN3_RX_DATA[CAN3_RX_DATA_LEN];
uint8 CAN4_RX_DATA[CAN4_RX_DATA_LEN];
uint8 CAN1_TX_DATA[CAN1_TX_DATA_LEN];
uint8 CAN2_TX_DATA[CAN2_TX_DATA_LEN];
uint8 CAN3_TX_DATA[CAN3_TX_DATA_LEN];
uint8 CAN4_TX_DATA[CAN4_TX_DATA_LEN];
bool DMA_CAN1_TX=true;
bool DMA_CAN2_TX=false;
bool DMA_CAN3_TX=false;
bool DMA_CAN4_TX=false;
bool DMA_CAN1_RX=true;
bool DMA_CAN2_RX=true;
bool DMA_CAN3_RX=true;
bool DMA_CAN4_RX=true;

g_dmaCTRL g_dmaCTRLPKT_CAN1_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_CAN1_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_CAN2_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_CAN2_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_CAN3_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_CAN3_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_CAN4_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_CAN4_RX;             /* dma control packet configuration stack */
CAN_McanMESSAGE_BOX CAN1_McanMESSAGE_BOX_TABLE[CAN_MCANMESSAGE_BOX_NUM]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
CAN_McanMESSAGE_BOX CAN2_McanMESSAGE_BOX_TABLE[CAN_MCANMESSAGE_BOX_NUM]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
CAN_McanMESSAGE_BOX CAN3_McanMESSAGE_BOX_TABLE[CAN_MCANMESSAGE_BOX_NUM]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
CAN_McanMESSAGE_BOX CAN4_McanMESSAGE_BOX_TABLE[CAN_MCANMESSAGE_BOX_NUM]={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                                                                        {0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
CAN_MSG_Revice CAN1_MSG_buff;
CAN_MSG_Revice CAN2_MSG_buff;
CAN_MSG_Revice CAN3_MSG_buff;
CAN_MSG_Revice CAN4_MSG_buff;
void CAN_init(CAN_Info canx)
{
    uint32 bau=0;
    bau=canx.burt;
    switch(canx.ch)
    {
        case CAN1:
            canREG1->CTL = (uint32)0x00000200U
                             | (uint32)0x00000000U
                             | (uint32)((uint32)0x00000005U  << 10U)
                             | (0x01 << 18) | (0x01 << 19) | (0x01 << 20)
                             | (uint32)0x00020043U;

                /** - Clear all pending error flags and reset current status */
                canREG1->ES |= 0xFFFFFFFFU;

                /** - Assign interrupt level for messages */
                canREG1->INTMUXx[0U] = (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U;

                canREG1->INTMUXx[1U] = (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U;

                /** - Setup auto bus on timer period */
                canREG1->ABOTR = (uint32)0U;

                /** - Initialize message 1
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */


                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)1U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 1U;
                canREG1->IF1STAT |= (0x01 <<6);
                /** - Initialize message 2
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)2U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 2U;

                /** - Initialize message 3
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)3U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 3U;

                /** - Initialize message 4
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)4U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 4U;

                /** - Initialize message 5
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)5U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 5U;

                /** - Initialize message 6
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)6U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 6U;

                /** - Initialize message 7
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)7U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 7U;

                /** - Initialize message 8
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)8U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 8U;

                /** - Initialize message 9
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)9U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 9U;

                /** - Initialize message 10
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)10U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 10U;

                /** - Initialize message 11
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)11U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 11U;

                /** - Initialize message 12
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)12U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 12U;

                /** - Initialize message 13
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)13U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 13U;

                /** - Initialize message 14
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)14U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 14U;

                /** - Initialize message 15
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)15U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 15U;

                /** - Initialize message 16
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)16U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 16U;

                /** - Initialize message 17
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)17U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 17U;

                /** - Initialize message 18
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)18U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 18U;

                /** - Initialize message 19
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)19U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 19U;

                /** - Initialize message 20
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)20U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 20U;

                /** - Initialize message 21
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)21U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 21U;

                /** - Initialize message 22
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)22U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 22U;

                /** - Initialize message 23
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)23U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 23U;

                /** - Initialize message 24
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)24U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 24U;

                /** - Initialize message 25
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)25U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 25U;

                /** - Initialize message 26
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)26U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 26U;

                /** - Initialize message 27
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)27U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 27U;

                /** - Initialize message 28
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)28U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 28U;

                /** - Initialize message 29
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)29U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 29U;

                /** - Initialize message 30
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)30U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 30U;

                /** - Initialize message 31
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)31U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 31U;

                /** - Initialize message 32
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)32U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 32U;

                /** - Initialize message 33
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)33U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 33U;

                /** - Initialize message 34
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)34U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 34U;

                /** - Initialize message 35
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)35U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 35U;

                /** - Initialize message 36
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)36U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 36U;

                /** - Initialize message 37
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)37U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 37U;

                /** - Initialize message 38
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)38U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 38U;

                /** - Initialize message 39
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)39U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 39U;

                /** - Initialize message 40
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)40U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 40U;

                /** - Initialize message 41
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)41U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 41U;

                /** - Initialize message 43
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)43U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 43U;

                /** - Initialize message 44
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)44U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 44U;

                /** - Initialize message 45
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)45U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 45U;

                /** - Initialize message 46
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)46U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 46U;

                /** - Initialize message 47
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)47U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 47U;

                /** - Initialize message 48
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)48U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 48U;

                /** - Initialize message 49
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)49U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 49U;

                /** - Initialize message 50
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)50U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 50U;

                /** - Initialize message 51
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)51U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 51U;

                /** - Initialize message 52
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)52U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 52U;

                /** - Initialize message 53
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)53U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 53U;

                /** - Initialize message 54
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)54U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 54U;

                /** - Initialize message 55
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)55U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 55U;

                /** - Initialize message 56
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)56U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 56U;

                /** - Initialize message 57
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)57U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 57U;

                /** - Initialize message 58
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)58U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 58U;

                /** - Initialize message 59
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)59U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 59U;

                /** - Initialize message 60
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)60U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 60U;

                /** - Initialize message 61
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)61U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 61U;

                /** - Initialize message 62
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)62U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 62U;

                /** - Initialize message 63
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)63U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF1CMD  = (uint8) 0xF8U;
                canREG1->IF1NO   = 63U;

                /** - Initialize message 64
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG1->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x00000000U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)64U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG1->IF2MCTL = 0x00001000U | (uint32)0x00000400U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG1->IF2CMD  = (uint8) 0xF8U;
                canREG1->IF2NO   = 64U;

                /** - Setup IF1 for data transmission
                *     - Wait until IF1 is ready for use
                *     - Set IF1 control byte
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */
                canREG1->IF1CMD  = 0x87U ;

                /** - Setup IF2 for reading data
                *     - Wait until IF1 is ready for use
                *     - Set IF1 control byte
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG1->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */
                canREG1->IF2CMD = 0x17U ;

                /** - Setup bit timing
                *     - Setup baud rate prescaler extension
                *     - Setup TSeg2
                *     - Setup TSeg1
                *     - Setup sample jump width
                *     - Setup baud rate prescaler
                */
                canREG1->BTR = (uint32)((uint32)0U << 16U) |
                               (uint32)((uint32)(4U - 1U) << 12U) |
                               (uint32)((uint32)((3U + 4U) - 1U) << 8U) |
                               (uint32)((uint32)(4U - 1U) << 6U) |
                               (uint32)bau;



                 /** - CAN1 Port output values */
                canREG1->TIOC =  (uint32)((uint32)1U  << 18U )
                               | (uint32)((uint32)0U  << 17U )
                               | (uint32)((uint32)0U  << 16U )
                               | (uint32)((uint32)1U  << 3U )
                               | (uint32)((uint32)1U  << 2U )
                               | (uint32)((uint32)0U << 1U );

                canREG1->RIOC =  (uint32)((uint32)1U  << 18U )
                               | (uint32)((uint32)0U  << 17U )
                               | (uint32)((uint32)0U  << 16U )
                               | (uint32)((uint32)1U  << 3U )
                               | (uint32)((uint32)0U  << 2U )
                               | (uint32)((uint32)0U <<1U );




                /** - Leave configuration and initialization mode  */
                canREG1->CTL &= ~(uint32)(0x00000041U);
                break;
        case CAN2:

                /** @b Initialize @b CAN2: */

                /** - Setup control register
                *     - Disable automatic wakeup on bus activity
                *     - Local power down mode disabled
                *     - Disable DMA request lines
                *     - Enable global Interrupt Line 0 and 1
                *     - Disable debug mode
                *     - Release from software reset
                *     - Enable/Disable parity or ECC
                *     - Enable/Disable auto bus on timer
                *     - Setup message completion before entering debug state
                *     - Setup normal operation mode
                *     - Request write access to the configuration registers
                *     - Setup automatic retransmission of messages
                *     - Disable error interrupts
                *     - Disable status interrupts
                *     - Enter initialization mode
                */
                canREG2->CTL = (uint32)0x00000200U
                             | (uint32)0x00000000U
                             | (uint32)((uint32)0x00000005U << 10U)
                             | 0x00020043U;

                /** - Clear all pending error flags and reset current status */
                canREG2->ES |= 0xFFFFFFFFU;

                /** - Assign interrupt level for messages */
                canREG2->INTMUXx[0U] = (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U;

                canREG2->INTMUXx[1U] = (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U;


                /** - Setup auto bus on timer period */
                canREG2->ABOTR = (uint32)0U;


                /** - Initialize message 1
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */


                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)1U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 1U;

                /** - Initialize message 2
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)2U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 2U;

                /** - Initialize message 3
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)3U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 3U;

                /** - Initialize message 4
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)4U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 4U;

                /** - Initialize message 5
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)5U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 5U;

                /** - Initialize message 6
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)6U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 6U;

                /** - Initialize message 7
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)7U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 7U;

                /** - Initialize message 8
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)8U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 8U;

                /** - Initialize message 9
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)9U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 9U;

                /** - Initialize message 10
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)10U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 10U;

                /** - Initialize message 11
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)11U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 11U;

                /** - Initialize message 12
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)12U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 12U;

                /** - Initialize message 13
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)13U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 13U;

                /** - Initialize message 14
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)14U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 14U;

                /** - Initialize message 15
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)15U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 15U;

                /** - Initialize message 16
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)16U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 16U;

                /** - Initialize message 17
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)17U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 17U;

                /** - Initialize message 18
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)18U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 18U;

                /** - Initialize message 19
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)19U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 19U;

                /** - Initialize message 20
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)20U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 20U;

                /** - Initialize message 21
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)21U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 21U;

                /** - Initialize message 22
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)22U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 22U;

                /** - Initialize message 23
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)23U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 23U;

                /** - Initialize message 24
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)24U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 24U;

                /** - Initialize message 25
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)25U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 25U;

                /** - Initialize message 26
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)26U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 26U;

                /** - Initialize message 27
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)27U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 27U;

                /** - Initialize message 28
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)28U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 28U;

                /** - Initialize message 29
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)29U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 29U;

                /** - Initialize message 30
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)30U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 30U;

                /** - Initialize message 31
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)31U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 31U;

                /** - Initialize message 32
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)32U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 32U;

                /** - Initialize message 33
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)33U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 33U;

                /** - Initialize message 34
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)34U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 34U;

                /** - Initialize message 35
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)35U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 35U;

                /** - Initialize message 36
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)36U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 36U;

                /** - Initialize message 37
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)37U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 37U;

                /** - Initialize message 38
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)38U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 38U;

                /** - Initialize message 39
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)39U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 39U;

                /** - Initialize message 40
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)40U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 40U;

                /** - Initialize message 41
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)41U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 41U;

                /** - Initialize message 42
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)42U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 42U;

                /** - Initialize message 43
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)43U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 43U;

                /** - Initialize message 44
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)44U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 44U;

                /** - Initialize message 45
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)45U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 45U;

                /** - Initialize message 46
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)46U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 46U;

                /** - Initialize message 47
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)47U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 47U;

                /** - Initialize message 48
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)48U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 48U;

                /** - Initialize message 49
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)49U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 49U;

                /** - Initialize message 50
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)50U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 50U;

                /** - Initialize message 51
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)51U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 51U;

                /** - Initialize message 52
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)52U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 52U;

                /** - Initialize message 53
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)53U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 53U;

                /** - Initialize message 54
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)54U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 54U;

                /** - Initialize message 55
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)55U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 55U;

                /** - Initialize message 56
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)56U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 56U;

                /** - Initialize message 57
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)57U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 57U;

                /** - Initialize message 58
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)58U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 58U;

                /** - Initialize message 59
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)59U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 59U;

                /** - Initialize message 60
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)60U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 60U;

                /** - Initialize message 61
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)61U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 61U;

                /** - Initialize message 62
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)62U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 62U;

                /** - Initialize message 63
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)63U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF1CMD  = (uint8) 0xF8U;
                canREG2->IF1NO   = 63U;

                /** - Initialize message 64
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG2->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x00000000U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)64U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG2->IF2MCTL = 0x00001000U | (uint32)0x00000400U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG2->IF2CMD  = (uint8) 0xF8U;
                canREG2->IF2NO   = 64U;

                /** - Setup IF1 for data transmission
                *     - Wait until IF1 is ready for use
                *     - Set IF1 control byte
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */
                canREG2->IF1CMD  = 0x87U;

                /** - Setup IF2 for reading data
                *     - Wait until IF1 is ready for use
                *     - Set IF1 control byte
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG2->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */
                canREG2->IF2CMD = 0x17U;

                /** - Setup bit timing
                *     - Setup baud rate prescaler extension
                *     - Setup TSeg2
                *     - Setup TSeg1
                *     - Setup sample jump width
                *     - Setup baud rate prescaler
                */
                canREG2->BTR = (uint32)((uint32)0U << 16U) |
                               (uint32)((uint32)(4U - 1U) << 12U) |
                               (uint32)((uint32)((3U + 4U) - 1U) << 8U) |
                               (uint32)((uint32)(4U - 1U) << 6U) |
                               (uint32)bau;


               /** - CAN2 Port output values */
                canREG2->TIOC =  (uint32)((uint32)1U  << 18U )
                               | (uint32)((uint32)0U  << 17U )
                               | (uint32)((uint32)0U  << 16U )
                               | (uint32)((uint32)1U  << 3U )
                               | (uint32)((uint32)1U  << 2U )
                               | (uint32)((uint32)0U << 1U );

                canREG2->RIOC =  (uint32)((uint32)1U  << 18U )
                               | (uint32)((uint32)0U  << 17U )
                               | (uint32)((uint32)0U  << 16U )
                               | (uint32)((uint32)1U  << 3U )
                               | (uint32)((uint32)0U  << 2U )
                               | (uint32)((uint32)0U <<1U );

                /** - Leave configuration and initialization mode  */
                canREG2->CTL &= ~(uint32)(0x00000041U);
                break;
        case CAN3:
                /** @b Initialize @b CAN3: */

                /** - Setup control register
                *     - Disable automatic wakeup on bus activity
                *     - Local power down mode disabled
                *     - Disable DMA request lines
                *     - Enable global Interrupt Line 0 and 1
                *     - Disable debug mode
                *     - Release from software reset
                *     - Enable/Disable parity or ECC
                *     - Enable/Disable auto bus on timer
                *     - Setup message completion before entering debug state
                *     - Setup normal operation mode
                *     - Request write access to the configuration registers
                *     - Setup automatic retransmission of messages
                *     - Disable error interrupts
                *     - Disable status interrupts
                *     - Enter initialization mode
                */
                canREG3->CTL = (uint32)0x00000000U
                             | (uint32)0x00000020U
                             | (uint32)((uint32)0x00000005U << 10U)
                             | 0x00020043U;

                /** - Clear all pending error flags and reset current status */
                canREG3->ES |= 0xFFFFFFFFU;

                /** - Assign interrupt level for messages */
                canREG3->INTMUXx[0U] = (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U;
                canREG3->INTMUXx[1U] = (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U;

                /** - Setup auto bus on timer period */
                canREG3->ABOTR = (uint32)0U;

                /** - Initialize message 1
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */


                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)1U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 1U;

                /** - Initialize message 2
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)2U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 2U;

                /** - Initialize message 3
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)3U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 3U;

                /** - Initialize message 4
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)4U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 4U;

                /** - Initialize message 5
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)5U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 5U;

                /** - Initialize message 6
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)6U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 6U;

                /** - Initialize message 7
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)7U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 7U;

                /** - Initialize message 8
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)8U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 8U;

                /** - Initialize message 9
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)9U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 9U;

                /** - Initialize message 10
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)10U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 10U;

                /** - Initialize message 11
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)11U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 11U;

                /** - Initialize message 12
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)12U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 12U;

                /** - Initialize message 13
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)13U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 13U;

                /** - Initialize message 14
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)14U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 14U;

                /** - Initialize message 15
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)15U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 15U;

                /** - Initialize message 16
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)16U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 16U;

                /** - Initialize message 17
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)17U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 17U;

                /** - Initialize message 18
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)18U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 18U;

                /** - Initialize message 19
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)19U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 19U;

                /** - Initialize message 20
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)20U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 20U;

                /** - Initialize message 21
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)21U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 21U;

                /** - Initialize message 22
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)22U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 22U;

                /** - Initialize message 23
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)23U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 23U;

                /** - Initialize message 24
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)24U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 24U;

                /** - Initialize message 25
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)25U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 25U;

                /** - Initialize message 26
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)26U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 26U;

                /** - Initialize message 27
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)27U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 27U;

                /** - Initialize message 28
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)28U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 28U;

                /** - Initialize message 29
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)29U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 29U;

                /** - Initialize message 30
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)30U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 30U;

                /** - Initialize message 31
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)31U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 31U;

                /** - Initialize message 32
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)32U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 32U;

                /** - Initialize message 33
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)33U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 33U;

                /** - Initialize message 34
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)34U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 34U;

                /** - Initialize message 35
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)35U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 35U;

                /** - Initialize message 36
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)36U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 36U;

                /** - Initialize message 37
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)37U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 37U;

                /** - Initialize message 38
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)38U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 38U;

                /** - Initialize message 39
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)39U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 39U;

                /** - Initialize message 40
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)40U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 40U;

                /** - Initialize message 41
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)41U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 41U;

                /** - Initialize message 42
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)42U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 42U;

                /** - Initialize message 43
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)43U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 43U;

                /** - Initialize message 44
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)44U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 44U;

                /** - Initialize message 45
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)45U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 45U;

                /** - Initialize message 46
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)46U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 46U;

                /** - Initialize message 47
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)47U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 47U;

                /** - Initialize message 48
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)48U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 48U;

                /** - Initialize message 49
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)49U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 49U;

                /** - Initialize message 50
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)50U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 50U;

                /** - Initialize message 51
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)51U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 51U;

                /** - Initialize message 52
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)52U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 52U;

                /** - Initialize message 53
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)53U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 53U;

                /** - Initialize message 54
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)54U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 54U;

                /** - Initialize message 55
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)55U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 55U;

                /** - Initialize message 56
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)56U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 56U;

                /** - Initialize message 57
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)57U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 57U;

                /** - Initialize message 58
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)58U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 58U;

                /** - Initialize message 59
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)59U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 59U;

                /** - Initialize message 60
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)60U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 60U;

                /** - Initialize message 61
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)61U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 61U;

                /** - Initialize message 62
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)62U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 62U;

                /** - Initialize message 63
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)63U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF1CMD  = (uint8) 0xF8U;
                canREG3->IF1NO   = 63U;

                /** - Initialize message 64
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG3->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x00000000U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x00000000U | (uint32)((uint32)((uint32)64U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG3->IF2MCTL = 0x00001000U | (uint32)0x00000400U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG3->IF2CMD  = (uint8) 0xF8U;
                canREG3->IF2NO   = 64U;

                /** - Setup IF1 for data transmission
                *     - Wait until IF1 is ready for use
                *     - Set IF1 control byte
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */
                canREG3->IF1CMD  = 0x87U;

                /** - Setup IF2 for reading data
                *     - Wait until IF1 is ready for use
                *     - Set IF1 control byte
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG3->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */
                canREG3->IF2CMD = 0x17U;

                /** - Setup bit timing
                *     - Setup baud rate prescaler extension
                *     - Setup TSeg2
                *     - Setup TSeg1
                *     - Setup sample jump width
                *     - Setup baud rate prescaler
                */
                canREG3->BTR = (uint32)((uint32)0U << 16U) |
                               (uint32)((uint32)(4U - 1U) << 12U) |
                               (uint32)((uint32)((3U + 4U) - 1U) << 8U) |
                               (uint32)((uint32)(4U - 1U) << 6U) |
                               (uint32)(uint32)bau;


                /** - CAN3 Port output values */
                canREG3->TIOC =  (uint32)((uint32)1U  << 18U )
                               | (uint32)((uint32)0U  << 17U )
                               | (uint32)((uint32)0U  << 16U )
                               | (uint32)((uint32)1U  << 3U )
                               | (uint32)((uint32)1U  << 2U )
                               | (uint32)((uint32)0U << 1U );

                canREG3->RIOC =  (uint32)((uint32)1U  << 18U )
                               | (uint32)((uint32)0U  << 17U )
                               | (uint32)((uint32)0U  << 16U )
                               | (uint32)((uint32)1U  << 3U )
                               | (uint32)((uint32)0U  << 2U )
                               | (uint32)((uint32)0U << 1U );

                /** - Leave configuration and initialization mode  */
                canREG3->CTL &= ~(uint32)(0x00000041U);
                break;
        case CAN4:
                /** @b Initialize @b CAN4: */

                /** - Setup control register
                *     - Disable automatic wakeup on bus activity
                *     - Local power down mode disabled
                *     - Disable DMA request lines
                *     - Enable global Interrupt Line 0 and 1
                *     - Disable debug mode
                *     - Release from software reset
                *     - Enable/Disable parity or ECC
                *     - Enable/Disable auto bus on timer
                *     - Setup message completion before entering debug state
                *     - Setup normal operation mode
                *     - Request write access to the configuration registers
                *     - Setup automatic retransmission of messages
                *     - Disable error interrupts
                *     - Disable status interrupts
                *     - Enter initialization mode
                */
                canREG4->CTL = (uint32)0x00000200U
                             | (uint32)0x00000000U
                             | ((uint32)0x00000005U  << 10U)
                             | (uint32)0x00020043U;

                /** - Clear all pending error flags and reset current status */
                canREG4->ES |= 0xFFFFFFFFU;

                /** - Assign interrupt level for messages */
                canREG4->INTMUXx[0U] = (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U;

                canREG4->INTMUXx[1U] = (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U
                                     | (uint32)0x00000000U;

                /** - Setup auto bus on timer period */
                canREG4->ABOTR = (uint32)0U;

                /** - Initialize message 1
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */


                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)1U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 1U;

                /** - Initialize message 2
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)2U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 2U;

                /** - Initialize message 3
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)3U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 3U;

                /** - Initialize message 4
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)4U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 4U;

                /** - Initialize message 5
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)5U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 5U;

                /** - Initialize message 6
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)6U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 6U;

                /** - Initialize message 7
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)7U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 7U;

                /** - Initialize message 8
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)8U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 8U;

                /** - Initialize message 9
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)9U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 9U;

                /** - Initialize message 10
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)10U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 10U;

                /** - Initialize message 11
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)11U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 11U;

                /** - Initialize message 12
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)12U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 12U;

                /** - Initialize message 13
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)13U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 13U;

                /** - Initialize message 14
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)14U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 14U;

                /** - Initialize message 15
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)15U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 15U;

                /** - Initialize message 16
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)16U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 16U;

                /** - Initialize message 17
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)17U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 17U;

                /** - Initialize message 18
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)18U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 18U;

                /** - Initialize message 19
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)19U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 19U;

                /** - Initialize message 20
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)20U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 20U;

                /** - Initialize message 21
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)21U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 21U;

                /** - Initialize message 22
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)22U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 22U;

                /** - Initialize message 23
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)23U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 23U;

                /** - Initialize message 24
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)24U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 24U;

                /** - Initialize message 25
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)25U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 25U;

                /** - Initialize message 26
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)26U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 26U;

                /** - Initialize message 27
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)27U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 27U;

                /** - Initialize message 28
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)28U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 28U;

                /** - Initialize message 29
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)29U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 29U;

                /** - Initialize message 30
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)30U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 30U;

                /** - Initialize message 31
                *     - Wait until IF1 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF1 control byte
                *     - Set IF1 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF1MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)31U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF1MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF1CMD  = (uint8) 0xF8U;
                canREG4->IF1NO   = 31U;

                /** - Initialize message 32
                *     - Wait until IF2 is ready for use
                *     - Set message mask
                *     - Set message control word
                *     - Set message arbitration
                *     - Set IF2 control byte
                *     - Set IF2 message number
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */

                canREG4->IF2MSK  = 0xC0000000U | (uint32)((uint32)((uint32)0x000007FFU & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2ARB  = (uint32)0x80000000U | (uint32)0x40000000U | (uint32)0x20000000U | (uint32)((uint32)((uint32)32U & (uint32)0x1FFFFFFFU) << (uint32)0U);
                canREG4->IF2MCTL = 0x00001000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)0x00000000U | (uint32)8U;
                canREG4->IF2CMD  = (uint8) 0xF8U;
                canREG4->IF2NO   = 32U;

                /** - Setup IF1 for data transmission
                *     - Wait until IF1 is ready for use
                *     - Set IF1 control byte
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF1STAT & 0x80U) ==0x80U)
                {
                } /* Wait */
                canREG4->IF1CMD  = 0x87U;

                /** - Setup IF2 for reading data
                *     - Wait until IF1 is ready for use
                *     - Set IF1 control byte
                */
                /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
                while ((canREG4->IF2STAT & 0x80U) ==0x80U)
                {
                } /* Wait */
                canREG4->IF2CMD = 0x17U;

                /** - Setup bit timing
                *     - Setup baud rate prescaler extension
                *     - Setup TSeg2
                *     - Setup TSeg1
                *     - Setup sample jump width
                *     - Setup baud rate prescaler
                */
                canREG4->BTR = ((uint32)0U << 16U) |
                               (((uint32)4U - 1U) << 12U) |
                               ((((uint32)3U + (uint32)4U) - 1U) << 8U) |
                               (((uint32)4U - 1U) << 6U) |
                               (uint32)bau;




                /** - CAN4 Port output values */
                canREG4->TIOC =  (uint32)((uint32)1U  << 18U )
                               | (uint32)((uint32)0U  << 17U )
                               | (uint32)((uint32)0U  << 16U )
                               | (uint32)((uint32)1U  << 3U )
                               | (uint32)((uint32)1U  << 2U )
                               | (uint32)((uint32)0U << 1U );

                canREG4->RIOC =  (uint32)((uint32)1U  << 18U )
                               | (uint32)((uint32)0U  << 17U )
                               | (uint32)((uint32)0U  << 16U )
                               | (uint32)((uint32)1U  << 3U )
                               | (uint32)((uint32)0U  << 2U )
                               | (uint32)((uint32)0U << 1U );
                /** - Leave configuration and initialization mode  */
                canREG4->CTL &= ~(uint32)(0x00000041U);
        break;
    }
}

/*******************************************************************************
Function:       // mibspidma_Init
Description:    // Initialization of MIBSPI DMA communication module
Input:          // none
Output:         // none
*******************************************************************************/
void CAN_DMA_init(CAN_Info canx)
{

    switch(canx.ch)
    {
        case CAN1:
            dmaReqAssign(DMA_CAN1_TRANSMIT_channel, DMA_CAN1_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_CAN1_TX.SADD      = (uint32)(CAN1_TX_DATA) ;
            g_dmaCTRLPKT_CAN1_TX.DADD      = ((uint32_t)((canREG1->IF1DATx)));
            g_dmaCTRLPKT_CAN1_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_CAN1_TX.FRCNT = 1;
            g_dmaCTRLPKT_CAN1_TX.ELCNT = 1;
            g_dmaCTRLPKT_CAN1_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_CAN1_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_CAN1_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_CAN1_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_CAN1_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_CAN1_TX.RDSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN1_TX.WRSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN1_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_CAN1_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_CAN1_TX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_CAN1_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_CAN1_TRANSMIT_channel,g_dmaCTRLPKT_CAN1_TX);


            dmaReqAssign(DMA_CAN1_REVICE_channel, DMA_CAN1_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_CAN1_RX.SADD      =((uint32_t)(&(canREG1->IF3ARB)))  ;
            g_dmaCTRLPKT_CAN1_RX.DADD      =(uint32)(CAN1_RX_DATA) ;
            g_dmaCTRLPKT_CAN1_RX.CHCTRL    = 1;
            g_dmaCTRLPKT_CAN1_RX.FRCNT = 1;
            g_dmaCTRLPKT_CAN1_RX.ELCNT = 2;
            g_dmaCTRLPKT_CAN1_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_CAN1_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_CAN1_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_CAN1_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_CAN1_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_CAN1_RX.RDSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN1_RX.WRSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN1_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_CAN1_RX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_CAN1_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_CAN1_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_CAN1_REVICE_channel,g_dmaCTRLPKT_CAN1_RX);

            dmaSetChEnable(DMA_CAN1_REVICE_channel, DMA_HW);

            canREG1->IF3OBS=0x18;
            canREG1->IF3UEy[0]=0x00000002;

        break;
        case CAN2:
            dmaReqAssign(DMA_CAN2_TRANSMIT_channel, DMA_CAN2_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_CAN2_TX.SADD      = (uint32)(CAN2_TX_DATA) ;
            g_dmaCTRLPKT_CAN2_TX.DADD      = ((uint32_t)(&(canREG2->IF1DATx)));
            g_dmaCTRLPKT_CAN2_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_CAN2_TX.FRCNT = 1;
            g_dmaCTRLPKT_CAN2_TX.ELCNT = 1;
            g_dmaCTRLPKT_CAN2_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_CAN2_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_CAN2_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_CAN2_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_CAN2_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_CAN2_TX.RDSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN2_TX.WRSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN2_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_CAN2_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_CAN2_TX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_CAN2_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_CAN2_TRANSMIT_channel,g_dmaCTRLPKT_CAN2_TX);


            dmaReqAssign(DMA_CAN2_REVICE_channel, DMA_CAN2_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_CAN2_RX.SADD      =((uint32_t)(&(canREG2->IF3ARB)))  ;
            g_dmaCTRLPKT_CAN2_RX.DADD      =(uint32)(CAN2_RX_DATA) ;
            g_dmaCTRLPKT_CAN2_RX.CHCTRL    = 1;
            g_dmaCTRLPKT_CAN2_RX.FRCNT = 1;
            g_dmaCTRLPKT_CAN2_RX.ELCNT = 2;
            g_dmaCTRLPKT_CAN2_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_CAN2_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_CAN2_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_CAN2_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_CAN2_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_CAN2_RX.RDSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN2_RX.WRSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN2_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_CAN2_RX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_CAN2_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_CAN2_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_CAN2_REVICE_channel,g_dmaCTRLPKT_CAN2_RX);

            dmaSetChEnable(DMA_CAN2_REVICE_channel, DMA_HW);

            canREG2->IF3OBS=0x18;
            canREG2->IF3UEy[0]=0x00000002;


        case CAN3:
            dmaReqAssign(DMA_CAN3_TRANSMIT_channel, DMA_CAN3_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_CAN3_TX.SADD      = (uint32)(CAN3_TX_DATA) ;
            g_dmaCTRLPKT_CAN3_TX.DADD      = ((uint32_t)(&(canREG3->IF1DATx)));
            g_dmaCTRLPKT_CAN3_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_CAN3_TX.FRCNT = 1;
            g_dmaCTRLPKT_CAN3_TX.ELCNT = 1;
            g_dmaCTRLPKT_CAN3_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_CAN3_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_CAN3_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_CAN3_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_CAN3_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_CAN3_TX.RDSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN3_TX.WRSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN3_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_CAN3_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_CAN3_TX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_CAN3_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_CAN3_TRANSMIT_channel,g_dmaCTRLPKT_CAN3_TX);


            dmaReqAssign(DMA_CAN3_REVICE_channel, DMA_CAN3_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_CAN3_RX.SADD      =((uint32_t)(&(canREG3->IF3ARB)))  ;
            g_dmaCTRLPKT_CAN3_RX.DADD      =(uint32)(CAN3_RX_DATA) ;
            g_dmaCTRLPKT_CAN3_RX.CHCTRL    = 1;
            g_dmaCTRLPKT_CAN3_RX.FRCNT = 1;
            g_dmaCTRLPKT_CAN3_RX.ELCNT = 2;
            g_dmaCTRLPKT_CAN3_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_CAN3_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_CAN3_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_CAN3_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_CAN3_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_CAN3_RX.RDSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN3_RX.WRSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN3_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_CAN3_RX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_CAN3_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_CAN3_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_CAN3_REVICE_channel,g_dmaCTRLPKT_CAN3_RX);

            dmaSetChEnable(DMA_CAN3_REVICE_channel, DMA_HW);

            canREG3->IF3OBS=0x18;
            canREG3->IF3UEy[0]=0x00000002;


        break;
        case CAN4:
            dmaReqAssign(DMA_CAN4_TRANSMIT_channel, DMA_CAN4_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_CAN4_TX.SADD      = (uint32)(CAN4_TX_DATA) ;
            g_dmaCTRLPKT_CAN4_TX.DADD      = ((uint32_t)(&(canREG4->IF1DATx)));
            g_dmaCTRLPKT_CAN4_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_CAN4_TX.FRCNT = 1;
            g_dmaCTRLPKT_CAN4_TX.ELCNT = 1;
            g_dmaCTRLPKT_CAN4_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_CAN4_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_CAN4_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_CAN4_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_CAN4_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_CAN4_TX.RDSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN4_TX.WRSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN4_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_CAN4_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_CAN4_TX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_CAN4_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_CAN4_TRANSMIT_channel,g_dmaCTRLPKT_CAN4_TX);


            dmaReqAssign(DMA_CAN4_REVICE_channel, DMA_CAN4_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_CAN4_RX.SADD      =((uint32_t)(&(canREG4->IF3ARB)))  ;
            g_dmaCTRLPKT_CAN4_RX.DADD      =(uint32)(CAN4_RX_DATA) ;
            g_dmaCTRLPKT_CAN4_RX.CHCTRL    = 1;
            g_dmaCTRLPKT_CAN4_RX.FRCNT = 1;
            g_dmaCTRLPKT_CAN4_RX.ELCNT = 2;
            g_dmaCTRLPKT_CAN4_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_CAN4_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_CAN4_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_CAN4_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_CAN4_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_CAN4_RX.RDSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN4_RX.WRSIZE    = ACCESS_64_BIT;
            g_dmaCTRLPKT_CAN4_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_CAN4_RX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_CAN4_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_CAN4_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_CAN4_REVICE_channel,g_dmaCTRLPKT_CAN4_RX);

            dmaSetChEnable(DMA_CAN4_REVICE_channel, DMA_HW);

            canREG4->IF3OBS=0x18;
            canREG4->IF3UEy[0]=0x00000002;
        break;
        default:
        break;

    }
}


void CAN_Tx(CAN_Info canx,uint32 ID,uint8* buff,uint32 len)
{
    uint8 i=0;
    uint32_t ret = 0U;
    uint8_t trytimes = 3U;
    uint32 canMcanMESSAGE_BOX=canMESSAGE_BOX1;
    switch(canx.ch)
    {
    case CAN1:
        if( DMA_CAN1_TX==true)
        {
            for(i=0;i<len;i++)
            {
                CAN1_TX_DATA[s_canByteOrder[i]]=buff[i];
            }
            canREG1->IF1CMD = 0x87U;
            canREG1->IF1STAT  =0x40;
            dmaSetChEnable(DMA_CAN1_TRANSMIT_channel, DMA_HW);

        }
        else
        {
           canMcanMESSAGE_BOX=Get_CanMcanMESSAGE_BOX_Num(canx,ID);
           do {
                  ret = canTransmit(canREG1, canMcanMESSAGE_BOX, buff);
                  if (ret == 0U) {
                      int16_t i = 1000U;
                      while (i > 0) i--;
                  }
                  trytimes--;
              } while ((ret == 0U) && (trytimes > 0U));
        }
    break;
    case CAN2:
        if( DMA_CAN1_TX==true)
        {
            for(i=0;i<len;i++)
            {
                CAN2_TX_DATA[s_canByteOrder[i]]=buff[i];
            }
            dmaSetChEnable(DMA_CAN2_TRANSMIT_channel, DMA_HW);
        }
        else
        {

            canMcanMESSAGE_BOX=Get_CanMcanMESSAGE_BOX_Num(canx,ID);
            do {
                   ret = canTransmit(canREG2, canMcanMESSAGE_BOX, buff);
                   if (ret == 0U) {
                       int16_t i = 1000U;
                       while (i > 0) i--;
                   }
                   trytimes--;
               } while ((ret == 0U) && (trytimes > 0U));
        }
        break;
    case CAN3:
        if( DMA_CAN3_TX==true)
        {
            for(i=0;i<len;i++)
            {
                CAN3_TX_DATA[s_canByteOrder[i]]=buff[i];
            }
            dmaSetChEnable(DMA_CAN3_TRANSMIT_channel, DMA_HW);
        }
        else
        {
            canMcanMESSAGE_BOX=Get_CanMcanMESSAGE_BOX_Num(canx,ID);
            do {
                   ret = canTransmit(canREG3, canMcanMESSAGE_BOX, buff);
                   if (ret == 0U) {
                       int16_t i = 1000U;
                       while (i > 0) i--;
                   }
                   trytimes--;
               } while ((ret == 0U) && (trytimes > 0U));
        }
        break;
    case CAN4:
        if( DMA_CAN4_TX==true)
        {
            for(i=0;i<len;i++)
            {
                CAN4_TX_DATA[s_canByteOrder[i]]=buff[i];
            }
            dmaSetChEnable(DMA_CAN4_TRANSMIT_channel, DMA_HW);
        }
        else
        {
            canMcanMESSAGE_BOX=Get_CanMcanMESSAGE_BOX_Num(canx,ID);
            do {
                   ret = canTransmit(canREG4, canMcanMESSAGE_BOX, buff);
                   if (ret == 0U) {
                       int16_t i = 1000U;
                       while (i > 0) i--;
                   }
                   trytimes--;
               } while ((ret == 0U) && (trytimes > 0U));
        }
        break;
    default:
        break;
    }
}

uint32 Get_CanMcanMESSAGE_BOX_Num(CAN_Info canx,uint32 ID)
{
    uint8 i=0;
    uint32 box=1;
    switch(canx.ch)
    {
        case CAN1:
            for(i=0;i<CAN_MCANMESSAGE_BOX_NUM;i++)
            {
                if(CAN1_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    box=CAN1_McanMESSAGE_BOX_TABLE[i].BOX_num;
                    break;
                }
            }
        break;
        case CAN2:
            for(i=0;i<CAN_MCANMESSAGE_BOX_NUM;i++)
            {
                if(CAN2_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    box=CAN2_McanMESSAGE_BOX_TABLE[i].BOX_num;
                    break;
                }
            }
        break;
        case CAN3:
            for(i=0;i<CAN_MCANMESSAGE_BOX_NUM;i++)
            {
                if(CAN3_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    box=CAN3_McanMESSAGE_BOX_TABLE[i].BOX_num;
                    break;
                }
            }
        break;
        case CAN4:
            for(i=0;i<CAN_MCANMESSAGE_BOX_NUM;i++)
            {
                if(CAN4_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    box=CAN4_McanMESSAGE_BOX_TABLE[i].BOX_num;
                    break;
                }
            }
        break;
        default:
        break;

    }
    return box;

}

void CAN_Tx_bind(CAN_Info canx, uint32 ID,uint8 messagebox)
{
    uint8 i=0;
    switch(canx.ch)
    {
        case CAN1:
            for(i=0;i<CAN_MCANMESSAGE_BOX_NUM;i++)
            {
                if(CAN1_McanMESSAGE_BOX_TABLE[i].ID!=ID && CAN1_McanMESSAGE_BOX_TABLE[i].ID==0)
                {
                    CAN1_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    CAN1_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    canUpdateID(canREG1,messagebox,ID);
                    return;
                }
                else if(CAN1_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    CAN1_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    CAN1_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    canUpdateID(canREG1,messagebox,ID);
                    return;
                }
            }
            break;
        case CAN2:
            for(i=0;i<CAN_MCANMESSAGE_BOX_NUM;i++)
            {
                if(CAN2_McanMESSAGE_BOX_TABLE[i].ID!=ID && CAN2_McanMESSAGE_BOX_TABLE[i].ID==0)
                {
                    CAN2_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    CAN2_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    canUpdateID(canREG2,messagebox,ID);
                    return;
                }
                else if(CAN2_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    CAN2_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    CAN2_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    canUpdateID(canREG2,messagebox,ID);
                    return;
                }
            }
            break;
        case CAN3:
            for(i=0;i<CAN_MCANMESSAGE_BOX_NUM;i++)
            {
                if(CAN3_McanMESSAGE_BOX_TABLE[i].ID!=ID && CAN3_McanMESSAGE_BOX_TABLE[i].ID==0)
                {
                    CAN3_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    CAN3_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    canUpdateID(canREG3,messagebox,ID);
                    return;
                }
                else if(CAN3_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    CAN3_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    CAN3_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    canUpdateID(canREG3,messagebox,ID);
                    return;
                }
            }
            break;
        case CAN4:
            for(i=0;i<CAN_MCANMESSAGE_BOX_NUM;i++)
            {
                if(CAN4_McanMESSAGE_BOX_TABLE[i].ID!=ID && CAN4_McanMESSAGE_BOX_TABLE[i].ID==0)
                {
                    CAN4_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    CAN4_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    canUpdateID(canREG4,messagebox,ID);
                    return;
                }
                else if(CAN4_McanMESSAGE_BOX_TABLE[i].ID==ID)
                {
                    CAN4_McanMESSAGE_BOX_TABLE[i].ID=ID;
                    CAN4_McanMESSAGE_BOX_TABLE[i].BOX_num=messagebox;
                    canUpdateID(canREG4,messagebox,ID);
                    return;
                }
            }
            break;
        default:
            break;

    }
}


void CAN1_revive(uint32 canrevID,uint8 * revice_data)
{

      /*The queue is full, and the data at the head of the column is discarded. */
       memcpy(CAN1_MSG_buff.revicebuf[CAN1_MSG_buff.tail].data, revice_data, sizeof(CAN1_MSG_buff.revicebuf[CAN1_MSG_buff.tail].data));
       CAN1_MSG_buff.revicebuf[CAN1_MSG_buff.tail].ID=canrevID;
       CAN1_MSG_buff.tail = (CAN1_MSG_buff.tail + 1) % CAN_MSG_Revice_NUM;
}
void CAN2_revive(uint32 canrevID,uint8 * revice_data)
{
      /*The queue is full, and the data at the head of the column is discarded. */
       memcpy(CAN2_MSG_buff.revicebuf[CAN2_MSG_buff.tail].data, revice_data, sizeof(CAN2_MSG_buff.revicebuf[CAN2_MSG_buff.tail].data));
       CAN2_MSG_buff.revicebuf[CAN2_MSG_buff.tail].ID=canrevID;
       CAN2_MSG_buff.tail = (CAN2_MSG_buff.tail + 1) % CAN_MSG_Revice_NUM;
}
void CAN3_revive(uint32 canrevID,uint8 * revice_data)
{
      /*The queue is full, and the data at the head of the column is discarded. */
       memcpy(CAN3_MSG_buff.revicebuf[CAN3_MSG_buff.tail].data, revice_data, sizeof(CAN3_MSG_buff.revicebuf[CAN3_MSG_buff.tail].data));
       CAN3_MSG_buff.revicebuf[CAN3_MSG_buff.tail].ID=canrevID;
       CAN3_MSG_buff.tail = (CAN3_MSG_buff.tail + 1) % CAN_MSG_Revice_NUM;
}
void CAN4_revive(uint32 canrevID,uint8 * revice_data)
{
      /*The queue is full, and the data at the head of the column is discarded. */
       memcpy(CAN4_MSG_buff.revicebuf[CAN4_MSG_buff.tail].data, revice_data, sizeof(CAN4_MSG_buff.revicebuf[CAN4_MSG_buff.tail].data));
       CAN4_MSG_buff.revicebuf[CAN4_MSG_buff.tail].ID=canrevID;
       CAN4_MSG_buff.tail = (CAN4_MSG_buff.tail + 1) % CAN_MSG_Revice_NUM;
}
/*******************************************************************************
Function:       // canMessageNotification
Description:    // CAN interrupt handler
Input:          // node       : hardware CAN module trigger the interrupt.(canREG1,canREG2,canREG3).
                // messageBox : messagebox in CAN module which trigger the interrupt
Output:         // none
*******************************************************************************/
void canMessageNotification(canBASE_t *node, uint32 messageBox)
{
    uint8 revdata[8U];

    if ((node == canREG1) && messageBox == canMESSAGE_BOX64)
    {
        while (!canIsRxMessageArrived(node, canMESSAGE_BOX64));
        canGetData(node, canMESSAGE_BOX64, revdata);
        //dmaSetChEnable(DMA_CAN1_REVICE_CHANNEL,DMA_HW);
        uint32 canrevID = canGetID(node, canMESSAGE_BOX64);
        CAN1_revive(canrevID,revdata);
    }
    else if (node == canREG2 && messageBox == canMESSAGE_BOX64)
    {
        while (!canIsRxMessageArrived(node, canMESSAGE_BOX64));
       canGetData(node, canMESSAGE_BOX64, revdata);
       //dmaSetChEnable(DMA_CAN2_REVICE_CHANNEL,DMA_HW);
       uint32 canrevID = canGetID(node, canMESSAGE_BOX64);
       CAN2_revive(canrevID,revdata);
    }
    else if (node == canREG3 && messageBox == canMESSAGE_BOX64)
    {
        while (!canIsRxMessageArrived(node, canMESSAGE_BOX64));
       canGetData(node, canMESSAGE_BOX64, revdata);
       //dmaSetChEnable(DMA_CAN3_REVICE_CHANNEL,DMA_HW);
       uint32 canrevID = canGetID(node, canMESSAGE_BOX64);
       CAN3_revive(canrevID,revdata);
    }
    else if (node == canREG4 && messageBox == canMESSAGE_BOX64)
    {
        while (!canIsRxMessageArrived(node, canMESSAGE_BOX64));
       canGetData(node, canMESSAGE_BOX64, revdata);
       //dmaSetChEnable(DMA_CAN4_REVICE_CHANNEL,DMA_HW);
       uint32 canrevID = canGetID(node, canMESSAGE_BOX64);
       CAN4_revive(canrevID,revdata);
    }
}
