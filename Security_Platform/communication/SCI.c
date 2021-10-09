/*
 * SCI.c
 *
 *  Created on: 2021Äê7ÔÂ30ÈÕ
 *      Author: 15755
 */


#include"SCI.h"
#include"HL_sci.h"
#include"HL_reg_sci.h"
#include "HL_mibspi.h"
#include "HL_sys_vim.h"
#include "HL_reg_dma.h"
#include "HL_sys_dma.h"
#include "HL_lin.h"
#include "HL_reg_lin.h"
#include "HL_pinmux.h"
#include "HL_reg_pinmux.h"
uint8 SCI1_RX_DATA[SCI1_RX_DATA_LEN]={0};
uint8 SCI2_RX_DATA[SCI2_RX_DATA_LEN]={0};
uint8 SCI3_RX_DATA[SCI3_RX_DATA_LEN]={0};
uint8 SCI4_RX_DATA[SCI4_RX_DATA_LEN]={0};
uint8 SCI1_TX_DATA[SCI1_TX_DATA_LEN]={0};
uint8 SCI2_TX_DATA[SCI2_TX_DATA_LEN]={0};
uint8 SCI3_TX_DATA[SCI3_TX_DATA_LEN]={0};
uint8 SCI4_TX_DATA[SCI4_TX_DATA_LEN]={0};
bool DMA_SCI1_TX=true;
bool DMA_SCI2_TX=false;
bool DMA_SCI3_TX=false;
bool DMA_SCI4_TX=false;
bool DMA_SCI1_RX=false;
bool DMA_SCI2_RX=true;
bool DMA_SCI3_RX=true;
bool DMA_SCI4_RX=true;
SCI_RecvQueue SCI1_MSG_Buff;
SCI_RecvQueue SCI2_MSG_Buff;
SCI_RecvQueue SCI3_MSG_Buff;
SCI_RecvQueue SCI4_MSG_Buff;
static volatile struct g_sciTransfer
{
    uint32   mode;         /* Used to check for TX interrupt Enable */
    uint32   tx_length;    /* Transmit data length in number of Bytes */
    uint32   rx_length;    /* Receive data length in number of Bytes */
    uint8    * tx_data;    /* Transmit data pointer */
    uint8    * rx_data;    /* Receive data pointer */
} g_sciTransfer_t[4U];

void SCI2_IO_Configure(void)
{
    /* Enable Pin Muxing */
           pinMuxReg->KICKER0 = 0x83E70B13U;
           pinMuxReg->KICKER1 = 0x95A4F1E0U;

           pinMuxReg->PINMUX[36]  =pinMuxReg->PINMUX[36] & (0xffff0000) | (0x1 << 1) | (0x1 << 9); /*SCI2TX */

           /* Disable Pin Muxing */
           pinMuxReg->KICKER0 = 0x00000000U;
           pinMuxReg->KICKER1 = 0x00000000U;
}

