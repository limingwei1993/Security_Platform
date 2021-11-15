/*
 * spi.c
 *
 *  Created on: 2021年10月8日
 *      Author: 15755
 */



#include"spi.h"
#include"HL_reg_spi.h"
#include "HL_spi.h"
#include "HL_reg_mibspi.h"
#include "HL_sys_vim.h"
#include "HL_reg_dma.h"
#include "HL_sys_dma.h"
SPI_revice_buff SPI1_MSG_buff;      /*SPI1 接收数据队列*/
SPI_revice_buff SPI2_MSG_buff;      /*SPI2 接收数据队列*/
SPI_revice_buff SPI3_MSG_buff;      /*SPI3 接收数据队列*/
SPI_revice_buff SPI4_MSG_buff;      /*SPI4 接收数据队列*/
SPI_revice_buff SPI5_MSG_buff;      /*SPI5 接收数据队列*/
#define DMA_SPI1_CHANNEL DMA_CH4    /*SPI1 DMA中断通道编号*/
#define DMA_SPI2_CHANNEL DMA_CH5    /*SPI2 DMA中断通道编号*/
#define DMA_SPI3_CHANNEL DMA_CH6    /*SPI3 DMA中断通道编号*/
#define DMA_SPI4_CHANNEL DMA_CH7    /*SPI4 DMA中断通道编号*/
#define DMA_SPI5_CHANNEL DMA_CH8    /*SPI5 DMA中断通道编号*/
bool DMA_SPI1_TX=false;     /*SPI1 启用DMA发送标志*/
bool DMA_SPI2_TX=false;     /*SPI2 启用DMA发送标志*/
bool DMA_SPI3_TX=false;     /*SPI3 启用DMA发送标志*/
bool DMA_SPI4_TX=false;     /*SPI4 启用DMA发送标志*/
bool DMA_SPI5_TX=true;      /*SPI5 启用DMA发送标志*/
bool DMA_SPI1_RX=true;      /*SPI1 启用DMA接收标志*/
bool DMA_SPI2_RX=true;      /*SPI2 启用DMA接收标志*/
bool DMA_SPI3_RX=true;      /*SPI3 启用DMA接收标志*/
bool DMA_SPI4_RX=true;      /*SPI4 启用DMA接收标志*/
bool DMA_SPI5_RX=true;      /*SPI5 启用DMA接收标志*/
uint8 SPI1_TX_DATA[SPI1_send_SIZE];         /*SPI1 DMA发送数据队列*/
uint8 SPI1_RX_DATA[SPI1_revice_SIZE];       /*SPI1 DMA接收数据队列*/
uint8 SPI2_TX_DATA[SPI2_send_SIZE];         /*SPI2 DMA发送数据队列*/
uint8 SPI2_RX_DATA[SPI2_revice_SIZE];       /*SPI2 DMA接收数据队列*/
uint8 SPI3_TX_DATA[SPI3_send_SIZE];         /*SPI3 DMA发送数据队列*/
uint8 SPI3_RX_DATA[SPI3_revice_SIZE];       /*SPI3 DMA接收数据队列*/
uint8 SPI4_TX_DATA[SPI4_send_SIZE];         /*SPI3 DMA发送数据队列*/
uint8 SPI4_RX_DATA[SPI4_revice_SIZE];       /*SPI4 DMA接收数据队列*/
uint8 SPI5_TX_DATA[SPI5_send_SIZE];         /*SPI4 DMA发送数据队列*/
uint8 SPI5_RX_DATA[SPI5_revice_SIZE];       /*SPI5 DMA接收数据队列*/
g_dmaCTRL g_dmaCTRLPKT_SPI1_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI1_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI2_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI2_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI3_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI3_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI4_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI4_RX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI5_TX;             /* dma control packet configuration stack */
g_dmaCTRL g_dmaCTRLPKT_SPI5_RX;             /* dma control packet configuration stack */
/******************
 * 函数：void SPI_init(SPI_Info spix)
 * 功能：SPI 初始化
 * 输入：spix ： 参数信息。 ->ch：SCI编号；可选SPI1、SPI2、SPI3、SPI4.
 *                    ->CPHA：时钟相位。
 *                    ->CPOL：时钟极性。
 *                    ->mode：主从模式。可选： SPI_SLAVEL、SPI_MASTER
 *                    ->burt：波特率。
 * 输出：无
 * *******************/
