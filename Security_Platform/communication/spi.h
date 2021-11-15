/*
 * spi.h
 *
 *  Created on: 2021年10月8日
 *      Author: 15755
 */

#ifndef COMMUNICATION_SPI_H_
#define COMMUNICATION_SPI_H_

#include "HL_hal_stdtypes.h"
#define VCLK 75000
#define DMA_SPI1_REVICE_channel    DMA_CH8
#define DMA_SPI1_TRANSMIT_channel  DMA_CH9
#define DMA_SPI2_REVICE_channel    DMA_CH10
#define DMA_SPI2_TRANSMIT_channel  DMA_CH11
#define DMA_SPI3_REVICE_channel    DMA_CH12
#define DMA_SPI3_TRANSMIT_channel  DMA_CH13
#define DMA_SPI4_REVICE_channel    DMA_CH14
#define DMA_SPI4_TRANSMIT_channel  DMA_CH15
#define DMA_SPI5_REVICE_channel    DMA_CH16
#define DMA_SPI5_TRANSMIT_channel  DMA_CH17
#define DMA_SPI1_REVICE_REQUEST_LINE    DMA_REQ0
#define DMA_SPI1_TRANSMIT_REQUEST_LINE  DMA_REQ1
#define DMA_SPI2_REVICE_REQUEST_LINE    DMA_REQ2
#define DMA_SPI2_TRANSMIT_REQUEST_LINE  DMA_REQ3
#define DMA_SPI3_REVICE_REQUEST_LINE    DMA_REQ14
#define DMA_SPI3_TRANSMIT_REQUEST_LINE  DMA_REQ15
#define DMA_SPI4_REVICE_REQUEST_LINE   DMA_REQ24
#define DMA_SPI4_TRANSMIT_REQUEST_LINE  DMA_REQ25
#define DMA_SPI5_REVICE_REQUEST_LINE   DMA_REQ30
#define DMA_SPI5_TRANSMIT_REQUEST_LINE  DMA_REQ31
#define DMA_SPI1_channel    0
#define DMA_SPI2_channel    1
#define DMA_SPI3_channel    2
#define DMA_SPI4_channel    3
#define DMA_SPI5_channel    4

#define kSPIBuffLen 60
#define SPI1_send_SIZE  (16U)
#define SPI1_revice_SIZE  (16U)
#define SPI2_send_SIZE  (16U)
#define SPI2_revice_SIZE  (16U)
#define SPI3_send_SIZE  (16U)
#define SPI3_revice_SIZE  (16U)
#define SPI4_send_SIZE  (16U)
#define SPI4_revice_SIZE  (16U)
#define SPI5_send_SIZE  (16U)
#define SPI5_revice_SIZE  (16U)
enum SPI_NUM  /*spi 编号 */
{
    SPI1=1,
    SPI2=2,
    SPI3=3,
    SPI4=4,
    SPI5=5,
};
enum SPI_MODE /*spi 主从模式 */
{
    SPI_SLAVEL=0,
    SPI_MASTER=1,
};
typedef struct _SPI_INFO /*spi 参数 */
{
    uint8_t ch;  /*SPI channel*/
    uint32 burt; /*Baud rate*/
    uint8 CPHA;  /*Clock phase*/
    uint8 CPOL;  /*Clock polarity*/
    uint8 mode;  /*Master or slaveL*/
} SPI_Info;
/*spi 接收数据队列参数 */
typedef struct _SPIData {
    uint8_t head;
    uint8_t tail;
    uint8_t revicebuf[kSPIBuffLen];
}SPI_revice_buff;

/** @struct g_dmaCTRL
*   @brief Interrupt mode globals
*
*/

extern SPI_revice_buff SPI1_MSG_buff;
extern SPI_revice_buff SPI2_MSG_buff;
extern SPI_revice_buff SPI3_MSG_buff;
extern SPI_revice_buff SPI4_MSG_buff;
extern SPI_revice_buff SPI5_MSG_buff;
void SPI_init(SPI_Info spix);
void SPI_Master_send(SPI_Info spix,uint8* buff, uint32 len);
void SPI_Slave_send(SPI_Info spix,uint8* buff, uint32 len);
void SPI_DMA_init(SPI_Info spix);
void SPI1_revive(uint8_t  revice_data);
void SPI2_revive(uint8_t  revice_data);
void SPI3_revive(uint8_t  revice_data);
void SPI4_revive(uint8_t  revice_data);
void SPI5_revive(uint8_t  revice_data);



#endif /* COMMUNICATION_SPI_H_ */
