/*
 * IIC.c
 *
 *  Created on: 2021年7月30日
 *      Author: 15755
 */

#include"IIC.h"
#include"HL_reg_i2c.h"
#include"HL_i2c.h"
#include "HL_reg_dma.h"
#include "HL_sys_dma.h"


bool DMA_IIC1_TX=true;  /*IIC1 发送数据启用DMA标志*/
bool DMA_IIC2_TX=false; /*IIC2 发送数据启用DMA标志*/
bool DMA_IIC1_RX=true;  /*IIC1 接收数据启用DMA标志*/
bool DMA_IIC2_RX=true;  /*IIC2 接收数据启用DMA标志*/

/******************
 * 函数：void IIC_init(IIC_Info iicx)
 * 功能：IIC 初始化
 * 输入：iicx：配置参数句柄。->ch：IIC的编号；可选：IIC1、IIC2。
 *                    ->ticks:速率
 * 输出：无
 * *******************/
void IIC_init(IIC_Info iicx)
{
    uint32 prescale;
       uint32 d;
       uint32 ck;
       float64 vclk = 75.000F * 1000000.0F;
       float64 divider= 0.0F;
       uint32 temp = 0U;
       divider = vclk / 8000000.0F;
       prescale = (uint32)divider - 1U;

       if(prescale>=2U)
       {
           d = 5U;
       }
       else
       {
           d = (prescale != 0U) ? 6U : 7U;
       }

       temp = 2U * iicx.ticks * 1000U * (prescale + 1U);
       divider = vclk / ((float64)temp);
       ck = (uint32)divider - d;

    switch(iicx.ch)
    {
        case IIC1:
            i2cREG1->MDR = (uint32)((uint32)0U << 5U);

                /** - set i2c mode */
                i2cREG1->MDR =   (uint32)((uint32)0U << 15U)     /* nack mode  */
                               | (uint32)((uint32)0U << 14U)     /* free running */
                               | (uint32)((uint32)0U << 13U)     /* start condition - master mode only */
                               | (uint32)((uint32)1U <<11U)     /* stop condition   */
                               | (uint32)((uint32)1U <<10U)     /* Master/Slave mode  */
                               | (uint32)((uint32)I2C_TRANSMITTER)     /* Transmitter/receiver */
                               | (uint32)((uint32)I2C_10BIT_AMODE)     /* xpanded address */
                               | (uint32)((uint32)0U << 7U)      /* repeat mode */
                               | (uint32)((uint32)0U << 6U)     /* digital loopback */
                               | (uint32)((uint32)0U << 4U)     /* start byte - master only */
                               | (uint32)((uint32)0U << 3U)     /* free data format */
                               | (uint32)((uint32)I2C_8_BIT);     /* bit count */

                /** - set i2c extended mode */
                i2cREG1->EMDR = (uint32)0U << 1U; /* Ignore Nack Enable/Disable */

                /** - set i2c Backward Compatibility mode */
                i2cREG1->EMDR |= 0U;

                /** - Disable DMA */
                i2cREG1->DMACR = 0x00U;

                /** - set i2c data count */
                i2cREG1->CNT = 16U;

                /** - disable all interrupts */
                i2cREG1->IMR = 0x00U;

                /** - set prescale */
                i2cREG1->PSC = prescale;

                /** - set clock rate */
                i2cREG1->CKH = ck;
                i2cREG1->CKL = ck;

                /** - set i2c pins functional mode */
                i2cREG1->PFNC = (0U);

                /** - set i2c pins default output value */
                i2cREG1->DOUT = (uint32)((uint32)0U << 1U)     /* sda pin */
                              | (uint32)(0U);     /* scl pin */

                /** - set i2c pins output direction */
                i2cREG1->DIR = (uint32)((uint32)0U << 1U)     /* sda pin */
                             | (uint32)(0U);     /* scl pin */

                /** - set i2c pins open drain enable */
                i2cREG1->PDR = (uint32)((uint32)0U << 1U)     /* sda pin */
                             | (uint32)(0U);     /* scl pin */

                /** - set i2c pins pullup/pulldown enable */
                i2cREG1->PDIS = (uint32)((uint32)0U << 1U)     /* sda pin */
                            | (uint32)(0U);     /* scl pin */

                /** - set i2c pins pullup/pulldown select */
                i2cREG1->PSEL = (uint32)((uint32)1U << 1U)     /* sda pin */
                             | (uint32)(1U);     /* scl pin */

                /** - set interrupt enable */
                i2cREG1->IMR    = (uint32)((uint32)0U << 6U)     /* Address as slave interrupt      */
                                | (uint32)((uint32)0U << 5U)     /* Stop Condition detect interrupt */
                                | (uint32)((uint32)0U << 4U)     /* Transmit data ready interrupt   */
                                | (uint32)((uint32)1U << 3U)     /* Receive data ready interrupt    */
                                | (uint32)((uint32)0U << 2U)     /* Register Access ready interrupt */
                                | (uint32)((uint32)0U << 1U)     /* No Acknowledgment interrupt    */
                                | (uint32)((uint32)0U);     /* Arbitration Lost interrupt      */

                i2cREG1->MDR |= (uint32)I2C_RESET_OUT; /* i2c out of reset */

                /** - initialize global transfer variables */
                g_i2cTransfer_t[0U].mode   = (uint32)0U << 4U;
                g_i2cTransfer_t[0U].length = 0U;

                /** @b initialize @b I2C2 */
            break;
            case IIC2:
                /** - i2c Enter reset */
                i2cREG2->MDR = (uint32)((uint32)0U << 5U);

                /** - set i2c mode */
                i2cREG2->MDR =   (uint32)((uint32)0U << 15U)     /* nack mode  */
                               | (uint32)((uint32)0U << 14U)     /* free running */
                               | (uint32)((uint32)0U << 13U)     /* start condition - master mode only */
                               | (uint32)((uint32)1U <<11U)     /* stop condition   */
                               | (uint32)((uint32)1U <<10U)     /* Master/Slave mode  */
                               | (uint32)((uint32)I2C_TRANSMITTER)     /* Transmitter/receiver */
                               | (uint32)((uint32)I2C_10BIT_AMODE)     /* Expanded address */
                               | (uint32)((uint32)0 << 7U)      /* repeat mode */
                               | (uint32)((uint32)0U << 6U)     /* digital loopback */
                               | (uint32)((uint32)0U << 4U)     /* start byte - master only */
                               | (uint32)((uint32)0U << 3U)     /* free data format */
                               | (uint32)(I2C_8_BIT);     /* bit count */

                /** - set i2c extended mode */
                i2cREG2->EMDR = (uint32)0U << 1U; /* Ignore Nack Enable/Disable */

                /** - set i2c Backward Compatibility mode */
                i2cREG2->EMDR |= 0U;

                /** - Disable DMA */
                i2cREG2->DMACR = 0x00U;

                /** - set i2c data count */
                i2cREG2->CNT = 16U;

                /** - disable all interrupts */
                i2cREG2->IMR = 0x00U;

                /** - set prescale */
                i2cREG2->PSC = prescale;

                /** - set clock rate */
                i2cREG2->CKH = ck;
                i2cREG2->CKL = ck;

                /** - set i2c pins functional mode */
                i2cREG2->PFNC = (0U);

                /** - set i2c pins default output value */
                i2cREG2->DOUT = (uint32)((uint32)0U << 1U)     /* sda pin */
                              | (uint32)(0U);     /* scl pin */

                /** - set i2c pins output direction */
                i2cREG2->DIR = (uint32)((uint32)0U << 1U)     /* sda pin */
                             | (uint32)(0U);     /* scl pin */

                /** - set i2c pins open drain enable */
                i2cREG2->PDR = (uint32)((uint32)0U << 1U)     /* sda pin */
                             | (uint32)(0U);     /* scl pin */

                /** - set i2c pins pullup/pulldown enable */
                i2cREG2->PDIS = (uint32)((uint32)0U << 1U)     /* sda pin */
                            | (uint32)(0U);     /* scl pin */

                /** - set i2c pins pullup/pulldown select */
                i2cREG2->PSEL = (uint32)((uint32)1U << 1U)     /* sda pin */
                             | (uint32)(1U);     /* scl pin */

                /** - set interrupt enable */
                i2cREG2->IMR    = (uint32)((uint32)0U << 6U)     /* Address as slave interrupt      */
                                | (uint32)((uint32)0U << 5U)     /* Stop Condition detect interrupt */
                                | (uint32)((uint32)0U << 4U)     /* Transmit data ready interrupt   */
                                | (uint32)((uint32)0U << 3U)     /* Receive data ready interrupt    */
                                | (uint32)((uint32)0U << 2U)     /* Register Access ready interrupt */
                                | (uint32)((uint32)0U << 1U)     /* No Acknowledgment interrupt    */
                                | (uint32)(0U);     /* Arbitration Lost interrupt      */

                i2cREG2->MDR |= (uint32)I2C_RESET_OUT; /* i2c out of reset */

                /** - initialize global transfer variables */
                g_i2cTransfer_t[1U].mode   = (uint32)0U << 4U;
                g_i2cTransfer_t[1U].length = 0U;
        break;
        default:
        break;

    }
}