void SPI_init(SPI_Info spix)
{

    uint32 clock_phase=0 ;
    uint32 clock_polarity=0 ;
    uint32 bauu ;
    bauu=VCLK/spix.burt-1;
    clock_phase=spix.CPHA;
    clock_polarity=spix.CPOL;
    switch(spix.ch)
    {
    case SPI1:
           spiREG1->GCR0 = 0U;
            spiREG1->GCR0 = 1U;

            /** SPI1 master mode and clock configuration */
            if(spix.mode==SPI_SLAVEL)
            {
              spiREG1->GCR1 = (spiREG1->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)0U << 1U)  /* CLOKMOD */
                          | 0U);  /* slavel */
            }
            else
            {
                spiREG1->GCR1 = (spiREG1->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)1U << 1U)  /* CLOKMOD */
                                          | 1U);  /* MASTER */
            }

            /** SPI1 enable pin configuration */
            spiREG1->INT0 = (spiREG1->INT0 & 0xFEFFFFFFU)| (uint32)((uint32)0U << 24U);  /* ENABLE HIGHZ */

            /** - Delays */
            spiREG1->DELAY = (uint32)((uint32)0U << 24U)  /* C2TDELAY */
                           | (uint32)((uint32)0U << 16U)  /* T2CDELAY */
                           | (uint32)((uint32)0U << 8U)   /* T2EDELAY */
                           | (uint32)((uint32)0U << 0U);  /* C2EDELAY */

            /** - Data Format 0 */
            spiREG1->FMT0 = (uint32)((uint32)0U << 24U)  /* wdelay */
                          | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                          | (uint32)((uint32)0U << 22U)  /* parity enable */
                          | (uint32)((uint32)0U << 21U)  /* wait on enable */
                          | (uint32)((uint32)0U << 20U)  /* shift direction  0--MSB   1==LSB*/
                          | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                          | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                          | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                          | (uint32)((uint32)8U << 0U);  /* data word length */

            /** - Data Format 1 */
            spiREG1->FMT1 = (uint32)((uint32)0U << 24U)  /* wdelay */
                          | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                          | (uint32)((uint32)0U << 22U)  /* parity enable */
                          | (uint32)((uint32)0U << 21U)  /* wait on enable */
                          | (uint32)((uint32)0U << 20U)  /* shift direction */
                          | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                          | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                          | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                          | (uint32)((uint32)8U << 0U);  /* data word length */

            /** - Data Format 2 */
            spiREG1->FMT2 = (uint32)((uint32)0U << 24U)  /* wdelay */
                          | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                          | (uint32)((uint32)0U << 22U)  /* parity enable */
                          | (uint32)((uint32)0U << 21U)  /* wait on enable */
                          | (uint32)((uint32)0U << 20U)  /* shift direction */
                          | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                          | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                          | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                          | (uint32)((uint32)8U << 0U);  /* data word length */

            /** - Data Format 3 */
            spiREG1->FMT3 = (uint32)((uint32)0U << 24U)  /* wdelay */
                          | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                          | (uint32)((uint32)0U << 22U)  /* parity enable */
                          | (uint32)((uint32)0U << 21U)  /* wait on enable */
                          | (uint32)((uint32)0U << 20U)  /* shift direction */
                          | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                          | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                          | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                          | (uint32)((uint32)8U << 0U);  /* data word length */

            /** - set interrupt levels */
            spiREG1->LVL = (uint32)((uint32)0U << 9U)  /* TXINT */
                         | (uint32)((uint32)0U << 8U)  /* RXINT */
                         | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                         | (uint32)((uint32)0U << 4U)  /* BITERR */
                         | (uint32)((uint32)0U << 3U)  /* DESYNC */
                         | (uint32)((uint32)0U << 2U)  /* PARERR */
                         | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                         | (uint32)((uint32)0U << 0U);  /* DLENERR */

            /** - clear any pending interrupts */
            spiREG1->FLG |= 0xFFFFU;

            /** - enable interrupts */
            spiREG1->INT0 = (spiREG1->INT0 & 0xFFFF0000U)
                          | (uint32)((uint32)0U << 9U)  /* TXINT */
                          | (uint32)((uint32)1U << 8U)  /* RXINT */
                          | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                          | (uint32)((uint32)0U << 4U)  /* BITERR */
                          | (uint32)((uint32)0U << 3U)  /* DESYNC */
                          | (uint32)((uint32)0U << 2U)  /* PARERR */
                          | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                          | (uint32)((uint32)0U << 0U);  /* DLENERR */

            /** @b initialize @b SPI1 @b Port */

            /** - SPI1 Port output values */
            spiREG1->PC3 =    (uint32)((uint32)1U << 0U)  /* SCS[0] */
                            | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                            | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                            | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                            | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                            | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                            | (uint32)((uint32)0U << 8U)  /* ENA */
                            | (uint32)((uint32)0U << 9U)  /* CLK */
                            | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                            | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                            | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                            | (uint32)((uint32)0U << 25U); /* SOMI[1] */

            /** - SPI1 Port direction  0--input   1--output */
            spiREG1->PC1  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                            | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                            | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                            | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                            | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                            | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                            | (uint32)((uint32)0U << 8U)  /* ENA */
                            | (uint32)((uint32)1U << 9U)  /* CLK */
                            | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                            | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                            | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                            | (uint32)((uint32)0U << 25U); /* SOMI[1] */

            /** - SPI1 Port open drain enable */
            spiREG1->PC6  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                            | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                            | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                            | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                            | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                            | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                            | (uint32)((uint32)0U << 8U)  /* ENA */
                            | (uint32)((uint32)0U << 9U)  /* CLK */
                            | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                            | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                            | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                            | (uint32)((uint32)0U << 25U); /* SOMI[1] */

            /** - SPI1 Port pullup / pulldown selection  0--down  1--up */
            spiREG1->PC8  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                            | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                            | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                            | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                            | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                            | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                            | (uint32)((uint32)1U << 8U)  /* ENA */
                            | (uint32)((uint32)1U << 9U)  /* CLK */
                            | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                            | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                            | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                            | (uint32)((uint32)1U << 25U); /* SOMI[1] */

            /** - SPI1 Port pullup / pulldown enable  0--enable  1--disable*/
            spiREG1->PC7  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                            | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                            | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                            | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                            | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                            | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                            | (uint32)((uint32)0U << 8U)  /* ENA */
                            | (uint32)((uint32)0U << 9U)  /* CLK */
                            | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                            | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                            | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                            | (uint32)((uint32)0U << 25U); /* SOMI[1] */

            /* SPI1 set all pins to functional 0--gpio  1--spi*/
            spiREG1->PC0  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                            | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                            | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                            | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                            | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                            | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                            | (uint32)((uint32)1U << 8U)  /* ENA */
                            | (uint32)((uint32)1U << 9U)  /* CLK */
                            | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                            | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                            | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                            | (uint32)((uint32)1U << 25U); /* SOMI[1] */


            /** - Finally start SPI1 */
            spiREG1->GCR1 = (spiREG1->GCR1 & 0xFEFFFFFFU) | 0x01000000U;

    break;
    case SPI2:
        spiREG2->GCR0 = 0U;
                  spiREG2->GCR0 = 1U;

                  /** SPI1 master mode and clock configuration */
                  if(spix.mode==SPI_SLAVEL)
                  {
                    spiREG2->GCR1 = (spiREG2->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)0U << 1U)  /* CLOKMOD */
                                | 0U);  /* slavel */
                  }
                  else
                  {
                      spiREG2->GCR1 = (spiREG2->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)1U << 1U)  /* CLOKMOD */
                                                | 1U);  /* MASTER */
                  }

                  /** SPI1 enable pin configuration */
                  spiREG2->INT0 = (spiREG2->INT0 & 0xFEFFFFFFU)| (uint32)((uint32)0U << 24U);  /* ENABLE HIGHZ */

                  /** - Delays */
                  spiREG2->DELAY = (uint32)((uint32)0U << 24U)  /* C2TDELAY */
                                 | (uint32)((uint32)0U << 16U)  /* T2CDELAY */
                                 | (uint32)((uint32)0U << 8U)   /* T2EDELAY */
                                 | (uint32)((uint32)0U << 0U);  /* C2EDELAY */

                  /** - Data Format 0 */
                  spiREG2->FMT0 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction  0--MSB   1==LSB*/
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 1 */
                  spiREG2->FMT1 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 2 */
                  spiREG2->FMT2 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 3 */
                  spiREG2->FMT3 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - set interrupt levels */
                  spiREG2->LVL = (uint32)((uint32)0U << 9U)  /* TXINT */
                               | (uint32)((uint32)0U << 8U)  /* RXINT */
                               | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                               | (uint32)((uint32)0U << 4U)  /* BITERR */
                               | (uint32)((uint32)0U << 3U)  /* DESYNC */
                               | (uint32)((uint32)0U << 2U)  /* PARERR */
                               | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                               | (uint32)((uint32)0U << 0U);  /* DLENERR */

                  /** - clear any pending interrupts */
                  spiREG2->FLG |= 0xFFFFU;

                  /** - enable interrupts */
                  spiREG2->INT0 = (spiREG2->INT0 & 0xFFFF0000U)
                                | (uint32)((uint32)0U << 9U)  /* TXINT */
                                | (uint32)((uint32)1U << 8U)  /* RXINT */
                                | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                                | (uint32)((uint32)0U << 4U)  /* BITERR */
                                | (uint32)((uint32)0U << 3U)  /* DESYNC */
                                | (uint32)((uint32)0U << 2U)  /* PARERR */
                                | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                                | (uint32)((uint32)0U << 0U);  /* DLENERR */

                  /** @b initialize @b SPI1 @b Port */

                  /** - SPI1 Port output values */
                  spiREG2->PC3 =    (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port direction  0--input   1--output */
                  spiREG2->PC1  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port open drain enable */
                  spiREG2->PC6  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port pullup / pulldown selection  0--down  1--up */
                  spiREG2->PC8  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)1U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)1U << 25U); /* SOMI[1] */

                  /** - SPI1 Port pullup / pulldown enable  0--enable  1--disable*/
                  spiREG2->PC7  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /* SPI1 set all pins to functional 0--gpio  1--spi*/
                  spiREG2->PC0  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)1U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)1U << 25U); /* SOMI[1] */


                  /** - Finally start SPI1 */
                  spiREG2->GCR1 = (spiREG2->GCR1 & 0xFEFFFFFFU) | 0x01000000U;


       break;
    case SPI3:
        spiREG3->GCR0 = 0U;
                  spiREG3->GCR0 = 1U;

                  /** SPI1 master mode and clock configuration */
                  if(spix.mode==SPI_SLAVEL)
                {
                  spiREG3->GCR1 = (spiREG3->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)0U << 1U)  /* CLOKMOD */
                              | 0U);  /* slavel */
                }
                else
                {
                    spiREG3->GCR1 = (spiREG3->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)1U << 1U)  /* CLOKMOD */
                                              | 1U);  /* MASTER */
                }

                  /** SPI1 enable pin configuration */
                  spiREG3->INT0 = (spiREG3->INT0 & 0xFEFFFFFFU)| (uint32)((uint32)0U << 24U);  /* ENABLE HIGHZ */

                  /** - Delays */
                  spiREG3->DELAY = (uint32)((uint32)0U << 24U)  /* C2TDELAY */
                                 | (uint32)((uint32)0U << 16U)  /* T2CDELAY */
                                 | (uint32)((uint32)0U << 8U)   /* T2EDELAY */
                                 | (uint32)((uint32)0U << 0U);  /* C2EDELAY */

                  /** - Data Format 0 */
                  spiREG3->FMT0 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction  0--MSB   1==LSB*/
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 1 */
                  spiREG3->FMT1 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 2 */
                  spiREG3->FMT2 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 3 */
                  spiREG3->FMT3 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - set interrupt levels */
                  spiREG3->LVL = (uint32)((uint32)0U << 9U)  /* TXINT */
                               | (uint32)((uint32)0U << 8U)  /* RXINT */
                               | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                               | (uint32)((uint32)0U << 4U)  /* BITERR */
                               | (uint32)((uint32)0U << 3U)  /* DESYNC */
                               | (uint32)((uint32)0U << 2U)  /* PARERR */
                               | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                               | (uint32)((uint32)0U << 0U);  /* DLENERR */

                  /** - clear any pending interrupts */
                  spiREG3->FLG |= 0xFFFFU;

                  /** - enable interrupts */
                  spiREG3->INT0 = (spiREG3->INT0 & 0xFFFF0000U)
                                | (uint32)((uint32)0U << 9U)  /* TXINT */
                                | (uint32)((uint32)1U << 8U)  /* RXINT */
                                | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                                | (uint32)((uint32)0U << 4U)  /* BITERR */
                                | (uint32)((uint32)0U << 3U)  /* DESYNC */
                                | (uint32)((uint32)0U << 2U)  /* PARERR */
                                | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                                | (uint32)((uint32)0U << 0U);  /* DLENERR */

                  /** @b initialize @b SPI1 @b Port */

                  /** - SPI1 Port output values */
                  spiREG3->PC3 =    (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port direction  0--input   1--output */
                  spiREG3->PC1  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port open drain enable */
                  spiREG3->PC6  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port pullup / pulldown selection  0--down  1--up */
                  spiREG3->PC8  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)1U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)1U << 25U); /* SOMI[1] */

                  /** - SPI1 Port pullup / pulldown enable  0--enable  1--disable*/
                  spiREG3->PC7  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /* SPI1 set all pins to functional 0--gpio  1--spi*/
                  spiREG3->PC0  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)1U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)1U << 25U); /* SOMI[1] */


                  /** - Finally start SPI1 */
                  spiREG3->GCR1 = (spiREG3->GCR1 & 0xFEFFFFFFU) | 0x01000000U;

       break;
    case SPI4:
        spiREG4->GCR0 = 0U;
                  spiREG4->GCR0 = 1U;

                  /** SPI1 master mode and clock configuration */
                  if(spix.mode==SPI_SLAVEL)
                  {
                    spiREG4->GCR1 = (spiREG4->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)0U << 1U)  /* CLOKMOD */
                                | 0U);  /* slavel */
                  }
                  else
                  {
                      spiREG4->GCR1 = (spiREG4->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)1U << 1U)  /* CLOKMOD */
                                                | 1U);  /* MASTER */
                  }

                  /** SPI1 enable pin configuration */
                  spiREG4->INT0 = (spiREG4->INT0 & 0xFEFFFFFFU)| (uint32)((uint32)0U << 24U);  /* ENABLE HIGHZ */

                  /** - Delays */
                  spiREG4->DELAY = (uint32)((uint32)0U << 24U)  /* C2TDELAY */
                                 | (uint32)((uint32)0U << 16U)  /* T2CDELAY */
                                 | (uint32)((uint32)0U << 8U)   /* T2EDELAY */
                                 | (uint32)((uint32)0U << 0U);  /* C2EDELAY */

                  /** - Data Format 0 */
                  spiREG4->FMT0 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction  0--MSB   1==LSB*/
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 1 */
                  spiREG4->FMT1 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 2 */
                  spiREG4->FMT2 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 3 */
                  spiREG4->FMT3 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - set interrupt levels */
                  spiREG4->LVL = (uint32)((uint32)0U << 9U)  /* TXINT */
                               | (uint32)((uint32)0U << 8U)  /* RXINT */
                               | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                               | (uint32)((uint32)0U << 4U)  /* BITERR */
                               | (uint32)((uint32)0U << 3U)  /* DESYNC */
                               | (uint32)((uint32)0U << 2U)  /* PARERR */
                               | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                               | (uint32)((uint32)0U << 0U);  /* DLENERR */

                  /** - clear any pending interrupts */
                  spiREG4->FLG |= 0xFFFFU;

                  /** - enable interrupts */
                  spiREG4->INT0 = (spiREG4->INT0 & 0xFFFF0000U)
                                | (uint32)((uint32)0U << 9U)  /* TXINT */
                                | (uint32)((uint32)1U << 8U)  /* RXINT */
                                | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                                | (uint32)((uint32)0U << 4U)  /* BITERR */
                                | (uint32)((uint32)0U << 3U)  /* DESYNC */
                                | (uint32)((uint32)0U << 2U)  /* PARERR */
                                | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                                | (uint32)((uint32)0U << 0U);  /* DLENERR */

                  /** @b initialize @b SPI1 @b Port */

                  /** - SPI1 Port output values */
                  spiREG4->PC3 =    (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port direction  0--input   1--output */
                  spiREG4->PC1  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port open drain enable */
                  spiREG4->PC6  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port pullup / pulldown selection  0--down  1--up */
                  spiREG4->PC8  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)1U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)1U << 25U); /* SOMI[1] */

                  /** - SPI1 Port pullup / pulldown enable  0--enable  1--disable*/
                  spiREG4->PC7  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /* SPI1 set all pins to functional 0--gpio  1--spi*/
                  spiREG4->PC0  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)1U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)1U << 25U); /* SOMI[1] */


                  /** - Finally start SPI1 */
                  spiREG4->GCR1 = (spiREG4->GCR1 & 0xFEFFFFFFU) | 0x01000000U;


       break;
    case SPI5:
        spiREG5->GCR0 = 0U;
                  spiREG5->GCR0 = 1U;

                  /** SPI1 master mode and clock configuration */
                  if(spix.mode==SPI_SLAVEL)
                 {
                    spiREG5->GCR1 = (spiREG5->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)0U << 1U)  /* CLOKMOD */
                               | 0U);  /* slavel */
                 }
                 else
                 {
                    spiREG5->GCR1 = (spiREG5->GCR1 & 0xFFFFFFFCU) | ((uint32)((uint32)1U << 1U)  /* CLOKMOD */
                                              | 1U);  /* MASTER */
                 }

                  /** SPI1 enable pin configuration */
                  spiREG5->INT0 = (spiREG5->INT0 & 0xFEFFFFFFU)| (uint32)((uint32)0U << 24U);  /* ENABLE HIGHZ */

                  /** - Delays */
                  spiREG5->DELAY = (uint32)((uint32)0U << 24U)  /* C2TDELAY */
                                 | (uint32)((uint32)0U << 16U)  /* T2CDELAY */
                                 | (uint32)((uint32)0U << 8U)   /* T2EDELAY */
                                 | (uint32)((uint32)0U << 0U);  /* C2EDELAY */

                  /** - Data Format 0 */
                  spiREG5->FMT0 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction  0--MSB   1==LSB*/
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 1 */
                  spiREG5->FMT1 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 2 */
                  spiREG5->FMT2 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - Data Format 3 */
                  spiREG5->FMT3 = (uint32)((uint32)0U << 24U)  /* wdelay */
                                | (uint32)((uint32)0U << 23U)  /* parity Polarity */
                                | (uint32)((uint32)0U << 22U)  /* parity enable */
                                | (uint32)((uint32)0U << 21U)  /* wait on enable */
                                | (uint32)((uint32)0U << 20U)  /* shift direction */
                                | (uint32)((uint32)clock_polarity << 17U)  /* clock polarity */
                                | (uint32)((uint32)clock_phase << 16U)  /* clock phase */
                                | (uint32)((uint32)bauu << 8U) /* baudrate prescale */
                                | (uint32)((uint32)8U << 0U);  /* data word length */

                  /** - set interrupt levels */
                  spiREG5->LVL = (uint32)((uint32)0U << 9U)  /* TXINT */
                               | (uint32)((uint32)0U << 8U)  /* RXINT */
                               | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                               | (uint32)((uint32)0U << 4U)  /* BITERR */
                               | (uint32)((uint32)0U << 3U)  /* DESYNC */
                               | (uint32)((uint32)0U << 2U)  /* PARERR */
                               | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                               | (uint32)((uint32)0U << 0U);  /* DLENERR */

                  /** - clear any pending interrupts */
                  spiREG5->FLG |= 0xFFFFU;

                  /** - enable interrupts */
                  spiREG5->INT0 = (spiREG5->INT0 & 0xFFFF0000U)
                                | (uint32)((uint32)0U << 9U)  /* TXINT */
                                | (uint32)((uint32)1U << 8U)  /* RXINT */
                                | (uint32)((uint32)0U << 6U)  /* OVRNINT */
                                | (uint32)((uint32)0U << 4U)  /* BITERR */
                                | (uint32)((uint32)0U << 3U)  /* DESYNC */
                                | (uint32)((uint32)0U << 2U)  /* PARERR */
                                | (uint32)((uint32)0U << 1U) /* TIMEOUT */
                                | (uint32)((uint32)0U << 0U);  /* DLENERR */

                  /** @b initialize @b SPI1 @b Port */

                  /** - SPI1 Port output values */
                  spiREG5->PC3 =    (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port direction  0--input   1--output */
                  spiREG5->PC1  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port open drain enable */
                  spiREG5->PC6  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /** - SPI1 Port pullup / pulldown selection  0--down  1--up */
                  spiREG5->PC8  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)1U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)1U << 25U); /* SOMI[1] */

                  /** - SPI1 Port pullup / pulldown enable  0--enable  1--disable*/
                  spiREG5->PC7  =   (uint32)((uint32)0U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)0U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)0U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)0U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)0U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)0U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)0U << 8U)  /* ENA */
                                  | (uint32)((uint32)0U << 9U)  /* CLK */
                                  | (uint32)((uint32)0U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)0U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)0U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)0U << 25U); /* SOMI[1] */

                  /* SPI1 set all pins to functional 0--gpio  1--spi*/
                  spiREG5->PC0  =   (uint32)((uint32)1U << 0U)  /* SCS[0] */
                                  | (uint32)((uint32)1U << 1U)  /* SCS[1] */
                                  | (uint32)((uint32)1U << 2U)  /* SCS[2] */
                                  | (uint32)((uint32)1U << 3U)  /* SCS[3] */
                                  | (uint32)((uint32)1U << 4U)  /* SCS[4] */
                                  | (uint32)((uint32)1U << 5U)  /* SCS[5] */
                                  | (uint32)((uint32)1U << 8U)  /* ENA */
                                  | (uint32)((uint32)1U << 9U)  /* CLK */
                                  | (uint32)((uint32)1U << 10U)  /* SIMO[0] */
                                  | (uint32)((uint32)1U << 11U)  /* SOMI[0] */
                                  | (uint32)((uint32)1U << 17U)  /* SIMO[1] */
                                  | (uint32)((uint32)1U << 25U); /* SOMI[1] */


                  /** - Finally start SPI1 */
                  spiREG5->GCR1 = (spiREG5->GCR1 & 0xFEFFFFFFU) | 0x01000000U;

       break;
       default:
       break;
    }
}

