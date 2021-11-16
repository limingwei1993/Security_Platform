/*
 * IIC.h
 *
 *  Created on: 2021年7月30日
 *      Author: 15755
 */

#ifndef COMMUNICATION_IIC_H_
#define COMMUNICATION_IIC_H_
#include "HL_hal_stdtypes.h"

#define IIC_SET_TX_DMA  (1<<1U)
#define IIC_SET_RX_DMA  (1<<0U)
#define IIC1_RX_DATA_LEN 30
#define IIC1_TX_DATA_LEN 30
#define IIC2_RX_DATA_LEN 30
#define IIC2_TX_DATA_LEN 30
#define IIC1_RX_SIZE 8
#define IIC1_TX_SIZE 8
#define IIC2_RX_SIZE 8
#define IIC2_TX_SIZE 8
#define DMA_IIC1_REVICE_channel    DMA_CH26
#define DMA_IIC1_TRANSMIT_channel  DMA_CH27
#define DMA_IIC2_REVICE_channel    DMA_CH28
#define DMA_IIC2_TRANSMIT_channel  DMA_CH29
#define DMA_IIC1_REVICE_REQUEST_LINE    DMA_REQ10
#define DMA_IIC1_TRANSMIT_REQUEST_LINE  DMA_REQ11
#define DMA_IIC2_REVICE_REQUEST_LINE    DMA_REQ32
#define DMA_IIC2_TRANSMIT_REQUEST_LINE  DMA_REQ44

#define IIC1_SLAVE_ADD  0x48
#define IIC2_SLAVE_ADD  0x48
enum IIc_NUM  /*IIC 编号*/
{
    IIC1=1,
    IIC2=2,
};
typedef struct _IIC_INFO /*IIC 参数*/
{
    uint8_t ch;  /*SPI channel*/
    uint32 ticks; /**/

} IIC_Info;

static struct g_i2cTransfer /*IIC 数据配置参数*/
{
    uint32  mode;
    uint32  length;
    uint8   * data;
} g_i2cTransfer_t[2U];

void IIC_init(IIC_Info iicx);
void ICC_DMA_init(IIC_Info iicx);
void IIC_ADD8_write(IIC_Info iicx, uint8 addr, uint8* buff, uint32 len);
void IIC_ADD16_write(IIC_Info iicx, uint16 addr, uint8* buff, uint32 len);
void IIC_ADD32_write(IIC_Info iicx, uint32 addr, uint8* buff, uint32 len);
void IIC_ADD8_read(IIC_Info iicx, uint8 addr, uint8* buff,uint32 len);
void IIC_ADD16_read(IIC_Info iicx, uint16 addr, uint8* buff,uint32 len);
void IIC_ADD32_read(IIC_Info iicx, uint32 addr, uint8* buff,uint32 len);
#endif /* COMMUNICATION_IIC_H_ */