/******************
 * 函数：void ICC_DMA_init(IIC_Info iicx)
 * 功能：IIC DMA初始化
 * 输入：iicx：配置参数句柄。->ch：IIC的编号；可选：IIC1、IIC2。
 *                    ->ticks:速率
 * 输出：无
 * *******************/
void ICC_DMA_init(IIC_Info iicx)
{
    uint32 IIC_TX_DATA=0;
    uint32 IIC_RX_DATA=0;
    g_dmaCTRL g_dmaCTRLPKT_IIC1_TX;             /* dma control packet configuration stack */
    g_dmaCTRL g_dmaCTRLPKT_IIC1_RX;             /* dma control packet configuration stack */
    g_dmaCTRL g_dmaCTRLPKT_IIC2_TX;             /* dma control packet configuration stack */
    g_dmaCTRL g_dmaCTRLPKT_IIC2_RX;             /* dma control packet configuration stack */
    switch(iicx.ch)
    {
    case IIC1:
         dmaReqAssign(DMA_IIC1_TRANSMIT_channel, DMA_IIC1_TRANSMIT_REQUEST_LINE);
         g_dmaCTRLPKT_IIC1_TX.SADD      = (uint32)(IIC_TX_DATA) ;
         g_dmaCTRLPKT_IIC1_TX.DADD      = ((uint32_t)(&(i2cREG1->DXR))+3);
         g_dmaCTRLPKT_IIC1_TX.CHCTRL    = 0;
         g_dmaCTRLPKT_IIC1_TX.FRCNT = 0;
         g_dmaCTRLPKT_IIC1_TX.ELCNT = 0;
         g_dmaCTRLPKT_IIC1_TX.ELDOFFSET = 0;
         g_dmaCTRLPKT_IIC1_TX.ELSOFFSET = 0;
         g_dmaCTRLPKT_IIC1_TX.FRDOFFSET = 0;
         g_dmaCTRLPKT_IIC1_TX.FRSOFFSET = 0;
         g_dmaCTRLPKT_IIC1_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
         g_dmaCTRLPKT_IIC1_TX.RDSIZE    = ACCESS_8_BIT;
         g_dmaCTRLPKT_IIC1_TX.WRSIZE    = ACCESS_8_BIT;
         g_dmaCTRLPKT_IIC1_TX.TTYPE     = FRAME_TRANSFER ;
         g_dmaCTRLPKT_IIC1_TX.ADDMODERD = ADDR_INC1;
         g_dmaCTRLPKT_IIC1_TX.ADDMODEWR = ADDR_FIXED;
         g_dmaCTRLPKT_IIC1_TX.AUTOINIT  = AUTOINIT_OFF;

         dmaSetCtrlPacket(DMA_IIC1_TRANSMIT_channel,g_dmaCTRLPKT_IIC1_TX);

        dmaReqAssign(DMA_IIC1_REVICE_channel, DMA_IIC1_REVICE_REQUEST_LINE);
        g_dmaCTRLPKT_IIC1_RX.SADD      =((uint32_t)(&(i2cREG1->DRR))+3)  ;     /* source address             */
        g_dmaCTRLPKT_IIC1_RX.DADD      =(uint32)(IIC_RX_DATA) ;                       /* destination  address       */
        g_dmaCTRLPKT_IIC1_RX.CHCTRL    = 0;
        g_dmaCTRLPKT_IIC1_RX.FRCNT = 0;
        g_dmaCTRLPKT_IIC1_RX.ELCNT = 0;                                     /* element destination offset */
        g_dmaCTRLPKT_IIC1_RX.ELDOFFSET = 0;                                     /* element destination offset */
        g_dmaCTRLPKT_IIC1_RX.ELSOFFSET = 0;                                     /* element destination offset */
        g_dmaCTRLPKT_IIC1_RX.FRDOFFSET = 0;                                     /* frame destination offset   */
        g_dmaCTRLPKT_IIC1_RX.FRSOFFSET = 0;                                     /* frame destination offset   */
        g_dmaCTRLPKT_IIC1_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
        g_dmaCTRLPKT_IIC1_RX.RDSIZE    = ACCESS_8_BIT;                         /* read size                  */
        g_dmaCTRLPKT_IIC1_RX.WRSIZE    = ACCESS_8_BIT;                         /* write size                 */
        g_dmaCTRLPKT_IIC1_RX.TTYPE     = FRAME_TRANSFER ;                       /* transfer type              */
        g_dmaCTRLPKT_IIC1_RX.ADDMODERD = ADDR_FIXED;                           /* address mode read          */
        g_dmaCTRLPKT_IIC1_RX.ADDMODEWR = ADDR_INC1;                             /* address mode write         */
        g_dmaCTRLPKT_IIC1_RX.AUTOINIT  = AUTOINIT_OFF;                           /* auto init                  */

        dmaSetCtrlPacket(DMA_IIC1_REVICE_channel,g_dmaCTRLPKT_IIC1_RX);

        i2cREG1->DMACR |= IIC_SET_TX_DMA | IIC_SET_RX_DMA;
    break;
    case IIC2:
             dmaReqAssign(DMA_IIC2_TRANSMIT_channel, DMA_IIC2_TRANSMIT_REQUEST_LINE);
             g_dmaCTRLPKT_IIC2_TX.SADD      = (uint32)(IIC_TX_DATA) ;
             g_dmaCTRLPKT_IIC2_TX.DADD      = ((uint32_t)(&(i2cREG2->DXR))+3);
             g_dmaCTRLPKT_IIC2_TX.CHCTRL    = 0;
             g_dmaCTRLPKT_IIC2_TX.FRCNT = 0;
             g_dmaCTRLPKT_IIC2_TX.ELCNT = 0;
             g_dmaCTRLPKT_IIC2_TX.ELDOFFSET = 0;
             g_dmaCTRLPKT_IIC2_TX.ELSOFFSET = 0;
             g_dmaCTRLPKT_IIC2_TX.FRDOFFSET = 0;
             g_dmaCTRLPKT_IIC2_TX.FRSOFFSET = 0;
             g_dmaCTRLPKT_IIC2_TX.PORTASGN  = PORTA_READ_PORTB_WRITE;
             g_dmaCTRLPKT_IIC2_TX.RDSIZE    = ACCESS_8_BIT;
             g_dmaCTRLPKT_IIC2_TX.WRSIZE    = ACCESS_8_BIT;
             g_dmaCTRLPKT_IIC2_TX.TTYPE     = FRAME_TRANSFER ;
             g_dmaCTRLPKT_IIC2_TX.ADDMODERD = ADDR_INC1;
             g_dmaCTRLPKT_IIC2_TX.ADDMODEWR = ADDR_FIXED;
             g_dmaCTRLPKT_IIC2_TX.AUTOINIT  = AUTOINIT_OFF;

             dmaSetCtrlPacket(DMA_IIC2_TRANSMIT_channel,g_dmaCTRLPKT_IIC2_TX);

            dmaReqAssign(DMA_IIC2_REVICE_channel, DMA_IIC2_REVICE_REQUEST_LINE);
            g_dmaCTRLPKT_IIC2_RX.SADD      =((uint32_t)(&(i2cREG2->DRR))+3)  ;     /* source address             */
            g_dmaCTRLPKT_IIC2_RX.DADD      =(uint32)(IIC_RX_DATA) ;                       /* destination  address       */
            g_dmaCTRLPKT_IIC2_RX.CHCTRL    = 0;
            g_dmaCTRLPKT_IIC2_RX.FRCNT = 0;
            g_dmaCTRLPKT_IIC2_RX.ELCNT = 0;                                     /* element destination offset */
            g_dmaCTRLPKT_IIC2_RX.ELDOFFSET = 0;                                     /* element destination offset */
            g_dmaCTRLPKT_IIC2_RX.ELSOFFSET = 0;                                     /* element destination offset */
            g_dmaCTRLPKT_IIC2_RX.FRDOFFSET = 0;                                     /* frame destination offset   */
            g_dmaCTRLPKT_IIC2_RX.FRSOFFSET = 0;                                     /* frame destination offset   */
            g_dmaCTRLPKT_IIC2_RX.PORTASGN  = PORTB_READ_PORTA_WRITE;
            g_dmaCTRLPKT_IIC2_RX.RDSIZE    = ACCESS_8_BIT;                         /* read size                  */
            g_dmaCTRLPKT_IIC2_RX.WRSIZE    = ACCESS_8_BIT;                         /* write size                 */
            g_dmaCTRLPKT_IIC2_RX.TTYPE     = FRAME_TRANSFER ;                       /* transfer type              */
            g_dmaCTRLPKT_IIC2_RX.ADDMODERD = ADDR_FIXED;                           /* address mode read          */
            g_dmaCTRLPKT_IIC2_RX.ADDMODEWR = ADDR_INC1;                             /* address mode write         */
            g_dmaCTRLPKT_IIC2_RX.AUTOINIT  = AUTOINIT_OFF;                           /* auto init                  */

            dmaSetCtrlPacket(DMA_IIC2_REVICE_channel,g_dmaCTRLPKT_IIC2_RX);

            i2cREG2->DMACR |= IIC_SET_TX_DMA | IIC_SET_RX_DMA;
        break;
        default:
        break;
    }

}
/******************
 * 函数：void IIC_ADD8_write(IIC_Info iicx, uint8 addr, uint8* buff, uint32 len)
 * 功能：IIC 8位地址发送数据
 * 输入：iicx：配置参数句柄。->ch：IIC的编号；可选：IIC1、IIC2。
 *                    ->ticks:速率
 *      addr：发送数据地址。
 *      buff：发送的数据。
 *      len：发送的数据长度。
 * 输出：无
 * *******************/
