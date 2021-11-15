/*
 * can.h
 *
 *  Created on: 2021��7��29��
 *      Author: 15755
 */

#ifndef COMMUNICATION_CAN_H_
#define COMMUNICATION_CAN_H_
#include "HL_hal_stdtypes.h"
#define DMA_CAN1_REVICE_channel   DMA_CH0           /*CAN1 ��������ʱ��������DMA��ͨ��*/
#define DMA_CAN1_TRANSMIT_channel DMA_CH1           /*CAN1 ��������ʱ��������DMA��ͨ��*/
#define DMA_CAN2_REVICE_channel   DMA_CH2           /*CAN2 ��������ʱ��������DMA��ͨ��*/
#define DMA_CAN2_TRANSMIT_channel DMA_CH3           /*CAN2 ��������ʱ��������DMA��ͨ��*/
#define DMA_CAN3_REVICE_channel   DMA_CH4           /*CAN3 ��������ʱ��������DMA��ͨ��*/
#define DMA_CAN3_TRANSMIT_channel DMA_CH5           /*CAN3 ��������ʱ��������DMA��ͨ��*/
#define DMA_CAN4_REVICE_channel   DMA_CH6           /*CAN4 ��������ʱ��������DMA��ͨ��*/
#define DMA_CAN4_TRANSMIT_channel DMA_CH7           /*CAN4 ��������ʱ��������DMA��ͨ��*/
#define DMA_CAN1_REVICE_REQUEST_LINE    DMA_REQ16   /*CAN1 ��������ʱ��DMA�ж���*/
#define DMA_CAN1_TRANSMIT_REQUEST_LINE  DMA_REQ8    /*CAN1 ��������ʱ��DMA�ж���*/
#define DMA_CAN2_REVICE_REQUEST_LINE    DMA_REQ4    /*CAN2 ��������ʱ��DMA�ж���*/
#define DMA_CAN2_TRANSMIT_REQUEST_LINE  DMA_REQ5    /*CAN2 ��������ʱ��DMA�ж���*/
#define DMA_CAN3_REVICE_REQUEST_LINE    DMA_REQ21   /*CAN3 ��������ʱ��DMA�ж���*/
#define DMA_CAN3_TRANSMIT_REQUEST_LINE  DMA_REQ17   /*CAN3 ��������ʱ��DMA�ж���*/
#define DMA_CAN4_REVICE_REQUEST_LINE   DMA_REQ47    /*CAN4 ��������ʱ��DMA�ж���*/
#define DMA_CAN4_TRANSMIT_REQUEST_LINE  DMA_REQ45   /*CAN4 ��������ʱ��DMA�ж���*/
#define CAN_MCANMESSAGE_BOX_NUM  63                 /*CAN �������ݵ�BOX����*/
#define CAN_MSG_Revice_NUM  60                      /*CAN ��������֡�ĸ���*/

#define  CAN1_RX_DATA_LEN  16    /*CAN1 DMA�������ݵĸ���*/
#define  CAN2_RX_DATA_LEN  16    /*CAN2 DMA�������ݵĸ���*/
#define  CAN3_RX_DATA_LEN  16    /*CAN3 DMA�������ݵĸ���*/
#define  CAN4_RX_DATA_LEN  16    /*CAN4 DMA�������ݵĸ���*/
#define  CAN1_TX_DATA_LEN  8     /*CAN1 DMA�������ݵĸ���*/
#define  CAN2_TX_DATA_LEN  8     /*CAN2 DMA�������ݵĸ���*/
#define  CAN3_TX_DATA_LEN  8     /*CAN3 DMA�������ݵĸ���*/
#define  CAN4_TX_DATA_LEN  8     /*CAN4 DMA�������ݵĸ���*/
enum CAN_NUM   /*CAN���*/
{
    CAN1=1,
    CAN2=2,
    CAN3=3,
    CAN4=4,
};
typedef struct _CAN_INFO   /*CAN����*/
{
    uint8_t ch;  /*Can channel*/
    uint32 burt; /*Baud rate*/

} CAN_Info;
typedef struct _CAN_MCANMESSAGE_BOX  /*CAN ID��box�Ķ�Ӧ��ϵ*/
{
    uint32 ID; /**/
    uint8_t BOX_num;  /**/
} CAN_McanMESSAGE_BOX;
typedef struct     /*CAN ֡*/
{
    uint32 ID; /**/
    uint8_t data[8];  /**/
} CAN_Frame;

typedef struct _CAN_MSG_Revice  /*����CAN���� �Ķ���*/
{
    uint8_t head;
    uint8_t tail;
    CAN_Frame revicebuf[CAN_MSG_Revice_NUM];
} CAN_MSG_Revice;


extern CAN_McanMESSAGE_BOX CAN1_McanMESSAGE_BOX_TABLE[CAN_MCANMESSAGE_BOX_NUM];
extern CAN_McanMESSAGE_BOX CAN2_McanMESSAGE_BOX_TABLE[CAN_MCANMESSAGE_BOX_NUM];
extern CAN_McanMESSAGE_BOX CAN3_McanMESSAGE_BOX_TABLE[CAN_MCANMESSAGE_BOX_NUM];
extern CAN_McanMESSAGE_BOX CAN4_McanMESSAGE_BOX_TABLE[CAN_MCANMESSAGE_BOX_NUM];

extern CAN_MSG_Revice CAN1_MSG_buff;
extern CAN_MSG_Revice CAN2_MSG_buff;
extern CAN_MSG_Revice CAN3_MSG_buff;
extern CAN_MSG_Revice CAN4_MSG_buff;


void CAN_init(CAN_Info canx);
void CAN_DMA_init(CAN_Info canx);
void CAN_Tx(CAN_Info canx,uint32 ID,uint8* buff,uint32 len);
void CAN1_revive(uint32 canrevID,uint8 * revice_data);
void CAN2_revive(uint32 canrevID,uint8 * revice_data);
void CAN3_revive(uint32 canrevID,uint8 * revice_data);
void CAN4_revive(uint32 canrevID,uint8 * revice_data);

uint32 Get_CanMcanMESSAGE_BOX_Num(CAN_Info canx,uint32 ID);
void CAN_Tx_bind(CAN_Info canx, uint32 ID,uint8 messagebox);
#endif /* COMMUNICATION_CAN_H_ */