void SCI3_IO_Configure(void)
{
    /* Enable Pin Muxing */
           pinMuxReg->KICKER0 = 0x83E70B13U;
           pinMuxReg->KICKER1 = 0x95A4F1E0U;

           pinMuxReg->PINMUX[29]  =pinMuxReg->PINMUX[29] & (0xff00ffff) | (0x1 << 17); /*SCI3RX */
           pinMuxReg->PINMUX[31]  =pinMuxReg->PINMUX[31] & (0xffff00ff) | (0x1 << 9); /*SCI3TX */

           /* Disable Pin Muxing */
           pinMuxReg->KICKER0 = 0x00000000U;
           pinMuxReg->KICKER1 = 0x00000000U;
}
void SCI4_IO_Configure(void)
{
    /* Enable Pin Muxing */
           pinMuxReg->KICKER0 = 0x83E70B13U;
           pinMuxReg->KICKER1 = 0x95A4F1E0U;

           pinMuxReg->PINMUX[32]  =pinMuxReg->PINMUX[32] & (0xff00ff00) | (0x1 << 2) | (0x1 << 18); /*SCI4TX */

           /* Disable Pin Muxing */
           pinMuxReg->KICKER0 = 0x00000000U;
           pinMuxReg->KICKER1 = 0x00000000U;
}
void SCI_init(SCI_Info scix)
{
       uint8 parity=0 ;
       uint8 bilen=0 ;
       uint8 stopbits=0 ;
       parity=scix.parity;
       bilen=scix.bitlen;
       stopbits=scix.stopbits;
       float64   vclk = 75.000 * 1000000.0;
       uint32 f    = 16U;
       uint32 temp;
       float64 temp2;

       /*SAFETYMCUSW 96 S MR:6.1 <APPROVED> "Calculations including int and float cannot be avoided" */
        temp = (f*(scix.burt));
        temp2 = ((vclk)/((float64)temp))-1U;
        temp2 =(uint32) ((temp2 + 0.5)/1);        /* Rounding-off to the closest integer */
       switch(scix.ch)
       {
       case SCI1:
               sciREG1->GCR0 = 0U;
               sciREG1->GCR0 = 1U;

               /** - Disable all interrupts */
               sciREG1->CLEARINT    = 0xFFFFFFFFU;
               sciREG1->CLEARINTLVL = 0xFFFFFFFFU;

               /** - global control 1 */
               sciREG1->GCR1 =  (uint32)((uint32)1U << 25U)  /* enable transmit */
                             | (uint32)((uint32)1U << 24U)  /* enable receive */
                             | (uint32)((uint32)1U << 5U)   /* internal clock (device has no clock pin) */
                             | (uint32)((uint32)(stopbits-1U) << 4U)  /* number of stop bits */
                             | (uint32)((uint32)(parity>1? 1:0) << 3U)  /* even parity, otherwise odd */
                             | (uint32)((uint32)(parity>0? 1:0) << 2U)  /* enable parity */
                             | (uint32)((uint32)1U << 1U);  /* asynchronous timing mode */

               /** - set baudrate */
               sciREG1->BRS =(uint32)((uint32)temp2 & 0x00FFFFFFU);  /* baudrate */

               /** - transmission length */
               sciREG1->FORMAT = bilen - 1U;  /* length */

               /** - set SCI1 pins functional mode */
               sciREG1->PIO0 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)1U << 1U); /* rx pin */

               /** - set SCI1 pins default output value */
               sciREG1->PIO3 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI1 pins output direction */
               sciREG1->PIO1 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI1 pins open drain enable */
               sciREG1->PIO6 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI1 pins pullup/pulldown enable */
               sciREG1->PIO7 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI1 pins pullup/pulldown select */
               sciREG1->PIO8 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)1U << 1U);  /* rx pin */


               /** - set interrupt level */
               sciREG1->SETINTLVL = (uint32)((uint32)0U << 26U)  /* Framing error */
                                 | (uint32)((uint32)0U << 25U)  /* Overrun error */
                                 | (uint32)((uint32)0U << 24U)  /* Parity error */
                                 | (uint32)((uint32)0U << 9U)  /* Receive */
                                 | (uint32)((uint32)0U << 8U)  /* Transmit */
                                 | (uint32)((uint32)0U << 1U)  /* Wakeup */
                                 | (uint32)((uint32)0U << 0U);  /* Break detect */

               /** - set interrupt enable */
               sciREG1->SETINT = (uint32)((uint32)0U << 26U)  /* Framing error */
                              | (uint32)((uint32)0U << 25U)  /* Overrun error */
                              | (uint32)((uint32)0U << 24U)  /* Parity error */
                              | (uint32)((uint32)0U << 9U)  /* Receive */
                              | (uint32)((uint32)0U << 1U)  /* Wakeup */
                              | (uint32)((uint32)0U << 0U);  /* Break detect */

               /** - initialize global transfer variables */
               g_sciTransfer_t[0U].mode   = (uint32)0U << 8U;
               g_sciTransfer_t[0U].tx_length = 0U;
               g_sciTransfer_t[0U].rx_length = 0U;

               /** - Finaly start SCI1 */
               sciREG1->GCR1 |= 0x80U;

               break;
       case SCI2:
               /** @b initialize @b SCI2 */

               /** - bring SCI2 out of reset */
               sciREG2->GCR0 = 0U;
               sciREG2->GCR0 = 1U;

               /** - Disable all interrupts */
               sciREG2->CLEARINT    = 0xFFFFFFFFU;
               sciREG2->CLEARINTLVL = 0xFFFFFFFFU;

               /** - global control 1 */
               sciREG2->GCR1 =  (uint32)((uint32)1U << 25U)  /* enable transmit */
                             | (uint32)((uint32)1U << 24U)  /* enable receive */
                             | (uint32)((uint32)1U << 5U)   /* internal clock (device has no clock pin) */
                             | (uint32)((uint32)(stopbits-1U) << 4U)  /* number of stop bits */
                             | (uint32)((uint32)(parity>1? 1:0)<< 3U)  /* even parity, otherwise odd */
                             | (uint32)((uint32)(parity>0? 1:0) << 2U)  /* enable parity */
                             | (uint32)((uint32)1U << 1U);  /* asynchronous timing mode */

               /** - set baudrate */
               sciREG2->BRS = (uint32)((uint32)temp2 & 0x00FFFFFFU);  /* baudrate */

               /** - transmission length */
               sciREG2->FORMAT = bilen - 1U;  /* length */

               /** - set SCI2 pins functional mode */
               sciREG2->PIO0 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)1U << 1U); /* rx pin */

               /** - set SCI2 pins default output value */
               sciREG2->PIO3 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI2 pins output direction */
               sciREG2->PIO1 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI2 pins open drain enable */
               sciREG2->PIO6 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI2 pins pullup/pulldown enable */
               sciREG2->PIO7 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI2 pins pullup/pulldown select */
               sciREG2->PIO8 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)1U << 1U);  /* rx pin */

               /** - set interrupt level */
               sciREG2->SETINTLVL = (uint32)((uint32)0U << 26U)  /* Framing error */
                                 | (uint32)((uint32)0U << 25U)  /* Overrun error */
                                 | (uint32)((uint32)0U << 24U)  /* Parity error */
                                 | (uint32)((uint32)0U << 9U)  /* Receive */
                                 | (uint32)((uint32)0U << 8U)  /* Transmit */
                                 | (uint32)((uint32)0U << 1U)  /* Wakeup */
                                 | (uint32)((uint32)0U << 0U);  /* Break detect */

               /** - set interrupt enable */
               sciREG2->SETINT = (uint32)((uint32)0U << 26U)  /* Framing error */
                              | (uint32)((uint32)0U << 25U)  /* Overrun error */
                              | (uint32)((uint32)0U << 24U)  /* Parity error */
                              | (uint32)((uint32)0U << 9U)  /* Receive */
                              | (uint32)((uint32)0U << 1U)  /* Wakeup */
                              | (uint32)((uint32)0U << 0U);  /* Break detect */

               /** - initialize global transfer variables */
               g_sciTransfer_t[1U].mode   = (uint32)0U << 8U;
               g_sciTransfer_t[1U].tx_length = 0U;
               g_sciTransfer_t[1U].rx_length = 0U;

               /** - Finaly start SCI2 */
               sciREG2->GCR1 |= 0x80U;



               break;
       case SCI3:
               /** @b initialize @b SCI3 */

               /** - bring SCI3 out of reset */
               sciREG3->GCR0 = 0U;
               sciREG3->GCR0 = 1U;

               /** - Disable all interrupts */
               sciREG3->CLEARINT    = 0xFFFFFFFFU;
               sciREG3->CLEARINTLVL = 0xFFFFFFFFU;

               /** - global control 1 */
               sciREG3->GCR1 =  (uint32)((uint32)1U << 25U)  /* enable transmit */
                             | (uint32)((uint32)1U << 24U)  /* enable receive */
                             | (uint32)((uint32)1U << 5U)   /* internal clock (device has no clock pin) */
                             | (uint32)((uint32)(stopbits-1U) << 4U)  /* number of stop bits */
                             | (uint32)((uint32)(parity>1? 1:0) << 3U)  /* even parity, otherwise odd */
                             | (uint32)((uint32)(parity>0? 1:0) << 2U)  /* enable parity */
                             | (uint32)((uint32)1U << 1U);  /* asynchronous timing mode */

               /** - set baudrate */
               sciREG3->BRS = (uint32)((uint32)temp2 & 0x00FFFFFFU);  /* baudrate */

               /** - transmission length */
               sciREG3->FORMAT = bilen - 1U;  /* length */

               /** - set SCI3 pins functional mode */
               sciREG3->PIO0 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)1U << 1U); /* rx pin */

               /** - set SCI3 pins default output value */
               sciREG3->PIO3 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI3 pins output direction */
               sciREG3->PIO1 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI3 pins open drain enable */
               sciREG3->PIO6 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI3 pins pullup/pulldown enable */
               sciREG3->PIO7 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI3 pins pullup/pulldown select */
               sciREG3->PIO8 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)1U << 1U);  /* rx pin */

               /** - set interrupt level */
               sciREG3->SETINTLVL = (uint32)((uint32)0U << 26U)  /* Framing error */
                                 | (uint32)((uint32)0U << 25U)  /* Overrun error */
                                 | (uint32)((uint32)0U << 24U)  /* Parity error */
                                 | (uint32)((uint32)0U << 9U)  /* Receive */
                                 | (uint32)((uint32)0U << 8U)  /* Transmit */
                                 | (uint32)((uint32)0U << 1U)  /* Wakeup */
                                 | (uint32)((uint32)0U << 0U);  /* Break detect */

               /** - set interrupt enable */
               sciREG3->SETINT = (uint32)((uint32)0U << 26U)  /* Framing error */
                              | (uint32)((uint32)0U << 25U)  /* Overrun error */
                              | (uint32)((uint32)0U << 24U)  /* Parity error */
                              | (uint32)((uint32)1U << 9U)  /* Receive */
                              | (uint32)((uint32)0U << 1U)  /* Wakeup */
                              | (uint32)((uint32)0U << 0U);  /* Break detect */

               /** - initialize global transfer variables */
               g_sciTransfer_t[2U].mode   = (uint32)0U << 8U;
               g_sciTransfer_t[2U].tx_length = 0U;
               g_sciTransfer_t[2U].rx_length = 0U;

               /** - Finaly start SCI3 */
               sciREG3->GCR1 |= 0x80U;


               break;
       case SCI4:
               /** @b initialize @b SCI4 */

               /** - bring SCI4 out of reset */
               sciREG4->GCR0 = 0U;
               sciREG4->GCR0 = 1U;

               /** - Disable all interrupts */
               sciREG4->CLEARINT    = 0xFFFFFFFFU;
               sciREG4->CLEARINTLVL = 0xFFFFFFFFU;

               /** - global control 1 */
               sciREG4->GCR1 =  (uint32)((uint32)1U << 25U)  /* enable transmit */
                             | (uint32)((uint32)1U << 24U)  /* enable receive */
                             | (uint32)((uint32)1U << 5U)   /* internal clock (device has no clock pin) */
                             | (uint32)((uint32)(stopbits-1U) << 4U)  /* number of stop bits */
                             | (uint32)((uint32)(parity>1? 1:0) << 3U)  /* even parity, otherwise odd */
                             | (uint32)((uint32)(parity>0? 1:0) << 2U)  /* enable parity */
                             | (uint32)((uint32)1U << 1U);  /* asynchronous timing mode */

               /** - set baudrate */
               sciREG4->BRS = (uint32)((uint32)temp2 & 0x00FFFFFFU);  /* baudrate */

               /** - transmission length */
               sciREG4->FORMAT = bilen - 1U;  /* length */

               /** - set SCI4 pins functional mode */
               sciREG4->PIO0 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)1U << 1U); /* rx pin */

               /** - set SCI4 pins default output value */
               sciREG4->PIO3 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI4 pins output direction */
               sciREG4->PIO1 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI4 pins open drain enable */
               sciREG4->PIO6 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI4 pins pullup/pulldown enable */
               sciREG4->PIO7 = (uint32)((uint32)0U << 2U)  /* tx pin */
                            | (uint32)((uint32)0U << 1U); /* rx pin */

               /** - set SCI4 pins pullup/pulldown select */
               sciREG4->PIO8 = (uint32)((uint32)1U << 2U)  /* tx pin */
                            | (uint32)((uint32)1U << 1U);  /* rx pin */

               /** - set interrupt level */
               sciREG4->SETINTLVL = (uint32)((uint32)0U << 26U)  /* Framing error */
                                 | (uint32)((uint32)0U << 25U)  /* Overrun error */
                                 | (uint32)((uint32)0U << 24U)  /* Parity error */
                                 | (uint32)((uint32)0U << 9U)  /* Receive */
                                 | (uint32)((uint32)0U << 8U)  /* Transmit */
                                 | (uint32)((uint32)0U << 1U)  /* Wakeup */
                                 | (uint32)((uint32)0U << 0U);  /* Break detect */

               /** - set interrupt enable */
               sciREG4->SETINT = (uint32)((uint32)0U << 26U)  /* Framing error */
                              | (uint32)((uint32)0U << 25U)  /* Overrun error */
                              | (uint32)((uint32)0U << 24U)  /* Parity error */
                              | (uint32)((uint32)1U << 9U)  /* Receive */
                              | (uint32)((uint32)0U << 1U)  /* Wakeup */
                              | (uint32)((uint32)0U << 0U);  /* Break detect */

               /** - initialize global transfer variables */
               g_sciTransfer_t[3U].mode   = (uint32)0U << 8U;
               g_sciTransfer_t[3U].tx_length = 0U;
               g_sciTransfer_t[3U].rx_length = 0U;

               /** - Finaly start SCI4 */
               sciREG4->GCR1 |= 0x80U;

       break;
       default:
           break;
       }
}