void IIC_ADD8_write(IIC_Info iicx, uint8 addr, uint8* buff, uint32 len)
{
    uint32 i=0;
    uint8 IIC_TX_DATA[100]={0};
    switch(iicx.ch)
    {
        case IIC1:
            if(DMA_IIC1_TX==true)
            {
                i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);  /*添加从机地址*/
                i2cSetDirection(i2cREG1, I2C_TRANSMITTER);/*设置通信方向为发送*/
                i2cSetCount(i2cREG1, 1+len);              /*设置发送数据长度*/
                i2cSetMode(i2cREG1, I2C_MASTER);          /*设置主从模式*/
                i2cSetStop(i2cREG1);
                i2cSetStart(i2cREG1);
                IIC_TX_DATA[0]=addr;
                for(i=0;i<len;i++)
                {
                    if(i<100)
                    {
                        IIC_TX_DATA[i+1]=buff[i];
                    }
                }
 //               while ((dmaREG->PEND & (1<<DMA_IIC1_TRANSMIT_channel))==0);
                dmaRAMREG->PCP[DMA_IIC1_TRANSMIT_channel].ISADDR  = (uint32_t)(IIC_TX_DATA)+1;
                dmaRAMREG->PCP[DMA_IIC1_TRANSMIT_channel].ITCOUNT = ((len) << 16U) | 1;
                dmaSetChEnable(DMA_IIC1_TRANSMIT_channel, DMA_HW);
                i2cREG1->DXR=(uint32)IIC_TX_DATA[0];
                while(i2cIsBusBusy(i2cREG1) == true);
                while(i2cIsStopDetected(i2cREG1) == 0);
                i2cClearSCD(i2cREG1);
            }
            else
            {
                i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);
                i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
                i2cSetCount(i2cREG1, 1+len);
                i2cSetMode(i2cREG1, I2C_MASTER);
                i2cSetStop(i2cREG1);
                i2cSetStart(i2cREG1);
                while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                {
                } /* Wait */
                i2cREG1->DXR = (uint32)addr;
                for(i=0;i<len;i++)
                {
                    while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                    {
                    } /* Wait */
                    i2cREG1->DXR = (uint32)buff[i];
                }
                while(i2cIsBusBusy(i2cREG1) == true);
                while(i2cIsStopDetected(i2cREG1) == 0);
                i2cClearSCD(i2cREG1);
            }

        break;
        case IIC2:
               if(DMA_IIC2_TX==true)
               {
                   i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);
                   i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
                   i2cSetCount(i2cREG2, 1+len);
                   i2cSetMode(i2cREG2, I2C_MASTER);
                   i2cSetStop(i2cREG2);
                   i2cSetStart(i2cREG2);
                   IIC_TX_DATA[0]=addr;
                   IIC_TX_DATA[0]=addr;
                   for(i=0;0<len;i++)
                   {
                       if(i<100)
                       {
                           IIC_TX_DATA[i+1]=buff[i];
                       }
                   }
                //   while ((dmaREG->PEND & (1<<DMA_IIC2_TRANSMIT_channel))==0);
                   dmaRAMREG->PCP[DMA_IIC2_TRANSMIT_channel].ISADDR  = (uint32_t)(IIC_TX_DATA)+1;
                   dmaRAMREG->PCP[DMA_IIC2_TRANSMIT_channel].ITCOUNT = ((len) << 16U) | 1;
                   dmaSetChEnable(DMA_IIC2_TRANSMIT_channel, DMA_HW);
                   i2cREG2->DXR=(uint32)IIC_TX_DATA[0];
                   while(i2cIsBusBusy(i2cREG2) == true);
                   while(i2cIsStopDetected(i2cREG2) == 0);
                   i2cClearSCD(i2cREG2);
               }
               else
               {
                   i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);
                   i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
                   i2cSetCount(i2cREG2, 1+len);
                   i2cSetMode(i2cREG2, I2C_MASTER);
                   i2cSetStop(i2cREG2);
                   i2cSetStart(i2cREG2);
                   while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                   {
                   } /* Wait */
                   i2cREG2->DXR = (uint32)addr;
                   for(i=0;i<len;i++)
                   {
                       while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                       {
                       } /* Wait */
                       i2cREG2->DXR = (uint32)buff[i];
                   }
                   while(i2cIsBusBusy(i2cREG2) == true);
                   while(i2cIsStopDetected(i2cREG2) == 0);
                   i2cClearSCD(i2cREG2);
               }

        break;
        default:
        break;
    }
}
/******************
 * 函数：void IIC_ADD16_write(IIC_Info iicx, uint16 addr, uint8* buff, uint32 len)
 * 功能：IIC 16位地址发送数据
 * 输入：iicx：配置参数句柄。->ch：IIC的编号；可选：IIC1、IIC2。
 *                    ->ticks:速率
 *      addr：发送数据地址。
 *      buff：发送的数据。
 *      len：发送的数据长度。
 * 输出：无
 * *******************/
