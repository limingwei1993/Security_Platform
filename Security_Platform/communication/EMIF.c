/*
 * EMIF.c
 *
 *  Created on: 2021年8月3日
 *      Author: 15755
 */
#include "EMIF.h"
#include "HL_reg_emif.h"
#include "HL_pinmux.h"
#include "HL_reg_pinmux.h"
/******************
 * 函数：void EMIF_IO_Configure(void)
 * 功能：EMIF的I/O口配置
 * 输入：：无。
 * 输出：  无。
 * *******************/
void EMIF_IO_Configure(void)
{

    /* Enable Pin Muxing */
        pinMuxReg->KICKER0 = 0x83E70B13U;
        pinMuxReg->KICKER1 = 0x95A4F1E0U;

        pinMuxReg->PINMUX[0]  =pinMuxReg->PINMUX[0] & (0x000000ff) | (0x01 << 8) |  (0x1 << 16) | (0x01 << 24) ; /*EMIF_ADDR[0] 、EMIF_ADDR[1]、 EMIF_ADDR[6]*/
        pinMuxReg->PINMUX[1]  =pinMuxReg->PINMUX[1] & (0x00000000) | (0x01 << 0) |  (0x1 << 8) | (0x01 << 16) | (0x01 << 24); /*EMIF_ADDR[7] 、EMIF_ADDR[8]、 EMIF_ADDR[9]、 EMIF_ADDR[10]*/
        pinMuxReg->PINMUX[2]  =pinMuxReg->PINMUX[2] & (0xffffff00) |  (0x01 << 0); /*EMIF_ADDR[11]*/
        pinMuxReg->PINMUX[8]  =pinMuxReg->PINMUX[8] & (0x00ffffff) |  (0x01 <<24); /*EMIF_BA[1] */
        pinMuxReg->PINMUX[9]  =pinMuxReg->PINMUX[9] & (0x00000000) |  (0x01 << 1) |  (0x1 << 8) | (0x01 << 16) | (0x01 << 24); /*EMIF_CLK、EMIF_nCAS 、EMIF_nCS[0]、 EMIF_nCS[2]*/
        pinMuxReg->PINMUX[10]  =pinMuxReg->PINMUX[10] & (0x0000fffff) | (0x01 << 16) | (0x01 << 24); /* EMIF_nRAS、 EMIF_nWAIT*/
        pinMuxReg->PINMUX[11]  =pinMuxReg->PINMUX[11] & (0x00000000) | (0x01 <<0) |  (0x1 << 9) | (0x01 << 17) | (0x01 << 25); /*EMIF_nWE、EMIF_ADDR[5]、 EMIF_ADDR[4]、 EMIF_ADDR[3] */
        pinMuxReg->PINMUX[12]  =pinMuxReg->PINMUX[12] & (0x00000000) | (0x01 <<1) |  (0x1 << 9) | (0x01 << 17) | (0x01 << 25); /*EMIF_ADDR[2]、EMIF_BA[0]、 EMIF_nOE、 EMIF_nDQM[1] */
        pinMuxReg->PINMUX[13]  =pinMuxReg->PINMUX[13] & (0x00000000) | (0x01 <<1) |  (0x1 << 9) | (0x01 << 17) | (0x01 << 25); /*EMIF_nDQM[0]、EMIF_DATA[0]、 EMIF_DATA[1]、 EMIF_DATA[2] */
        pinMuxReg->PINMUX[14]  =pinMuxReg->PINMUX[14] & (0x00000000) | (0x01 <<1) |  (0x1 << 9) | (0x01 << 17) | (0x01 << 25); /*EMIF_nDQM[3]、EMIF_DATA[4]、 EMIF_DATA[5]、 EMIF_DATA[6] */
        pinMuxReg->PINMUX[15]  =pinMuxReg->PINMUX[15] & (0x00000000) | (0x01 <<1) |  (0x1 << 9) | (0x01 << 17) | (0x01 << 25); /*EMIF_nDQM[7]、EMIF_DATA[8]、 EMIF_DATA[9]、 EMIF_DATA[10] */
        pinMuxReg->PINMUX[16]  =pinMuxReg->PINMUX[16] & (0x00000000) | (0x01 <<1) | (0x1 << 9) | (0x01 << 17) | (0x01 << 25); /*EMIF_DATA[11]、EMIF_DATA[12]、 EMIF_DATA[13]、 EMIF_DATA[14] */
        pinMuxReg->PINMUX[17]  =pinMuxReg->PINMUX[17] & (0xffffff00) | (0x01 <<1); /*EMIF_DAT[15] */
        pinMuxReg->PINMUX[32]  =pinMuxReg->PINMUX[32] & (0x000000ff) | (0x1 << 9) | (0x01 << 17) |  (0x01 << 25); /*EMIF_RNW、EMIF_nDQM[0]、EMIF_nDQM[1] */

        pinMuxReg->PINMUX[9] = (pinMuxReg->PINMUX[9] &(~(uint32)((uint32)0xFFU << 0U))) | ((uint32)((uint32)0x2U <<  0U));  /*enable EMIF Output*/
        pinMuxReg->PINMUX[174] = (pinMuxReg->PINMUX[174] &(~(uint32)((uint32)0xFFU << 8U))) | ((uint32)((uint32)0x2U <<  8U));  /*enable EMIF Output*/
        /* Disable Pin Muxing */
        pinMuxReg->KICKER0 = 0x00000000U;
        pinMuxReg->KICKER1 = 0x00000000U;

}
/******************
 * 函数：void EMIF_init(void)
 * 功能：EMIF初始化
 * 输入：：无。
 * 输出：  无。
 * *******************/