g_dmaCTRL g_dmaCTRLPKT_SCI1_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SCI1_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SCI2_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SCI2_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SCI3_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SCI3_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SCI4_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SCI4_RX;             /* dma control packet configuration stack */
void SCI_DMA_init(SCI_Info scix)
{
    switch(scix.ch)
    {
        case SCI1:

            while (((sciREG1->FLR & SCI_TX_INT) == 0U) || ((sciREG1->FLR & 0x4) == 0x4))
            {
            }
            dmaReqAssign(DMA_SCI1_TRANSMIT_channel, DMA_SCI1_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_SCI1_TX.SADD      = (uint32)(SCI1_TX_DATA) ;
            g_dmaCTRLPKT_SCI1_TX.DADD      = ((uint32_t)(&(sciREG1->TD))+3);
            g_dmaCTRLPKT_SCI1_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_SCI1_TX.FRCNT = SCI1_TX_DATA_LEN;
            g_dmaCTRLPKT_SCI1_TX.ELCNT = 1;
            g_dmaCTRLPKT_SCI1_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SCI1_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SCI1_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SCI1_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SCI1_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_SCI1_TX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI1_TX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI1_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SCI1_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_SCI1_TX.ADDMODEWR = ADDR_FIXED;
            g_dmaCTRLPKT_SCI1_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_SCI1_TRANSMIT_channel,g_dmaCTRLPKT_SCI1_TX);


            dmaReqAssign(DMA_SCI1_REVICE_channel, DMA_SCI1_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_SCI1_RX.SADD      =((uint32_t)(&(sciREG1->RD))+3)  ;
            g_dmaCTRLPKT_SCI1_RX.DADD      =(uint32)(SCI1_RX_DATA) ;
            g_dmaCTRLPKT_SCI1_RX.CHCTRL    = 1;
            g_dmaCTRLPKT_SCI1_RX.FRCNT = SCI1_RX_DATA_LEN;
            g_dmaCTRLPKT_SCI1_RX.ELCNT = 1;
            g_dmaCTRLPKT_SCI1_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SCI1_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SCI1_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SCI1_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SCI1_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_SCI1_RX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI1_RX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI1_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SCI1_RX.ADDMODERD = ADDR_FIXED;
            g_dmaCTRLPKT_SCI1_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_SCI1_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_SCI1_REVICE_channel,g_dmaCTRLPKT_SCI1_RX);

            dmaSetChEnable(DMA_SCI1_REVICE_channel, DMA_HW);

            sciREG1->SETINT |= SCI_SET_TX_DMA | SCI_SET_RX_DMA | SCI_SET_RX_DMA_ALL;


        break;
        case SCI2:
            while (((sciREG2->FLR & SCI_TX_INT) == 0U) || ((sciREG2->FLR & 0x4) == 0x4))
            {
            }
            dmaReqAssign(DMA_SCI2_TRANSMIT_channel, DMA_SCI2_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_SCI2_TX.SADD      = (uint32)(SCI2_TX_DATA) ;
            g_dmaCTRLPKT_SCI2_TX.DADD      = ((uint32_t)(&(sciREG2->TD))+3);
            g_dmaCTRLPKT_SCI2_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_SCI2_TX.FRCNT = 8;
            g_dmaCTRLPKT_SCI2_TX.ELCNT = 1;
            g_dmaCTRLPKT_SCI2_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SCI2_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SCI2_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SCI2_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SCI2_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_SCI2_TX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI2_TX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI2_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SCI2_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_SCI2_TX.ADDMODEWR = ADDR_FIXED;
            g_dmaCTRLPKT_SCI2_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_SCI2_TRANSMIT_channel,g_dmaCTRLPKT_SCI2_TX);


            dmaReqAssign(DMA_SCI2_REVICE_channel, DMA_SCI2_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_SCI2_RX.SADD      =((uint32_t)(&(sciREG2->RD))+3)  ;
            g_dmaCTRLPKT_SCI2_RX.DADD      =(uint32)(SCI2_RX_DATA) ;
            g_dmaCTRLPKT_SCI2_RX.CHCTRL    = 1;
            g_dmaCTRLPKT_SCI2_RX.FRCNT = SCI2_RX_DATA_LEN;
            g_dmaCTRLPKT_SCI2_RX.ELCNT = 1;
            g_dmaCTRLPKT_SCI2_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SCI2_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SCI2_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SCI2_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SCI2_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_SCI2_RX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI2_RX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI2_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SCI2_RX.ADDMODERD = ADDR_FIXED;
            g_dmaCTRLPKT_SCI2_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_SCI2_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_SCI2_REVICE_channel,g_dmaCTRLPKT_SCI2_RX);

            dmaSetChEnable(DMA_SCI2_REVICE_channel, DMA_HW);

            sciREG2->SETINT |= SCI_SET_TX_DMA | SCI_SET_RX_DMA | SCI_SET_RX_DMA_ALL;
        break;
        case SCI3:
            while (((sciREG3->FLR & SCI_TX_INT) == 0U) || ((sciREG3->FLR & 0x4) == 0x4))
            {
            }
            dmaReqAssign(DMA_SCI3_TRANSMIT_channel, DMA_SCI3_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_SCI3_TX.SADD      = (uint32)(SCI3_TX_DATA) ;
            g_dmaCTRLPKT_SCI3_TX.DADD      = ((uint32_t)(&(sciREG3->TD))+3);
            g_dmaCTRLPKT_SCI3_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_SCI3_TX.FRCNT = 8;
            g_dmaCTRLPKT_SCI3_TX.ELCNT = 1;
            g_dmaCTRLPKT_SCI3_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SCI3_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SCI3_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SCI3_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SCI3_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_SCI3_TX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI3_TX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI3_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SCI3_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_SCI3_TX.ADDMODEWR = ADDR_FIXED;
            g_dmaCTRLPKT_SCI3_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_SCI3_TRANSMIT_channel,g_dmaCTRLPKT_SCI3_TX);


            dmaReqAssign(DMA_SCI3_REVICE_channel, DMA_SCI3_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_SCI3_RX.SADD      =((uint32_t)(&(sciREG3->RD))+3)  ;
            g_dmaCTRLPKT_SCI3_RX.DADD      =(uint32)(SCI3_RX_DATA) ;
            g_dmaCTRLPKT_SCI3_RX.CHCTRL    = 1;
            g_dmaCTRLPKT_SCI3_RX.FRCNT = SCI3_RX_DATA_LEN;
            g_dmaCTRLPKT_SCI3_RX.ELCNT = 1;
            g_dmaCTRLPKT_SCI3_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SCI3_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SCI3_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SCI3_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SCI3_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_SCI3_RX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI3_RX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI3_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SCI3_RX.ADDMODERD = ADDR_FIXED;
            g_dmaCTRLPKT_SCI3_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_SCI3_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_SCI3_REVICE_channel,g_dmaCTRLPKT_SCI3_RX);

            dmaSetChEnable(DMA_SCI3_REVICE_channel, DMA_HW);

            sciREG3->SETINT |= SCI_SET_TX_DMA | SCI_SET_RX_DMA | SCI_SET_RX_DMA_ALL;
        break;
        case SCI4:
            while (((sciREG4->FLR & SCI_TX_INT) == 0U) || ((sciREG4->FLR & 0x4) == 0x4))
            {
            }
            dmaReqAssign(DMA_SCI4_TRANSMIT_channel, DMA_SCI4_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_SCI4_TX.SADD      = (uint32)(SCI4_TX_DATA) ;
            g_dmaCTRLPKT_SCI4_TX.DADD      = ((uint32_t)(&(sciREG4->TD))+3);
            g_dmaCTRLPKT_SCI4_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_SCI4_TX.FRCNT = 8;
            g_dmaCTRLPKT_SCI4_TX.ELCNT = 1;
            g_dmaCTRLPKT_SCI4_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SCI4_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SCI4_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SCI4_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SCI4_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_SCI4_TX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI4_TX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI4_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SCI4_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_SCI4_TX.ADDMODEWR = ADDR_FIXED;
            g_dmaCTRLPKT_SCI4_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_SCI4_TRANSMIT_channel,g_dmaCTRLPKT_SCI4_TX);


            dmaReqAssign(DMA_SCI4_REVICE_channel, DMA_SCI4_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_SCI4_RX.SADD      =((uint32_t)(&(sciREG4->RD))+3)  ;
            g_dmaCTRLPKT_SCI4_RX.DADD      =(uint32)(SCI4_RX_DATA) ;
            g_dmaCTRLPKT_SCI4_RX.CHCTRL    = 1;
            g_dmaCTRLPKT_SCI4_RX.FRCNT = SCI1_RX_DATA_LEN;
            g_dmaCTRLPKT_SCI4_RX.ELCNT = 1;
            g_dmaCTRLPKT_SCI4_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SCI4_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SCI4_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SCI4_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SCI4_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_SCI4_RX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI4_RX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SCI4_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SCI4_RX.ADDMODERD = ADDR_FIXED;
            g_dmaCTRLPKT_SCI4_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_SCI4_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_SCI4_REVICE_channel,g_dmaCTRLPKT_SCI4_RX);

            dmaSetChEnable(DMA_SCI4_REVICE_channel, DMA_HW);

            sciREG4->SETINT |= SCI_SET_TX_DMA | SCI_SET_RX_DMA | SCI_SET_RX_DMA_ALL;
        break;
        default:
        break;
    }
}