void IIC_ADD16_write(IIC_Info iicx, uint16 addr, uint8* buff, uint32 len)
{
    uint32 i=0;
    uint8 IIC_TX_DATA[100]={0};
    switch(iicx.ch)
    {
        case IIC1:
            if(DMA_IIC1_TX==true)
            {

                i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);
                i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
                i2cSetCount(i2cREG1, 2+len);
                i2cSetMode(i2cREG1, I2C_MASTER);
                i2cSetStop(i2cREG1);
                i2cSetStart(i2cREG1);
                IIC_TX_DATA[0]=(uint8)(addr>>8);
                IIC_TX_DATA[1]=(uint8)(addr);
                for(i=0;0<len;i++)
                {
                    if(i<100)
                    {
                        IIC_TX_DATA[i+2]=buff[i];
                    }
                }
            //    while ((dmaREG->PEND & (1<<DMA_IIC1_TRANSMIT_channel))==0);
                dmaRAMREG->PCP[DMA_IIC1_TRANSMIT_channel].ISADDR  = (uint32_t)(IIC_TX_DATA)+1;
                dmaRAMREG->PCP[DMA_IIC1_TRANSMIT_channel].ITCOUNT = ((len+1) << 16U) | 1;
                dmaSetChEnable(DMA_IIC1_TRANSMIT_channel, DMA_HW);
                i2cREG1->DXR=(uint32)IIC_TX_DATA[0];
                while(i2cIsBusBusy(i2cREG1) == true);
                while(i2cIsStopDetected(i2cREG1) == 0);
                i2cClearSCD(i2cREG1);
            }
            else
            {

                i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);
                i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
                i2cSetCount(i2cREG1, 2+len);
                i2cSetMode(i2cREG1, I2C_MASTER);
                i2cSetStop(i2cREG1);
                i2cSetStart(i2cREG1);
                while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                {
                } /* Wait */
                i2cREG1->DXR = (uint32)(addr>>8);
                while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                {
                } /* Wait */
                i2cREG1->DXR = (uint32)(addr&0xff);
                for(i=0;i<len;i++)
                {
                    while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                    {
                    } /* Wait */
                    i2cREG1->DXR = (uint32)buff[i];
                }
                while(i2cIsBusBusy(i2cREG1) == true);
                while(i2cIsStopDetected(i2cREG1) == 0);
                i2cClearSCD(i2cREG1);
            }

        break;
        case IIC2:
               if(DMA_IIC2_TX==true)
               {

                   i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);
                   i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
                   i2cSetCount(i2cREG2, 2+len);
                   i2cSetMode(i2cREG2, I2C_MASTER);
                   i2cSetStop(i2cREG2);
                   i2cSetStart(i2cREG2);
                   IIC_TX_DATA[0]=(uint8)(addr>>8);
                   IIC_TX_DATA[1]=(uint8)(addr);
                   for(i=0;0<len;i++)
                   {
                       if(i<100)
                       {
                           IIC_TX_DATA[i+2]=buff[i];
                       }
                   }
            //       while ((dmaREG->PEND & (1<<DMA_IIC2_TRANSMIT_channel))==0);
                   dmaRAMREG->PCP[DMA_IIC2_TRANSMIT_channel].ISADDR  = (uint32_t)(IIC_TX_DATA)+1;
                   dmaRAMREG->PCP[DMA_IIC2_TRANSMIT_channel].ITCOUNT = ((len+1) << 16U) | 1;
                   dmaSetChEnable(DMA_IIC2_TRANSMIT_channel, DMA_HW);
                   i2cREG2->DXR=(uint32)IIC_TX_DATA[0];
                   while(i2cIsBusBusy(i2cREG2) == true);
                   while(i2cIsStopDetected(i2cREG2) == 0);
                   i2cClearSCD(i2cREG2);
               }
               else
               {

                   i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);
                   i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
                   i2cSetCount(i2cREG2, 2+len);
                   i2cSetMode(i2cREG2, I2C_MASTER);
                   i2cSetStop(i2cREG2);
                   i2cSetStart(i2cREG2);
                   while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                   {
                   } /* Wait */
                   i2cREG2->DXR = (uint32)(addr>>8);
                   while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                   {
                   } /* Wait */
                   i2cREG2->DXR = (uint32)(addr&0xff);
                   for(i=0;i<len;i++)
                   {
                       while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                       {
                       } /* Wait */
                       i2cREG2->DXR = (uint32)buff[i];
                   }
                   while(i2cIsBusBusy(i2cREG2) == true);
                   while(i2cIsStopDetected(i2cREG2) == 0);
                   i2cClearSCD(i2cREG2);
               }

        break;
        default:
        break;
    }
}
/******************
 * 函数：void IIC_ADD32_write(IIC_Info iicx, uint16 addr, uint8* buff, uint32 len)
 * 功能：IIC 32位地址发送数据
 * 输入：iicx：配置参数句柄。->ch：IIC的编号；可选：IIC1、IIC2。
 *                    ->ticks:速率
 *      addr：发送数据地址。
 *      buff：发送的数据。
 *      len：发送的数据长度。
 * 输出：无
 * *******************/