/******************
 * 函数：void SPI_Master_send(SPI_Info spix,uint8* buff, uint32 len)
 * 功能：SPI 主模式发送数据
 * 输入：spix ： 参数信息。 ->ch：SCI编号；可选SPI1、SPI2、SPI3、SPI4.
 *                    ->CPHA：时钟相位。
 *                    ->CPOL：时钟极性。
 *                    ->mode：主从模式。可选： SPI_SLAVEL、SPI_MASTER
 *                    ->burt：波特率。
 *     buff：发送的数据。
 *     len：发送的数据长度。
 * 输出：无
 * *******************/

void SPI_Master_send(SPI_Info spix,uint8* buff, uint32 len)
{
    uint8 i=0;
    switch(spix.ch)
    {
    case SPI1:
        if(DMA_SPI1_TX==true)
        {
            for(i=0;i<len;i++)
            {
                SPI1_TX_DATA[i]=buff[i];
            }
            dmaSetChEnable(DMA_SPI1_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;
                   spiREG1->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0 << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG1->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG1->BUF;
                   len--;
               }

        }
        break;
    case SPI2:
        if(DMA_SPI2_TX==true)
        {
                for(i=0;i<len;i++)
                {
                    SPI2_TX_DATA[i]=buff[i];
                }
                dmaSetChEnable(DMA_SPI2_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG2->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0 << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG2->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG2->BUF;
                   len--;
               }

        }
        break;
    case SPI3:
        if(DMA_SPI3_TX==true)
        {
                for(i=0;i<len;i++)
                {
                    SPI3_TX_DATA[i]=buff[i];
                }
                dmaSetChEnable(DMA_SPI3_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG3->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0 << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG3->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG3->BUF;
                   len--;
               }

        }
        break;
    case SPI4:
        if(DMA_SPI4_TX==true)
        {
                for(i=0;i<len;i++)
                {
                    SPI4_TX_DATA[i]=buff[i];
                }
                dmaSetChEnable(DMA_SPI4_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG4->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0 << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG4->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG4->BUF;
                   len--;
               }

        }
        break;
    case SPI5:
        if(DMA_SPI5_TX==true)
        {
                for(i=0;i<len;i++)
                {
                    SPI5_TX_DATA[i]=buff[i];
                }
                dmaSetChEnable(DMA_SPI5_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG5->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0 << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG5->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG5->BUF;
                   len--;
               }

        }
        break;
    default:
        break;
    }

}
/******************
 * 函数：void SPI_Slave_send(SPI_Info spix,uint8* buff, uint32 len)
 * 功能：SPI 从模式发送数据
 * 输入：spix ： 参数信息。 ->ch：SCI编号；可选SPI1、SPI2、SPI3、SPI4.
 *                    ->CPHA：时钟相位。
 *                    ->CPOL：时钟极性。
 *                    ->mode：主从模式。可选： SPI_SLAVEL、SPI_MASTER
 *                    ->burt：波特率。
 *     buff：发送的数据。
 *     len：发送的数据长度。
 * 输出：无
 * *******************/
