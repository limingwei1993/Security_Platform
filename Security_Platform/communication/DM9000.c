/*
 * DM900.c
 *
 *  Created on: 2021Äê10ÔÂ13ÈÕ
 *      Author: 15755
 */



#include "lwip/pbuf.h"
#include "DM9000.h"
struct dm9000_config dm9000cfg;
void delay_ms(uint32 num)
{
    while(num--);
}

uint8 DM9000_Init(void)
{
    uint32 temp=0;

    dm9000cfg.mode=DM9000_AUTO;
    dm9000cfg.queue_packet_len=0;
    dm9000cfg.imr_all = IMR_PAR|IMR_PRI;
    dm9000cfg.mac_addr[0]=2;
    dm9000cfg.mac_addr[1]=0;
    dm9000cfg.mac_addr[2]=0;
    dm9000cfg.mac_addr[3]=(temp>>16)&0XFF;
    dm9000cfg.mac_addr[4]=(temp>>8)&0XFFF;
    dm9000cfg.mac_addr[5]=temp&0XFF;
    dm9000cfg.multicase_addr[0]=0Xff;
    dm9000cfg.multicase_addr[1]=0Xff;
    dm9000cfg.multicase_addr[2]=0Xff;
    dm9000cfg.multicase_addr[3]=0Xff;
    dm9000cfg.multicase_addr[4]=0Xff;
    dm9000cfg.multicase_addr[5]=0Xff;
    dm9000cfg.multicase_addr[6]=0Xff;
    dm9000cfg.multicase_addr[7]=0Xff;

    DM9000_Reset();
    delay_ms(100);
    temp=DM9000_Get_DeiviceID();
    if(temp!=DM9000_ID) return 1;
    DM9000_Set_PHYMode(dm9000cfg.mode);

    DM9000_WriteReg(DM9000_NCR,0X00);
    DM9000_WriteReg(DM9000_TCR,0X00);
    DM9000_WriteReg(DM9000_BPTR,0X3F);
    DM9000_WriteReg(DM9000_FCTR,0X38);
    DM9000_WriteReg(DM9000_FCR,0X00);
    DM9000_WriteReg(DM9000_SMCR,0X00);
    DM9000_WriteReg(DM9000_NSR,NSR_WAKEST|NSR_TX2END|NSR_TX1END);
    DM9000_WriteReg(DM9000_ISR,0X0F);
    DM9000_WriteReg(DM9000_TCR2,0X80);
    DM9000_Set_MACAddress(dm9000cfg.mac_addr);
    DM9000_Set_Multicast(dm9000cfg.multicase_addr);
    DM9000_WriteReg(DM9000_RCR,RCR_DIS_LONG|RCR_DIS_CRC|RCR_RXEN);
    DM9000_WriteReg(DM9000_IMR,IMR_PAR);
    temp=DM9000_Get_SpeedAndDuplex();
    DM9000_WriteReg(DM9000_IMR,dm9000cfg.imr_all);
    return 0;
}


uint16 DM9000_ReadReg(uint16 reg)
{
    DM9000_IO_CMD=reg;
    return DM9000_IO_DATA;
}


void DM9000_WriteReg(uint16 reg,uint16 data)
{
    DM9000_IO_CMD=reg;
    DM9000_IO_DATA=data;
}


uint16 DM9000_PHY_ReadReg(uint16 reg)
{
    uint16 temp;
    DM9000_WriteReg(DM9000_EPAR,DM9000_PHY|reg);
    DM9000_WriteReg(DM9000_EPCR,0X0C);
    delay_ms(10);
    DM9000_WriteReg(DM9000_EPCR,0X00);
    temp=(DM9000_ReadReg(DM9000_EPDRH)<<8)|(DM9000_ReadReg(DM9000_EPDRL));
    return temp;
}


void DM9000_PHY_WriteReg(uint16 reg,uint16 data)
{
    DM9000_WriteReg(DM9000_EPAR,DM9000_PHY|reg);
    DM9000_WriteReg(DM9000_EPDRL,(data&0xff));
    DM9000_WriteReg(DM9000_EPDRH,((data>>8)&0xff));
    DM9000_WriteReg(DM9000_EPCR,0X0A);
    delay_ms(50);
    DM9000_WriteReg(DM9000_EPCR,0X00);
}


uint32 DM9000_Get_DeiviceID(void)
{
    uint32 value;
    value =DM9000_ReadReg(DM9000_VIDL);
    value|=DM9000_ReadReg(DM9000_VIDH) << 8;
    value|=DM9000_ReadReg(DM9000_PIDL) << 16;
    value|=DM9000_ReadReg(DM9000_PIDH) << 24;
    return value;
}


uint8 DM9000_Get_SpeedAndDuplex(void)
{
    uint8 temp;
    uint8 i=0;
    if(dm9000cfg.mode==DM9000_AUTO)
    {
        while(!(DM9000_PHY_ReadReg(0X01)&0X0020))
        {
            delay_ms(100);
            i++;
            if(i>100)return 0XFF;
        }
    }else
    {
        while(!(DM9000_ReadReg(DM9000_NSR)&0X40))
        {
            delay_ms(100);
            i++;
            if(i>100)return 0XFF;
        }
    }
    temp =((DM9000_ReadReg(DM9000_NSR)>>6)&0X02);
    temp|=((DM9000_ReadReg(DM9000_NCR)>>3)&0X01);
    return temp;
}