void IIC_ADD32_write(IIC_Info iicx, uint32 addr, uint8* buff, uint32 len)
{
    uint32 i=0;
    uint8 IIC_TX_DATA[100]={0};
    switch(iicx.ch)
    {
        case IIC1:
            if(DMA_IIC1_TX==true)
            {

                i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);
                i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
                i2cSetCount(i2cREG1, 4+len);
                i2cSetMode(i2cREG1, I2C_MASTER);
                i2cSetStop(i2cREG1);
                i2cSetStart(i2cREG1);
                IIC_TX_DATA[0]=(uint8)(addr>>24);
                IIC_TX_DATA[1]=(uint8)(addr>>16);
                IIC_TX_DATA[2]=(uint8)(addr>>8);
                IIC_TX_DATA[3]=(uint8)(addr);
                for(i=0;0<len;i++)
                {
                    if(i<100)
                    {
                        IIC_TX_DATA[i+4]=buff[i];
                    }
                }
          //      while ((dmaREG->PEND & (1<<DMA_IIC1_TRANSMIT_channel))==0);
                dmaRAMREG->PCP[DMA_IIC1_TRANSMIT_channel].ISADDR  = (uint32_t)(IIC_TX_DATA)+1;
                dmaRAMREG->PCP[DMA_IIC1_TRANSMIT_channel].ITCOUNT = ((len+3) << 16U) | 1;
                dmaSetChEnable(DMA_IIC1_TRANSMIT_channel, DMA_HW);
                i2cREG1->DXR=(uint32)IIC_TX_DATA[0];
                while(i2cIsBusBusy(i2cREG1) == true);
                while(i2cIsStopDetected(i2cREG1) == 0);
                i2cClearSCD(i2cREG1);
            }
            else
            {

                i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);
                i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
                i2cSetCount(i2cREG1, 4+len);
                i2cSetMode(i2cREG1, I2C_MASTER);
                i2cSetStop(i2cREG1);
                i2cSetStart(i2cREG1);
                while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                {
                } /* Wait */
                i2cREG1->DXR = (uint32)(addr>>24);
                while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                {
                } /* Wait */
                i2cREG1->DXR = (uint32)(addr>>16);
                while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                {
                } /* Wait */
                i2cREG1->DXR = (uint32)(addr>>8);
                while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                {
                } /* Wait */
                i2cREG1->DXR = (uint32)(addr&0xff);
                for(i=0;i<len;i++)
                {
                    while ((i2cREG1->STR & (uint32)I2C_TX_INT) == 0U)
                    {
                    } /* Wait */
                    i2cREG1->DXR = (uint32)buff[i];
                }
                while(i2cIsBusBusy(i2cREG1) == true);
                while(i2cIsStopDetected(i2cREG1) == 0);
                i2cClearSCD(i2cREG1);
            }

        break;
        case IIC2:
               if(DMA_IIC2_TX==true)
               {

                   i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);
                   i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
                   i2cSetCount(i2cREG2, 4+len);
                   i2cSetMode(i2cREG2, I2C_MASTER);
                   i2cSetStop(i2cREG2);
                   i2cSetStart(i2cREG2);
                   IIC_TX_DATA[0]=(uint8)(addr>>24);
                   IIC_TX_DATA[1]=(uint8)(addr>>16);
                   IIC_TX_DATA[2]=(uint8)(addr>>8);
                   IIC_TX_DATA[3]=(uint8)(addr);
                   for(i=0;0<len;i++)
                   {
                       if(i<100)
                       {
                           IIC_TX_DATA[i+4]=buff[i];
                       }
                   }
          //         while ((dmaREG->PEND & (1<<DMA_IIC2_TRANSMIT_channel))==0);
                   dmaRAMREG->PCP[DMA_IIC2_TRANSMIT_channel].ISADDR  = (uint32_t)(IIC_TX_DATA)+1;
                   dmaRAMREG->PCP[DMA_IIC2_TRANSMIT_channel].ITCOUNT = ((len+3) << 16U) | 1;
                   dmaSetChEnable(DMA_IIC2_TRANSMIT_channel, DMA_HW);
                   i2cREG2->DXR=(uint32)IIC_TX_DATA[0];
                   while(i2cIsBusBusy(i2cREG2) == true);
                   while(i2cIsStopDetected(i2cREG2) == 0);
                   i2cClearSCD(i2cREG2);
               }
               else
               {

                   i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);
                   i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
                   i2cSetCount(i2cREG2, 4+len);
                   i2cSetMode(i2cREG2, I2C_MASTER);
                   i2cSetStop(i2cREG2);
                   i2cSetStart(i2cREG2);
                   while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                   {
                   } /* Wait */
                   i2cREG2->DXR = (uint32)(addr>>24);
                   while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                   {
                   } /* Wait */
                   i2cREG2->DXR = (uint32)(addr>>16);
                   while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                   {
                   } /* Wait */
                   i2cREG2->DXR = (uint32)(addr>>8);
                   while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                   {
                   } /* Wait */
                   i2cREG2->DXR = (uint32)(addr&0xff);
                   for(i=0;i<len;i++)
                   {
                       while ((i2cREG2->STR & (uint32)I2C_TX_INT) == 0U)
                       {
                       } /* Wait */
                       i2cREG2->DXR = (uint32)buff[i];
                   }
                   while(i2cIsBusBusy(i2cREG2) == true);
                   while(i2cIsStopDetected(i2cREG2) == 0);
                   i2cClearSCD(i2cREG2);
               }

        break;
        default:
        break;
    }
}
/******************
 * 函数：void IIC_ADD8_read(IIC_Info iicx, uint8 addr, uint8* buff,uint32 len)
 * 功能：IIC 8位地址读取数据
 * 输入：iicx：配置参数句柄。->ch：IIC的编号；可选：IIC1、IIC2。
 *                    ->ticks:速率
 *      addr：读取数据的地址。
 *      buff：保存读取的数据。
 *      len：读取的数据长度。
 * 输出：无
 * *******************/