void SPI_Slave_send(SPI_Info spix,uint8* buff, uint32 len)
{
    uint8 i=0;
    switch(spix.ch)
    {
    case SPI1:
        if(DMA_SPI1_TX==true)
        {
            for(i=0;i<len;i++)
            {
                SPI1_TX_DATA[i]=0xff00 | buff[i];
            }
            dmaSetChEnable(DMA_SPI1_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG1->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0xff << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG1->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG1->BUF;
                   len--;
               }

        }
        break;
    case SPI2:
        if(DMA_SPI2_TX==true)
        {
                for(i=0;i<len;i++)
                {
                    SPI2_TX_DATA[i]=0xff00 | buff[i];
                }
                dmaSetChEnable(DMA_SPI2_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG2->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0xff << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG2->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG2->BUF;
                   len--;
               }

        }
        break;
    case SPI3:
        if(DMA_SPI3_TX==true)
        {
                for(i=0;i<len;i++)
                {
                    SPI3_TX_DATA[i]=0xff00 | buff[i];
                }
                dmaSetChEnable(DMA_SPI3_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG3->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0xff << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG3->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG3->BUF;
                   len--;
               }

        }
        break;
    case SPI4:
        if(DMA_SPI4_TX==true)
        {
                for(i=0;i<len;i++)
                {
                    SPI4_TX_DATA[i]=0xff00 | buff[i];
                }
                dmaSetChEnable(DMA_SPI4_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG4->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0xff << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG4->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG4->BUF;
                   len--;
               }

        }
        break;
    case SPI5:
        if(DMA_SPI5_TX==true)
        {
                for(i=0;i<len;i++)
                {
                    SPI5_TX_DATA[i]=0xff00 | buff[i];
                }
                dmaSetChEnable(DMA_SPI5_TRANSMIT_channel, DMA_HW);
        }
        else
        {
               volatile uint32 SpiBuf;
               uint8 Tx_Data;
               while(len != 0U)
               {
                   Tx_Data = *buff;

                   spiREG5->DAT1 =  ((uint32)0 << 24U) |
                                ((uint32)0xff << 16U) |
                                (0)           |
                                (0) |
                                (uint32)Tx_Data;
                   /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
                   buff++;
                   /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
                   while((spiREG5->FLG & 0x00000200U) != 0x00000200U)
                   {
                   } /* Wait */
                   SpiBuf = spiREG5->BUF;
                   len--;
               }

        }
        break;
    default:
           break;
    }

}
/******************
 * 函数：void SPI1_revive(uint8_t  revice_data)
 * 功能：SPI1 保存接收到的数据
 * 输入：srevice_data：接收到的数据
 * 输出：无
 * *******************/

