/*
 * SCI.h
 *
 *  Created on: 2021年7月30日
 *      Author: 15755
 */

#ifndef COMMUNICATION_SCI_H_
#define COMMUNICATION_SCI_H_
#include "HL_hal_stdtypes.h"
#include "HL_sys_dma.h"
#define SCI1_RX_DATA_LEN 30
#define SCI1_TX_DATA_LEN 30
#define SCI2_RX_DATA_LEN 30
#define SCI2_TX_DATA_LEN 30
#define SCI3_RX_DATA_LEN 30
#define SCI3_TX_DATA_LEN 30
#define SCI4_RX_DATA_LEN 30
#define SCI4_TX_DATA_LEN 30
#define SCI1_RX_SIZE 8
#define SCI1_TX_SIZE 8
#define SCI2_RX_SIZE 8
#define SCI2_TX_SIZE 8
#define SCI3_RX_SIZE 8
#define SCI3_TX_SIZE 8
#define SCI4_RX_SIZE 8
#define SCI4_TX_SIZE 8
#define DMA_SCI1_REVICE_channel    DMA_CH18
#define DMA_SCI1_TRANSMIT_channel  DMA_CH19
#define DMA_SCI2_REVICE_channel    DMA_CH20
#define DMA_SCI2_TRANSMIT_channel  DMA_CH21
#define DMA_SCI3_REVICE_channel    DMA_CH22
#define DMA_SCI3_TRANSMIT_channel  DMA_CH23
#define DMA_SCI4_REVICE_channel    DMA_CH24
#define DMA_SCI4_TRANSMIT_channel  DMA_CH25
#define DMA_SCI1_REVICE_REQUEST_LINE    DMA_REQ28
#define DMA_SCI1_TRANSMIT_REQUEST_LINE  DMA_REQ29
#define DMA_SCI2_REVICE_REQUEST_LINE    DMA_REQ40
#define DMA_SCI2_TRANSMIT_REQUEST_LINE  DMA_REQ41
#define DMA_SCI3_REVICE_REQUEST_LINE    DMA_REQ30
#define DMA_SCI3_TRANSMIT_REQUEST_LINE  DMA_REQ31
#define DMA_SCI4_REVICE_REQUEST_LINE   DMA_REQ42
#define DMA_SCI4_TRANSMIT_REQUEST_LINE  DMA_REQ43
#define SCI_SET_TX_DMA      (1<<16U)
#define SCI_SET_RX_DMA      (1<<17U)
#define SCI_SET_RX_DMA_ALL      (1<<18U)
enum SCII_NUM  /*sci编号*/
{
    SCI1=1,
    SCI2=2,
    SCI3=3,
    SCI4=4,
};
typedef struct _SCI_INFO  /*sci参数*/
{
    uint8_t ch;  /*SPI channel*/
    uint32 burt; /*Baud rate*/
    uint8 parity;  /**/
    uint8 bitlen;  /**/
    uint8 stopbits;  /**/

} SCI_Info;

typedef struct  /*sci接收数据队列参数*/
{
  /*Receive data byte queue */
  uint16_t head;
  uint16_t tail;
  uint8_t frame_temp[50];
} SCI_RecvQueue;

extern g_dmaCTRL g_dmaCTRLPKT;

extern uint8 SCI1_RX_DATA[SCI1_RX_DATA_LEN];
extern uint8 SCI2_RX_DATA[SCI2_RX_DATA_LEN];
extern uint8 SCI3_RX_DATA[SCI3_RX_DATA_LEN];
extern uint8 SCI4_RX_DATA[SCI4_RX_DATA_LEN];
extern uint8 SCI1_TX_DATA[SCI1_TX_DATA_LEN];
extern uint8 SCI2_TX_DATA[SCI2_TX_DATA_LEN];
extern uint8 SCI3_TX_DATA[SCI3_TX_DATA_LEN];
extern uint8 SCI4_TX_DATA[SCI4_TX_DATA_LEN];

void SCI2_IO_Configure(void);
void SCI_init(SCI_Info scix);
void SCI_DMA_init(SCI_Info scix);
void SCI_Tx(SCI_Info scix, uint8*buff,uint32 len);
void SCI1_RX();
void SCI2_RX();
void SCI3_RX();
void SCI4_RX();
#endif /* COMMUNICATION_SCI_H_ */