void IIC_ADD8_read(IIC_Info iicx, uint8 addr, uint8* buff,uint32 len)
{

    switch(iicx.ch)
    {
    case IIC1:
        if(DMA_IIC1_RX==true)
        {
            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

            i2cSetCount(i2cREG1, 1);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);

            i2cSetStart(i2cREG1);

            i2cSendByte(i2cREG1, addr);

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);

            while(i2cIsMasterReady(i2cREG1) != true);

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_RECEIVER);

            i2cSetCount(i2cREG1, len);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);
     //       while( ((i2cREG1->STR & I2C_NACK) == 0) && ((i2cREG1->STR & I2C_ARDY) == 0) );

            i2cSetStart(i2cREG1);

           // i2cReceive(i2cREG1, len, buff);

            while ((dmaREG->PEND & (1<<DMA_IIC1_REVICE_channel))==0);
            i2cREG1->DMACR &= (~(IIC_SET_TX_DMA));
            dmaRAMREG->PCP[DMA_IIC1_REVICE_channel].IDADDR  = (uint32_t)(buff);
            dmaRAMREG->PCP[DMA_IIC1_REVICE_channel].ITCOUNT = ((len) << 16U) | 1;
            dmaSetChEnable(DMA_IIC1_REVICE_channel, DMA_HW);
            i2cREG1->DMACR |= IIC_SET_TX_DMA ;
            while(i2cIsBusBusy(i2cREG1) == true);
            while(i2cIsStopDetected(i2cREG1) == 0);
            i2cClearSCD(i2cREG1);
        }
        else
        {
            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

            i2cSetCount(i2cREG1, 1);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);

            i2cSetStart(i2cREG1);

            i2cSendByte(i2cREG1, addr);

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);

            while(i2cIsMasterReady(i2cREG1) != true);

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_RECEIVER);

            i2cSetCount(i2cREG1, len);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);
     //       while( ((i2cREG1->STR & I2C_NACK) == 0) && ((i2cREG1->STR & I2C_ARDY) == 0) );

            i2cSetStart(i2cREG1);

            i2cReceive(i2cREG1, len, buff);

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);
        }

    break;
    case IIC2:
            if(DMA_IIC2_RX==true)
            {
                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_TRANSMITTER);

                i2cSetCount(i2cREG2, 1);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);

                i2cSetStart(i2cREG2);

                i2cSendByte(i2cREG2, addr);

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);

                while(i2cIsMasterReady(i2cREG2) != true);

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_RECEIVER);

                i2cSetCount(i2cREG2, len);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);
        //        while( ((i2cREG2->STR & I2C_NACK) == 0) && ((i2cREG2->STR & I2C_ARDY) == 0) );

                i2cSetStart(i2cREG2);

               // i2cReceive(i2cREG1, len, buff);

                while ((dmaREG->PEND & (1<<DMA_IIC2_REVICE_channel))==0);
                i2cREG2->DMACR &= (~(IIC_SET_TX_DMA));
                dmaRAMREG->PCP[DMA_IIC2_REVICE_channel].IDADDR  = (uint32_t)(buff);
                dmaRAMREG->PCP[DMA_IIC2_REVICE_channel].ITCOUNT = ((len) << 16U) | 1;
                dmaSetChEnable(DMA_IIC2_REVICE_channel, DMA_HW);
                i2cREG2->DMACR |= IIC_SET_TX_DMA ;
                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);
            }
            else
            {
                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_TRANSMITTER);

                i2cSetCount(i2cREG2, 1);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);

                i2cSetStart(i2cREG2);

                i2cSendByte(i2cREG2, addr);

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);

                while(i2cIsMasterReady(i2cREG2) != true);

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_RECEIVER);

                i2cSetCount(i2cREG2, len);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);
               // while( ((i2cREG2->STR & I2C_NACK) == 0) && ((i2cREG2->STR & I2C_ARDY) == 0) );

                i2cSetStart(i2cREG2);

                i2cReceive(i2cREG2, len, buff);

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);
            }

        break;
        default:
        break;
    }
}
/******************
 * 函数：void IIC_ADD16_read(IIC_Info iicx, uint8 addr, uint8* buff,uint32 len)
 * 功能：IIC 16位地址读取数据
 * 输入：iicx：配置参数句柄。->ch：IIC的编号；可选：IIC1、IIC2。
 *                    ->ticks:速率
 *      addr：读取数据的地址。
 *      buff：保存读取的数据。
 *      len：读取的数据长度。
 * 输出：无
 * *******************/