void SCI_Tx(SCI_Info scix, uint8*buff,uint32 len)
{
    uint8 i=0;
    uint8 txdata=0;
    switch(scix.ch)
    {
    case SCI1:
        if(DMA_SCI1_TX==true)
        {
            for(i=0;i<len;i++)
            {
               SCI1_TX_DATA[i]=buff[i];
            }
            while (((sciREG1->FLR & SCI_TX_INT) == 0U) || ((sciREG1->FLR & 0x4) == 0x4));
            dmaSetChEnable(DMA_SCI1_TRANSMIT_channel, DMA_HW);

        }
        else
        {
            for(i=0;i<len;i++)
            {
                while ((sciREG1->FLR & (uint32)SCI_TX_INT) == 0U)
                {
                } /* Wait */
                /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
                txdata = buff[i];
                sciREG1->TD = (uint32)(txdata);
            }
        }
    break;
    case SCI2:
        if(DMA_SCI2_TX==true)
        {
            for(i=0;i<len;i++)
            {
               SCI2_TX_DATA[i]=buff[i];
            }
            dmaSetChEnable(DMA_SCI2_TRANSMIT_channel, DMA_HW);
        }
        else
        {
            for(i=0;i<len;i++)
            {
                while ((sciREG2->FLR & (uint32)SCI_TX_INT) == 0U)
                {
                } /* Wait */
                /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
                txdata = buff[i];
                sciREG2->TD = (uint32)(txdata);
            }
        }
    break;
    case SCI3:
        if(DMA_SCI3_TX==true)
        {
            for(i=0;i<len;i++)
            {
               SCI3_TX_DATA[i]=buff[i];
            }
            dmaSetChEnable(DMA_SCI3_TRANSMIT_channel, DMA_HW);
        }
        else
        {
            for(i=0;i<len;i++)
            {
                while ((sciREG3->FLR & (uint32)SCI_TX_INT) == 0U)
                {
                } /* Wait */
                /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
                txdata = buff[i];
                sciREG3->TD = (uint32)(txdata);
            }
        }
    break;
    case SCI4:
        if(DMA_SCI4_TX==true)
        {
            for(i=0;i<len;i++)
            {
               SCI4_TX_DATA[i]=buff[i];
            }
            dmaSetChEnable(DMA_SCI4_TRANSMIT_channel, DMA_HW);
        }
        else
        {
            for(i=0;i<len;i++)
            {
                while ((sciREG4->FLR & (uint32)SCI_TX_INT) == 0U)
                {
                } /* Wait */
                /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
                txdata = buff[i];
                sciREG4->TD = (uint32)(txdata);
            }
        }
    break;
    default:
    break;

    }
}