void EMIF_init(void)
{
        volatile uint32 buffer;

        /* Procedure B Step 1:  EMIF Clock Frequency is assumed to be configured in the startup */

        /* Procedure B  Step 2:  Program SDTIMR and SDSRETR to satisfy requirements of SDRAM Device */
        emifREG->SDTIMR  = (uint32)((uint32)4U << 27U)|     /*T_RFC*/
                           (uint32)((uint32)1U << 24U)|     /*T_RP*/
                           (uint32)((uint32)0U << 23U)|     /*RESERVED*/
                           (uint32)((uint32)1U << 20U)|     /*T_RCD*/
                           (uint32)((uint32)0U << 19U)|     /*RESERVED*/
                           (uint32)((uint32)1U << 16U)|     /*T_WR*/
                           (uint32)((uint32)3U << 12U)|     /*T_PAS*/
                           (uint32)((uint32)4U << 8U)|      /*T_RC*/
                           (uint32)((uint32)0U << 7U)|      /*RESERVED*/
                           (uint32)((uint32)1U << 4U)|      /*T_RRD*/
                           (uint32)((uint32)0U << 3U);      /*RESERVED*/

        emifREG->SDSRETR = (uint32)5U; /*T_XS*/

        /* Procedure B  Step 3:  Program the RR Field of SDRCR to provide 200us of initialization time */
        emifREG->SDRCR   = 1605U;  /*RR*/

        /* Procedure B  Step 4:  Program SDRCR to Trigger Initialization Sequence */
        /**  -general clearing of register
        *    -for NM for setting 16 bit data bus
        *    -cas latency
        *    -BIT11_9CLOCK to allow the cl field to be written
        *    -selecting the banks
        *    -setting the pagesize
        */
        emifREG->SDCR   = (uint32)((uint32)0U << 31U)|      /* 0--exit the self-refresh mode ,1--enter the self-refresh mode*/
                          (uint32)((uint32)1U << 14U)|      /*0--32 bit SDRAM data bus is used,1--16 bit*/
                          (uint32)((uint32)2U << 9U)|       /*CAS Latency: 2--2 EMIF_CLK cycles,3--3 EMIF_CLK cycles*/
                          (uint32)((uint32)1U << 8U)|       /*set CAS Latency enable: 0--disable,1--enable*/
                          (uint32)((uint32)2U << 4U)|       /*SDRAM NUM:0--1 bank SDRAM devices, 1--2 bank, 2--4 bank */
                          (uint32)((uint32)elements_256);   /*Page Size:0--256,1--512,2--1024,3--2048*/

        /* Procedure B  Step 5:  Read of SDRAM memory location causes processor to wait until SDRAM Initialization completes */
        buffer           = *EMIF_BASE_ADDRESS;
        /* prevents optimization */
        buffer           = buffer;

        /* Procedure B  Step 6:  Program the RR field to the default Refresh Interval of the SDRAM*/
        emifREG->SDRCR   = 1171U; /*RR*/

        /* Place the EMIF in Self Refresh Mode For Clock Change          */
        /* Must only write to the upper byte of the SDCR to avoid        */
        /* a second intiialization sequence                              */
        /* The byte address depends on endian (0x3U in LE, 0x00 in BE32) */
           *((unsigned char *)(&emifREG->SDCR) + 0x0U) = 0x80U;
}
/******************
 * 函数：void EMIF_write(uint32 write_addr,uint8 *write_buff,uint32 write_len)
 * 功能：EMIF写数据
 * 输入：：write_addr：写数据的地址。
 *       write_buff：数据
 *       write_len：数据长度
 * 输出：  无。
 * *******************/
void EMIF_write(uint32 write_addr,uint8 *write_buff,uint32 write_len)
{
   uint8* DATA_ADDR=(uint8 *)(EMIF_BASE_ADDRESS)+write_addr;
   uint32 i=0;
   for(i=0;i<write_len;i++)
   {
       * DATA_ADDR=write_buff[i];
       DATA_ADDR++;
   }

}
/******************
 * 函数：void EMIF_read(uint32 read_addr,uint8 * read_buff,uint32 read_len)
 * 功能：EMIF读数据
 * 输入：：read_addr：读取数据的地址。
 *       read_buff：保存读取到的数据
 *       read_len：要读的数据长度
 * 输出：  无。
 * *******************/
void EMIF_read(uint32 read_addr,uint8 * read_buff,uint32 read_len)
{
   uint8* DATA_ADDR=(uint8 *)(EMIF_BASE_ADDRESS)+read_addr;
   uint32 i=0;
   for(i=0;i<read_len;i++)
   {
       read_buff[i]=* DATA_ADDR;
       DATA_ADDR++;
   }

}