void IIC_ADD16_read(IIC_Info iicx, uint16 addr, uint8* buff,uint32 len)
{

    switch(iicx.ch)
    {
    case IIC1:
        if(DMA_IIC1_RX==true)
        {
            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

            i2cSetCount(i2cREG1, 2);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);

            i2cSetStart(i2cREG1);

            i2cSendByte(i2cREG1, (uint8)(addr>>8));
            i2cSendByte(i2cREG1,  (uint8)(addr));

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);

            while(i2cIsMasterReady(i2cREG1) != true);

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_RECEIVER);

            i2cSetCount(i2cREG1, len);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);
     //       while( ((i2cREG1->STR & I2C_NACK) == 0) && ((i2cREG1->STR & I2C_ARDY) == 0) );

            i2cSetStart(i2cREG1);

           // i2cReceive(i2cREG1, len, buff);

            while ((dmaREG->PEND & (1<<DMA_IIC1_REVICE_channel))==0);
            i2cREG1->DMACR &= (~(IIC_SET_TX_DMA));
            dmaRAMREG->PCP[DMA_IIC1_REVICE_channel].IDADDR  = (uint32_t)(buff);
            dmaRAMREG->PCP[DMA_IIC1_REVICE_channel].ITCOUNT = ((len) << 16U) | 1;
            dmaSetChEnable(DMA_IIC1_REVICE_channel, DMA_HW);
            i2cREG1->DMACR |= IIC_SET_TX_DMA ;
            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);
        }
        else
        {

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

            i2cSetCount(i2cREG1, 2);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);

            i2cSetStart(i2cREG1);

            i2cSendByte(i2cREG1, (uint8)(addr>>16));
            i2cSendByte(i2cREG1, (uint8)(addr));

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);

            while(i2cIsMasterReady(i2cREG1) != true);

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_RECEIVER);

            i2cSetCount(i2cREG1, len);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);
     //       while( ((i2cREG1->STR & I2C_NACK) == 0) && ((i2cREG1->STR & I2C_ARDY) == 0) );

            i2cSetStart(i2cREG1);

            i2cReceive(i2cREG1, len, buff);

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);
        }

    break;
    case IIC2:
            if(DMA_IIC2_RX==true)
            {

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_TRANSMITTER);

                i2cSetCount(i2cREG2, 2);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);

                i2cSetStart(i2cREG2);

                i2cSendByte(i2cREG2, (uint8)(addr>>16));
                i2cSendByte(i2cREG2, (uint8)(addr));

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);

                while(i2cIsMasterReady(i2cREG2) != true);

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_RECEIVER);

                i2cSetCount(i2cREG2, len);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);
        //        while( ((i2cREG2->STR & I2C_NACK) == 0) && ((i2cREG2->STR & I2C_ARDY) == 0) );

                i2cSetStart(i2cREG2);

               // i2cReceive(i2cREG1, len, buff);

                while ((dmaREG->PEND & (1<<DMA_IIC2_REVICE_channel))==0);
                i2cREG2->DMACR &= (~(IIC_SET_TX_DMA));
                dmaRAMREG->PCP[DMA_IIC2_REVICE_channel].IDADDR  = (uint32_t)(buff);
                dmaRAMREG->PCP[DMA_IIC2_REVICE_channel].ITCOUNT = ((len) << 16U) | 1;
                dmaSetChEnable(DMA_IIC2_REVICE_channel, DMA_HW);
                i2cREG2->DMACR |= IIC_SET_TX_DMA ;
                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);
            }
            else
            {

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_TRANSMITTER);

                i2cSetCount(i2cREG2, 2);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);

                i2cSetStart(i2cREG2);

                i2cSendByte(i2cREG2, (uint8)(addr>>16));
                i2cSendByte(i2cREG2, (uint8)(addr));

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);

                while(i2cIsMasterReady(i2cREG2) != true);

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_RECEIVER);

                i2cSetCount(i2cREG2, len);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);
               // while( ((i2cREG2->STR & I2C_NACK) == 0) && ((i2cREG2->STR & I2C_ARDY) == 0) );

                i2cSetStart(i2cREG2);

                i2cReceive(i2cREG2, len, buff);

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);
            }

        break;
        default:
        break;
    }
}
/******************
 * 函数：void IIC_ADD32_read(IIC_Info iicx, uint8 addr, uint8* buff,uint32 len)
 * 功能：IIC 32位地址读取数据
 * 输入：iicx：配置参数句柄。->ch：IIC的编号；可选：IIC1、IIC2。
 *                    ->ticks:速率
 *      addr：读取数据的地址。
 *      buff：保存读取的数据。
 *      len：读取的数据长度。
 * 输出：无
 * *******************/