void DM9000_Set_PHYMode(uint8 mode)
{
    uint16 BMCR_Value,ANAR_Value;
    switch(mode)
    {
        case DM9000_10MHD:
            BMCR_Value=0X0000;
            ANAR_Value=0X21;
            break;
        case DM9000_10MFD:
            BMCR_Value=0X0100;
            ANAR_Value=0X41;
            break;
        case DM9000_100MHD:
            BMCR_Value=0X2000;
            ANAR_Value=0X81;
            break;
        case DM9000_100MFD:
            BMCR_Value=0X2100;
            ANAR_Value=0X101;
            break;
        case DM9000_AUTO:
            BMCR_Value=0X1000;
            ANAR_Value=0X01E1;
            break;
    }
    DM9000_PHY_WriteReg(DM9000_PHY_BMCR,BMCR_Value);
    DM9000_PHY_WriteReg(DM9000_PHY_ANAR,ANAR_Value);
    DM9000_WriteReg(DM9000_GPR,0X00);
}


void DM9000_Set_MACAddress(uint8 *macaddr)
{
    uint8 i;
    for(i=0;i<6;i++)
    {
        DM9000_WriteReg(DM9000_PAR+i,macaddr[i]);
    }
}


void DM9000_Set_Multicast(uint8 *multicastaddr)
{
    uint8 i;
    for(i=0;i<8;i++)
    {
        DM9000_WriteReg(DM9000_MAR+i,multicastaddr[i]);
    }
}


void DM9000_Reset(void)
{
    DM9000_RST(0);
    delay_ms(10);
    DM9000_RST(1);
    delay_ms(100);
    DM9000_WriteReg(DM9000_GPCR,0x01);
    DM9000_WriteReg(DM9000_GPR,0);
    DM9000_WriteReg(DM9000_NCR,(0x02|NCR_RST));
    do
    {
        delay_ms(25);
    }while(DM9000_ReadReg(DM9000_NCR)&1);
    DM9000_WriteReg(DM9000_NCR,0);
    DM9000_WriteReg(DM9000_NCR,(0x02|NCR_RST));
    do
    {
        delay_ms(25);
    }while (DM9000_ReadReg(DM9000_NCR)&1);
}


void DM9000_SendPacket(struct pbuf *p)
{
    struct pbuf *q;
    uint16 pbuf_index = 0;
    uint8 word[2], word_index = 0;

    DM9000_WriteReg(DM9000_IMR,IMR_PAR);
    DM9000_IO_CMD=DM9000_MWCMD;
    q=p;
    while(q)
    {
        if (pbuf_index < q->len)
        {
            word[word_index++] = ((u8_t*)q->payload)[pbuf_index++];
            if (word_index == 2)
            {
                DM9000_IO_DATA=((uint16)word[1]<<8)|word[0];
                word_index = 0;
            }
        }else
        {
            q=q->next;
            pbuf_index = 0;
        }
    }
    if(word_index==1)DM9000_IO_DATA=word[0];
    DM9000_WriteReg(DM9000_TXPLL,p->tot_len&0XFF);
    DM9000_WriteReg(DM9000_TXPLH,(p->tot_len>>8)&0XFF);
    DM9000_WriteReg(DM9000_TCR,0X01);
    while((DM9000_ReadReg(DM9000_ISR)&0X02)==0);
    DM9000_WriteReg(DM9000_ISR,0X02);
    DM9000_WriteReg(DM9000_IMR,dm9000cfg.imr_all);
}

struct pbuf *DM9000_Receive_Packet(void)
{
    struct pbuf* p;
    struct pbuf* q;
    uint32 rxbyte;
    uint16 rx_status, rx_length;
    uint16* data;
    uint16 dummy;
    int len;

    p=NULL;
    __error_retry:
    DM9000_ReadReg(DM9000_MRCMDX);
    rxbyte=(uint8)DM9000_IO_DATA;
    if(rxbyte)
    {
        if(rxbyte>1)
        {

            DM9000_WriteReg(DM9000_RCR,0x00);
            DM9000_WriteReg(DM9000_ISR,0x80);
            return (struct pbuf*)p;
        }
        DM9000_IO_CMD=DM9000_MRCMD;
        rx_status=DM9000_IO_DATA;
        rx_length=DM9000_IO_DATA;

        p=pbuf_alloc(PBUF_RAW,rx_length,PBUF_POOL);
        if(p!=NULL)
        {
            for(q=p;q!=NULL;q=q->next)
            {
                data=(uint16*)q->payload;
                len=q->len;
                while(len>0)
                {
                    *data=DM9000_IO_DATA;
                    data++;
                    len-= 2;
                }
            }
        }else
        {
            data=&dummy;
            len=rx_length;
            while(len)
            {
                *data=DM9000_IO_DATA;
                len-=2;
            }
        }

        if((rx_status&0XBF00) || (rx_length < 0X40) || (rx_length > DM9000_PKT_MAX))
        {

            if (rx_length>DM9000_PKT_MAX)
            {

                DM9000_WriteReg(DM9000_NCR, NCR_RST);
                delay_ms(5);
            }
            if(p!=NULL)pbuf_free((struct pbuf*)p);
            p=NULL;
            goto __error_retry;
        }
    }else
    {
        DM9000_WriteReg(DM9000_ISR,ISR_PTS);
        dm9000cfg.imr_all=IMR_PAR|IMR_PRI;
        DM9000_WriteReg(DM9000_IMR, dm9000cfg.imr_all);
    }
    return (struct pbuf*)p;
}