void SPI1_revive(uint8_t  revice_data)
{
      SPI1_MSG_buff.revicebuf[SPI1_MSG_buff.tail]=revice_data;
      SPI1_MSG_buff.tail = (SPI1_MSG_buff.tail + 1) % kSPIBuffLen;
}
/******************
 * 函数：void SPI2_revive(uint8_t  revice_data)
 * 功能：SPI2 保存接收到的数据
 * 输入：srevice_data：接收到的数据
 * 输出：无
 * *******************/
void SPI2_revive(uint8_t  revice_data)
{
      SPI2_MSG_buff.revicebuf[SPI2_MSG_buff.tail]=revice_data;
      SPI2_MSG_buff.tail = (SPI2_MSG_buff.tail + 1) % kSPIBuffLen;
}
/******************
 * 函数：void SPI3_revive(uint8_t  revice_data)
 * 功能：SPI3 保存接收到的数据
 * 输入：srevice_data：接收到的数据
 * 输出：无
 * *******************/
void SPI3_revive(uint8_t  revice_data)
{
      SPI3_MSG_buff.revicebuf[SPI3_MSG_buff.tail]=revice_data;
      SPI3_MSG_buff.tail = (SPI3_MSG_buff.tail + 1) % kSPIBuffLen;
}
/******************
 * 函数：void SPI4_revive(uint8_t  revice_data)
 * 功能：SPI4 保存接收到的数据
 * 输入：srevice_data：接收到的数据
 * 输出：无
 * *******************/