void IIC_ADD32_read(IIC_Info iicx, uint32 addr, uint8* buff,uint32 len)
{

    switch(iicx.ch)
    {
    case IIC1:
        if(DMA_IIC1_RX==true)
        {

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

            i2cSetCount(i2cREG1, 4);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);

            i2cSetStart(i2cREG1);

            i2cSendByte(i2cREG1, (uint8)(addr>>24));
            i2cSendByte(i2cREG1, (uint8)(addr>>16));
            i2cSendByte(i2cREG1, (uint8)(addr>>8));
            i2cSendByte(i2cREG1,  (uint8)(addr));

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);

            while(i2cIsMasterReady(i2cREG1) != true);

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_RECEIVER);

            i2cSetCount(i2cREG1, len);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);
     //       while( ((i2cREG1->STR & I2C_NACK) == 0) && ((i2cREG1->STR & I2C_ARDY) == 0) );

            i2cSetStart(i2cREG1);

           // i2cReceive(i2cREG1, len, buff);

            while ((dmaREG->PEND & (1<<DMA_IIC1_REVICE_channel))==0);
            i2cREG1->DMACR &= (~(IIC_SET_TX_DMA));
            dmaRAMREG->PCP[DMA_IIC1_REVICE_channel].IDADDR  = (uint32_t)(buff);
            dmaRAMREG->PCP[DMA_IIC1_REVICE_channel].ITCOUNT = ((len) << 16U) | 1;
            dmaSetChEnable(DMA_IIC1_REVICE_channel, DMA_HW);
            i2cREG1->DMACR |= IIC_SET_TX_DMA ;
            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);
        }
        else
        {

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_TRANSMITTER);

            i2cSetCount(i2cREG1, 4);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);

            i2cSetStart(i2cREG1);

            i2cSendByte(i2cREG1, (uint8)(addr>>24));
            i2cSendByte(i2cREG1, (uint8)(addr>>16));
            i2cSendByte(i2cREG1, (uint8)(addr>>16));
            i2cSendByte(i2cREG1, (uint8)(addr));

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);

            while(i2cIsMasterReady(i2cREG1) != true);

            i2cSetSlaveAdd(i2cREG1, IIC1_SLAVE_ADD);

            i2cSetDirection(i2cREG1, I2C_RECEIVER);

            i2cSetCount(i2cREG1, len);

            i2cSetMode(i2cREG1, I2C_MASTER);

            i2cSetStop(i2cREG1);
     //       while( ((i2cREG1->STR & I2C_NACK) == 0) && ((i2cREG1->STR & I2C_ARDY) == 0) );

            i2cSetStart(i2cREG1);

            i2cReceive(i2cREG1, len, buff);

            while(i2cIsBusBusy(i2cREG1) == true);

            while(i2cIsStopDetected(i2cREG1) == 0);

            i2cClearSCD(i2cREG1);
        }

    break;
    case IIC2:
            if(DMA_IIC2_RX==true)
            {

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_TRANSMITTER);

                i2cSetCount(i2cREG2, 4);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);

                i2cSetStart(i2cREG2);

                i2cSendByte(i2cREG2, (uint8)(addr>>24));
                i2cSendByte(i2cREG2, (uint8)(addr>>16));
                i2cSendByte(i2cREG2, (uint8)(addr>>16));
                i2cSendByte(i2cREG2, (uint8)(addr));

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);

                while(i2cIsMasterReady(i2cREG2) != true);

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_RECEIVER);

                i2cSetCount(i2cREG2, len);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);
        //        while( ((i2cREG2->STR & I2C_NACK) == 0) && ((i2cREG2->STR & I2C_ARDY) == 0) );

                i2cSetStart(i2cREG2);

               // i2cReceive(i2cREG1, len, buff);

                while ((dmaREG->PEND & (1<<DMA_IIC2_REVICE_channel))==0);
                i2cREG2->DMACR &= (~(IIC_SET_TX_DMA));
                dmaRAMREG->PCP[DMA_IIC2_REVICE_channel].IDADDR  = (uint32_t)(buff);
                dmaRAMREG->PCP[DMA_IIC2_REVICE_channel].ITCOUNT = ((len) << 16U) | 1;
                dmaSetChEnable(DMA_IIC2_REVICE_channel, DMA_HW);
                i2cREG2->DMACR |= IIC_SET_TX_DMA ;
                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);
            }
            else
            {
                while(i2cIsMasterReady(i2cREG2) != true);
                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_TRANSMITTER);

                i2cSetCount(i2cREG2, 4);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);

                i2cSetStart(i2cREG2);

                i2cSendByte(i2cREG2, (uint8)(addr>>24));
                i2cSendByte(i2cREG2, (uint8)(addr>>16));
                i2cSendByte(i2cREG2, (uint8)(addr>>16));
                i2cSendByte(i2cREG2, (uint8)(addr));

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);

                while(i2cIsMasterReady(i2cREG2) != true);

                i2cSetSlaveAdd(i2cREG2, IIC2_SLAVE_ADD);

                i2cSetDirection(i2cREG2, I2C_RECEIVER);

                i2cSetCount(i2cREG2, len);

                i2cSetMode(i2cREG2, I2C_MASTER);

                i2cSetStop(i2cREG2);
               // while( ((i2cREG2->STR & I2C_NACK) == 0) && ((i2cREG2->STR & I2C_ARDY) == 0) );

                i2cSetStart(i2cREG2);

                i2cReceive(i2cREG2, len, buff);

                while(i2cIsBusBusy(i2cREG2) == true);

                while(i2cIsStopDetected(i2cREG2) == 0);

                i2cClearSCD(i2cREG2);
            }

        break;
        default:
        break;
    }
}
void i2cNotification(i2cBASE_t *i2c, uint32 flags)
{
/*  enter user code between the USER CODE BEGIN and USER CODE END. */
/* USER CODE BEGIN (24) */
    if(i2c==i2cREG1 && flags==I2C_RX_INT)
    {

    }
    else if(i2c==i2cREG2 && flags==I2C_RX_INT)
    {

    }
/* USER CODE END */
}