void SCI1_RX()
{
    uint8 i=0;
    if(DMA_SCI1_RX==true)
    {
        SCI1_MSG_Buff.head=0;
        SCI1_MSG_Buff.tail=0;
         for(i=0;i<SCI1_RX_SIZE;i++)
         {
             SCI1_MSG_Buff.frame_temp[i]=SCI1_RX_DATA[i];
             SCI1_MSG_Buff.tail++;
         }
    }
    else
    {
        SCI1_MSG_Buff.frame_temp[SCI1_MSG_Buff.tail]= (uint8)(sciREG1->RD & 0x000000FFU);
        SCI1_MSG_Buff.tail++;
    }
}
void SCI2_RX()
{
    uint8 i=0;
    if(DMA_SCI2_RX==true)
    {
        SCI2_MSG_Buff.head=0;
        SCI2_MSG_Buff.tail=0;
         for(i=0;i<SCI2_RX_SIZE;i++)
         {
             SCI2_MSG_Buff.frame_temp[i]=SCI2_RX_DATA[i];
             SCI2_MSG_Buff.tail++;
         }
    }
    else
    {
        SCI2_MSG_Buff.frame_temp[SCI2_MSG_Buff.tail]= (uint8)(sciREG2->RD & 0x000000FFU);
        SCI2_MSG_Buff.tail++;
    }
}
void SCI3_RX()
{
    uint8 i=0;
    if(DMA_SCI3_RX==true)
    {
        SCI3_MSG_Buff.head=0;
        SCI3_MSG_Buff.tail=0;
         for(i=0;i<SCI3_RX_SIZE;i++)
         {
             SCI3_MSG_Buff.frame_temp[i]=SCI3_RX_DATA[i];
             SCI3_MSG_Buff.tail++;
         }
    }
    else
    {
        SCI3_MSG_Buff.frame_temp[SCI3_MSG_Buff.tail]= (uint8)(sciREG3->RD & 0x000000FFU);
        SCI3_MSG_Buff.tail++;
    }
}
void SCI4_RX()
{
    uint8 i=0;
    if(DMA_SCI4_RX==true)
    {
        SCI4_MSG_Buff.head=0;
        SCI4_MSG_Buff.tail=0;
         for(i=0;i<SCI4_RX_SIZE;i++)
         {
             SCI4_MSG_Buff.frame_temp[i]=SCI4_RX_DATA[i];
             SCI4_MSG_Buff.tail++;
         }
    }
    else
    {
        SCI4_MSG_Buff.frame_temp[SCI4_MSG_Buff.tail]= (uint8)(sciREG4->RD & 0x000000FFU);
        SCI4_MSG_Buff.tail++;
    }
}

/*******************************************************************************
Function:       // sciNotification
Description:    // SCI interrupt handler
Input:          // sci   : hardware SCI module trigger the interrupt.
                // flags : type of interrupt in SCI module which trigger the interrupt
Output:         // none
*******************************************************************************/
void sciNotification(sciBASE_t *sci, uint32 flags)
{
    if(sci==sciREG1)
    {
        if((flags == SCI_RX_INT))
        {
            SCI1_RX();
        }

    }
}