void SPI4_revive(uint8_t  revice_data)
{
      SPI4_MSG_buff.revicebuf[SPI4_MSG_buff.tail]=revice_data;
      SPI4_MSG_buff.tail = (SPI4_MSG_buff.tail + 1) % kSPIBuffLen;
}
/******************
 * 函数：void SPI5_revive(uint8_t  revice_data)
 * 功能：SPI5 保存接收到的数据
 * 输入：srevice_data：接收到的数据
 * 输出：无
 * *******************/
void SPI5_revive(uint8_t  revice_data)
{
      SPI5_MSG_buff.revicebuf[SPI5_MSG_buff.tail]=revice_data;
      SPI5_MSG_buff.tail = (SPI5_MSG_buff.tail + 1) % kSPIBuffLen;
}

/******************
 * 函数：void SPI_DMA_init(SPI_Info spix)
 * 功能：SPI DMA配置
 * 输入：spix ： 参数信息。 ->ch：SCI编号；可选SPI1、SPI2、SPI3、SPI4.
 *                    ->CPHA：时钟相位。
 *                    ->CPOL：时钟极性。
 *                    ->mode：主从模式。可选： SPI_SLAVEL、SPI_MASTER
 *                    ->burt：波特率。
 * 输出：无
 * *******************/
void SPI_DMA_init(SPI_Info spix)
{

      switch(spix.ch)
      {
      case SPI1:
            while((spiREG1->FLG & 0x00000200U) != 0x00000200U);
            dmaReqAssign(DMA_SPI1_TRANSMIT_channel, DMA_SPI1_TRANSMIT_REQUEST_LINE);
            g_dmaCTRLPKT_SPI1_TX.SADD      = (uint32)(SPI1_TX_DATA) ;
            g_dmaCTRLPKT_SPI1_TX.DADD      = ((uint32_t)(&(spiREG2->DAT1))+3);
            g_dmaCTRLPKT_SPI1_TX.CHCTRL    = 0;
            g_dmaCTRLPKT_SPI1_TX.FRCNT = SPI1_send_SIZE;
            g_dmaCTRLPKT_SPI1_TX.ELCNT = 1;
            g_dmaCTRLPKT_SPI1_TX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SPI1_TX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SPI1_TX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SPI1_TX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SPI1_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
            g_dmaCTRLPKT_SPI1_TX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SPI1_TX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SPI1_TX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SPI1_TX.ADDMODERD = ADDR_INC1;
            g_dmaCTRLPKT_SPI1_TX.ADDMODEWR = ADDR_FIXED;
            g_dmaCTRLPKT_SPI1_TX.AUTOINIT  = AUTOINIT_OFF;

            dmaSetCtrlPacket(DMA_SPI1_TRANSMIT_channel,g_dmaCTRLPKT_SPI1_TX);


            dmaReqAssign(DMA_SPI1_REVICE_channel, DMA_SPI1_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_SPI1_RX.SADD      =((uint32_t)(&(spiREG1->BUF))+3)  ;
            g_dmaCTRLPKT_SPI1_RX.DADD      =(uint32)(SPI1_RX_DATA) ;
            g_dmaCTRLPKT_SPI1_RX.CHCTRL    = 0;
            g_dmaCTRLPKT_SPI1_RX.FRCNT = SPI1_revice_SIZE;
            g_dmaCTRLPKT_SPI1_RX.ELCNT = 1;
            g_dmaCTRLPKT_SPI1_RX.ELDOFFSET = 0;
            g_dmaCTRLPKT_SPI1_RX.ELSOFFSET = 0;
            g_dmaCTRLPKT_SPI1_RX.FRDOFFSET = 0;
            g_dmaCTRLPKT_SPI1_RX.FRSOFFSET = 0;
            g_dmaCTRLPKT_SPI1_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_SPI1_RX.RDSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SPI1_RX.WRSIZE    = ACCESS_8_BIT;
            g_dmaCTRLPKT_SPI1_RX.TTYPE     = FRAME_TRANSFER ;
            g_dmaCTRLPKT_SPI1_RX.ADDMODERD = ADDR_FIXED;
            g_dmaCTRLPKT_SPI1_RX.ADDMODEWR = ADDR_INC1;
            g_dmaCTRLPKT_SPI1_RX.AUTOINIT  = AUTOINIT_ON;

            dmaSetCtrlPacket(DMA_SPI1_REVICE_channel,g_dmaCTRLPKT_SPI1_RX);

            dmaSetChEnable(DMA_SPI1_REVICE_channel, DMA_HW);

            spiREG1->INT0 |=(0x01 << 16);

      break;
      case SPI2:
           while((spiREG2->FLG & 0x00000200U) != 0x00000200U);
           dmaReqAssign(DMA_SPI2_TRANSMIT_channel, DMA_SPI2_TRANSMIT_REQUEST_LINE);
           g_dmaCTRLPKT_SPI2_TX.SADD      = (uint32)(SPI2_TX_DATA) ;
           g_dmaCTRLPKT_SPI2_TX.DADD      = ((uint32_t)(&(spiREG2->DAT1))+3);
           g_dmaCTRLPKT_SPI2_TX.CHCTRL    = 0;
           g_dmaCTRLPKT_SPI2_TX.FRCNT = SPI2_send_SIZE;
           g_dmaCTRLPKT_SPI2_TX.ELCNT = 1;
           g_dmaCTRLPKT_SPI2_TX.ELDOFFSET = 0;
           g_dmaCTRLPKT_SPI2_TX.ELSOFFSET = 0;
           g_dmaCTRLPKT_SPI2_TX.FRDOFFSET = 0;
           g_dmaCTRLPKT_SPI2_TX.FRSOFFSET = 0;
           g_dmaCTRLPKT_SPI2_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
           g_dmaCTRLPKT_SPI2_TX.RDSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI2_TX.WRSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI2_TX.TTYPE     = FRAME_TRANSFER ;
           g_dmaCTRLPKT_SPI2_TX.ADDMODERD = ADDR_INC1;
           g_dmaCTRLPKT_SPI2_TX.ADDMODEWR = ADDR_FIXED;
           g_dmaCTRLPKT_SPI2_TX.AUTOINIT  = AUTOINIT_OFF;

           dmaSetCtrlPacket(DMA_SPI2_TRANSMIT_channel,g_dmaCTRLPKT_SPI2_TX);


           dmaReqAssign(DMA_SPI2_REVICE_channel, DMA_SPI2_REVICE_REQUEST_LINE);
           g_dmaCTRLPKT_SPI2_RX.SADD      =((uint32_t)(&(spiREG2->BUF))+3)  ;
           g_dmaCTRLPKT_SPI2_RX.DADD      =(uint32)(SPI2_RX_DATA) ;
           g_dmaCTRLPKT_SPI2_RX.CHCTRL    = 0;
           g_dmaCTRLPKT_SPI2_RX.FRCNT = SPI2_revice_SIZE;
           g_dmaCTRLPKT_SPI2_RX.ELCNT = 1;
           g_dmaCTRLPKT_SPI2_RX.ELDOFFSET = 0;
           g_dmaCTRLPKT_SPI2_RX.ELSOFFSET = 0;
           g_dmaCTRLPKT_SPI2_RX.FRDOFFSET = 0;
           g_dmaCTRLPKT_SPI2_RX.FRSOFFSET = 0;
           g_dmaCTRLPKT_SPI2_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
           g_dmaCTRLPKT_SPI2_RX.RDSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI2_RX.WRSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI2_RX.TTYPE     = FRAME_TRANSFER ;
           g_dmaCTRLPKT_SPI2_RX.ADDMODERD = ADDR_FIXED;
           g_dmaCTRLPKT_SPI2_RX.ADDMODEWR = ADDR_INC1;
           g_dmaCTRLPKT_SPI2_RX.AUTOINIT  = AUTOINIT_ON;

           dmaSetCtrlPacket(DMA_SPI2_REVICE_channel,g_dmaCTRLPKT_SPI2_RX);

           dmaSetChEnable(DMA_SPI2_REVICE_channel, DMA_HW);

           spiREG2->INT0 |=(0x01 << 16);
      break;
      case SPI3:
           while((spiREG3->FLG & 0x00000200U) != 0x00000200U);
           dmaReqAssign(DMA_SPI3_TRANSMIT_channel, DMA_SPI3_TRANSMIT_REQUEST_LINE);
           g_dmaCTRLPKT_SPI3_TX.SADD      = (uint32)(SPI3_TX_DATA) ;
           g_dmaCTRLPKT_SPI3_TX.DADD      = ((uint32_t)(&(spiREG3->DAT1))+3);
           g_dmaCTRLPKT_SPI3_TX.CHCTRL    = 0;
           g_dmaCTRLPKT_SPI3_TX.FRCNT = SPI3_send_SIZE;
           g_dmaCTRLPKT_SPI3_TX.ELCNT = 1;
           g_dmaCTRLPKT_SPI3_TX.ELDOFFSET = 0;
           g_dmaCTRLPKT_SPI3_TX.ELSOFFSET = 0;
           g_dmaCTRLPKT_SPI3_TX.FRDOFFSET = 0;
           g_dmaCTRLPKT_SPI3_TX.FRSOFFSET = 0;
           g_dmaCTRLPKT_SPI3_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
           g_dmaCTRLPKT_SPI3_TX.RDSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI3_TX.WRSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI3_TX.TTYPE     = FRAME_TRANSFER ;
           g_dmaCTRLPKT_SPI3_TX.ADDMODERD = ADDR_INC1;
           g_dmaCTRLPKT_SPI3_TX.ADDMODEWR = ADDR_FIXED;
           g_dmaCTRLPKT_SPI3_TX.AUTOINIT  = AUTOINIT_OFF;

           dmaSetCtrlPacket(DMA_SPI3_TRANSMIT_channel,g_dmaCTRLPKT_SPI3_TX);


           dmaReqAssign(DMA_SPI3_REVICE_channel, DMA_SPI3_REVICE_REQUEST_LINE);
           g_dmaCTRLPKT_SPI3_RX.SADD      =((uint32_t)(&(spiREG3->BUF))+3)  ;
           g_dmaCTRLPKT_SPI3_RX.DADD      =(uint32)(SPI3_RX_DATA) ;
           g_dmaCTRLPKT_SPI3_RX.CHCTRL    = 0;
           g_dmaCTRLPKT_SPI3_RX.FRCNT = SPI3_revice_SIZE;
           g_dmaCTRLPKT_SPI3_RX.ELCNT = 1;
           g_dmaCTRLPKT_SPI3_RX.ELDOFFSET = 0;
           g_dmaCTRLPKT_SPI3_RX.ELSOFFSET = 0;
           g_dmaCTRLPKT_SPI3_RX.FRDOFFSET = 0;
           g_dmaCTRLPKT_SPI3_RX.FRSOFFSET = 0;
           g_dmaCTRLPKT_SPI3_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
           g_dmaCTRLPKT_SPI3_RX.RDSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI3_RX.WRSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI3_RX.TTYPE     = FRAME_TRANSFER ;
           g_dmaCTRLPKT_SPI3_RX.ADDMODERD = ADDR_FIXED;
           g_dmaCTRLPKT_SPI3_RX.ADDMODEWR = ADDR_INC1;
           g_dmaCTRLPKT_SPI3_RX.AUTOINIT  = AUTOINIT_ON;

           dmaSetCtrlPacket(DMA_SPI3_REVICE_channel,g_dmaCTRLPKT_SPI3_RX);

           dmaSetChEnable(DMA_SPI3_REVICE_channel, DMA_HW);

           spiREG3->INT0 |=(0x01 << 16);
      break;
      case SPI4:
           while((spiREG4->FLG & 0x00000200U) != 0x00000200U);
           dmaReqAssign(DMA_SPI4_TRANSMIT_channel, DMA_SPI4_TRANSMIT_REQUEST_LINE);
           g_dmaCTRLPKT_SPI4_TX.SADD      = (uint32)(SPI4_TX_DATA) ;
           g_dmaCTRLPKT_SPI4_TX.DADD      = ((uint32_t)(&(spiREG4->DAT1))+3);
           g_dmaCTRLPKT_SPI4_TX.CHCTRL    = 0;
           g_dmaCTRLPKT_SPI4_TX.FRCNT = SPI4_send_SIZE;
           g_dmaCTRLPKT_SPI4_TX.ELCNT = 1;
           g_dmaCTRLPKT_SPI4_TX.ELDOFFSET = 0;
           g_dmaCTRLPKT_SPI4_TX.ELSOFFSET = 0;
           g_dmaCTRLPKT_SPI4_TX.FRDOFFSET = 0;
           g_dmaCTRLPKT_SPI4_TX.FRSOFFSET = 0;
           g_dmaCTRLPKT_SPI4_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
           g_dmaCTRLPKT_SPI4_TX.RDSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI4_TX.WRSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI4_TX.TTYPE     = FRAME_TRANSFER ;
           g_dmaCTRLPKT_SPI4_TX.ADDMODERD = ADDR_INC1;
           g_dmaCTRLPKT_SPI4_TX.ADDMODEWR = ADDR_FIXED;
           g_dmaCTRLPKT_SPI4_TX.AUTOINIT  = AUTOINIT_OFF;

           dmaSetCtrlPacket(DMA_SPI4_TRANSMIT_channel,g_dmaCTRLPKT_SPI4_TX);


           dmaReqAssign(DMA_SPI4_REVICE_channel, DMA_SPI4_REVICE_REQUEST_LINE);
           g_dmaCTRLPKT_SPI4_RX.SADD      =((uint32_t)(&(spiREG4->BUF))+3)  ;
           g_dmaCTRLPKT_SPI4_RX.DADD      =(uint32)(SPI4_RX_DATA) ;
           g_dmaCTRLPKT_SPI4_RX.CHCTRL    = 0;
           g_dmaCTRLPKT_SPI4_RX.FRCNT = SPI4_revice_SIZE;
           g_dmaCTRLPKT_SPI4_RX.ELCNT = 1;
           g_dmaCTRLPKT_SPI4_RX.ELDOFFSET = 0;
           g_dmaCTRLPKT_SPI4_RX.ELSOFFSET = 0;
           g_dmaCTRLPKT_SPI4_RX.FRDOFFSET = 0;
           g_dmaCTRLPKT_SPI4_RX.FRSOFFSET = 0;
           g_dmaCTRLPKT_SPI4_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
           g_dmaCTRLPKT_SPI4_RX.RDSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI4_RX.WRSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI4_RX.TTYPE     = FRAME_TRANSFER ;
           g_dmaCTRLPKT_SPI4_RX.ADDMODERD = ADDR_FIXED;
           g_dmaCTRLPKT_SPI4_RX.ADDMODEWR = ADDR_INC1;
           g_dmaCTRLPKT_SPI4_RX.AUTOINIT  = AUTOINIT_ON;

           dmaSetCtrlPacket(DMA_SPI4_REVICE_channel,g_dmaCTRLPKT_SPI4_RX);

           dmaSetChEnable(DMA_SPI4_REVICE_channel, DMA_HW);

           spiREG4->INT0 |=(0x01 << 16);
      break;
      case SPI5:
           while((spiREG5->FLG & 0x00000200U) != 0x00000200U);
           dmaReqAssign(DMA_SPI5_TRANSMIT_channel, DMA_SPI5_TRANSMIT_REQUEST_LINE);
           g_dmaCTRLPKT_SPI5_TX.SADD      = (uint32)(SPI5_TX_DATA) ;
           g_dmaCTRLPKT_SPI5_TX.DADD      = ((uint32_t)(&(spiREG5->DAT1))+3);
           g_dmaCTRLPKT_SPI5_TX.CHCTRL    = 0;
           g_dmaCTRLPKT_SPI5_TX.FRCNT = SPI5_send_SIZE;
           g_dmaCTRLPKT_SPI5_TX.ELCNT = 1;
           g_dmaCTRLPKT_SPI5_TX.ELDOFFSET = 0;
           g_dmaCTRLPKT_SPI5_TX.ELSOFFSET = 0;
           g_dmaCTRLPKT_SPI5_TX.FRDOFFSET = 0;
           g_dmaCTRLPKT_SPI5_TX.FRSOFFSET = 0;
           g_dmaCTRLPKT_SPI5_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
           g_dmaCTRLPKT_SPI5_TX.RDSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI5_TX.WRSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI5_TX.TTYPE     = FRAME_TRANSFER ;
           g_dmaCTRLPKT_SPI5_TX.ADDMODERD = ADDR_INC1;
           g_dmaCTRLPKT_SPI5_TX.ADDMODEWR = ADDR_FIXED;
           g_dmaCTRLPKT_SPI5_TX.AUTOINIT  = AUTOINIT_OFF;

           dmaSetCtrlPacket(DMA_SPI5_TRANSMIT_channel,g_dmaCTRLPKT_SPI5_TX);


           dmaReqAssign(DMA_SPI5_REVICE_channel, DMA_SPI5_REVICE_REQUEST_LINE);
           g_dmaCTRLPKT_SPI5_RX.SADD      =((uint32_t)(&(spiREG5->BUF))+3)  ;
           g_dmaCTRLPKT_SPI5_RX.DADD      =(uint32)(SPI5_RX_DATA) ;
           g_dmaCTRLPKT_SPI5_RX.CHCTRL    = 0;
           g_dmaCTRLPKT_SPI5_RX.FRCNT = SPI5_revice_SIZE;
           g_dmaCTRLPKT_SPI5_RX.ELCNT = 1;
           g_dmaCTRLPKT_SPI5_RX.ELDOFFSET = 0;
           g_dmaCTRLPKT_SPI5_RX.ELSOFFSET = 0;
           g_dmaCTRLPKT_SPI5_RX.FRDOFFSET = 0;
           g_dmaCTRLPKT_SPI5_RX.FRSOFFSET = 0;
           g_dmaCTRLPKT_SPI5_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
           g_dmaCTRLPKT_SPI5_RX.RDSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI5_RX.WRSIZE    = ACCESS_8_BIT;
           g_dmaCTRLPKT_SPI5_RX.TTYPE     = FRAME_TRANSFER ;
           g_dmaCTRLPKT_SPI5_RX.ADDMODERD = ADDR_FIXED;
           g_dmaCTRLPKT_SPI5_RX.ADDMODEWR = ADDR_INC1;
           g_dmaCTRLPKT_SPI5_RX.AUTOINIT  = AUTOINIT_ON;

           dmaSetCtrlPacket(DMA_SPI5_REVICE_channel,g_dmaCTRLPKT_SPI5_RX);

           dmaSetChEnable(DMA_SPI5_REVICE_channel, DMA_HW);

           spiREG5->INT0 |=(0x01 << 16);
      break;
      default:
      break;

      }
}
/******************
 * 函数：spiEndNotification(spiBASE_t *spi)
 * 功能：SPI 接收中断服务函数
 * 输入：spi ： spi地址。可选spiREG1、spiREG2、spiREG3、spiREG4、spiREG5.
 * 输出：无
 * *******************/
void spiEndNotification(spiBASE_t *spi)
{
    uint8_t SPI_Revice_data=0;
    if(spi==spiREG1)
    {
        SPI_Revice_data = (uint8)(spiREG1->BUF);
        SPI1_revive(SPI_Revice_data);

    }
    else if(spi==spiREG2)
    {
        SPI_Revice_data = (uint8)(spiREG2->BUF);
        SPI2_revive(SPI_Revice_data);

    }
    else if(spi==spiREG3)
    {
        SPI_Revice_data = (uint8)(spiREG3->BUF);
        SPI3_revive(SPI_Revice_data);

    }
    else if(spi==spiREG4)
    {
        SPI_Revice_data = (uint8)(spiREG4->BUF);
        SPI4_revive(SPI_Revice_data);

    }
    else if(spi==spiREG5)
    {
        SPI_Revice_data = (uint8)(spiREG5->BUF);
        SPI5_revive(SPI_Revice_data);

    }
}
