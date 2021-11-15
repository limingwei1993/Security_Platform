/*
 * user_main.c
 *
 *  Created on: 2021年10月9日
 *      Author: 15755
 */


#include"user_main.h"
#include"HL_reg_spi.h"
#include"HL_spi.h"
ETH_Info  ethx;
socket  psocket;
ETH_Info IPPOS;
extern uint8_t revice_ok;
uint8 buff[40]={0};
uint16 num=0;
uint32 time_nu=5000;
SPI_Info spix;
SCI_Info scix;
ADC_Single_Channel_Info pch;
ADC_Continues_Channel_Info PCHS;
/******************
 * 函数：void user_mian(void)
 * 功能：用户主函数入口
 * 输入：无
 * 输出：无
 * *******************/
void user_mian(void)
{

    /*
    pch.ADC_group=1;
    pch.ch=0x0000000f;
    pch.source=ADC2_GIOB0;
    PCHS.ADC_group=1;
    PCHS.chs=0x0000000f;
    PCHS.source=ADC2_GIOB0;
    gioInit();
    dmaEnable();
    gioSetDirection(gioPORTB, 1);
  //  ADC_DMA_Single_Channel_init(&pch);
  //  ADC_Single_start(&pch);
    ADC_DMA_Continues_Channel_init(&PCHS);
    ADC_Continues_start(&PCHS);
    while(1)
    {

        gioSetBit(gioPORTB, 0, 1);
        while(time_nu--);
        time_nu=50000;
        while(time_nu--);
        time_nu=50000;
        while(time_nu--);
        time_nu=50000;
        while(time_nu--);
        time_nu=50000;
        while(time_nu--);
        time_nu=50000;
      //  while(ADC_Single_isfinish(&pch)==0);
      //  adcGetData(adcREG1, adcGROUP1,pch.data);
        gioSetBit(gioPORTB, 0, 0);
        ADC_Single_start(&pch);
        while(time_nu--);
        time_nu=50000;
        while(time_nu--);
        time_nu=50000;
        while(time_nu--);
        time_nu=50000;
        while(time_nu--);
        time_nu=50000;
        while(time_nu--);
        time_nu=50000;
    }*/
    /*
    scix.ch=1;
    scix.burt=9600;
    scix.bitlen=8;
    scix.stopbits=1;
    scix.parity=0;
    _enable_IRQ_interrupt_();
    _enable_IRQ();
    SCI_init(scix);
    dmaEnable();
    SCI_DMA_init(scix);
    for(num=0;num<40;num++)
    {
        buff[num]=num;
    }
    while(1)
    {

         SCI_Tx(scix, buff,10);
         while(time_nu--);
         time_nu=50000;
         SCI_Tx(scix, buff,10);
            while(time_nu--);
            time_nu=30000;
            SCI_Tx(scix, buff,20);
            while(time_nu--);
            time_nu=50000;
            SCI_Tx(scix, buff,5);
            while(time_nu--);
            time_nu=50000;
            SCI_Tx(scix, buff,40);
            while(time_nu--);
            time_nu=50000;
    }*/
  /*     //测试SPI
    uint8 num=0;
    spix.ch=5;
    spix.burt=1000;
    spix.CPHA=0;
    spix.CPOL=0;
    spix.mode=1;
    SPI_init(spix);
    dmaEnable();
    SPI_DMA_init(spix);
   // _enable_IRQ_interrupt_();
   // _enable_IRQ();
    for(num=0;num<40;num++)
    {
        buff[num]=num;
    }
    spiEnableLoopback(spiREG5, Digital_Lbk);
    SPI_Master_send(spix,buff, 10);
    while(1);
*/
  /*    //测试EMAC
    uint8 UDP_Iaddr[4] = { 192, 168, 0, 2 };
    uint8 UDP_Oaddr[4] = { 192, 168, 1, 113 };
    uint8 netmask[4] = { 255, 255, 255, 0 };
    uint8 gateway[4] = { 192, 168, 0, 4 };
    uint32_t ip=0;
    uint8_t num=0;
    gioInit();
    rtiInit();
    rtiEnableNotification(rtiREG1,rtiNOTIFICATION_COMPARE3); //10ms
    rtiStartCounter(rtiREG1,rtiCOUNTER_BLOCK1);
    ip=0;
    ip  =(uint32_t)(UDP_Iaddr[0]<<24 | UDP_Iaddr[1]<<16 | UDP_Iaddr[2]<<8 | UDP_Iaddr[3]);
    ethx.IP=ip;
    ip=0;
    ip  =(uint32_t)(netmask[0]<<24 | netmask[1]<<16 | netmask[2]<<8 | netmask[3]);
    ethx.mask=ip;
    ip=0;
    ip  =(uint32_t)(gateway[0]<<24 | gateway[1]<<16 | gateway[2]<<8 | gateway[3]);
    ethx.gate=ip;
  //  ETH_init_EMAC(&ethx);
    IPPOS=ETH_DHCP_init_EMAC(20);
    if(IPPOS.IP==0)
    {
        while(1);
    }
    ip=0;
    ip  =(uint32_t)(UDP_Oaddr[0]<<24 | UDP_Oaddr[1]<<16 | UDP_Oaddr[2]<<8 | UDP_Oaddr[3]);
    psocket.oIP=ip;
  //  ip=0;
 //   ip  =(uint32_t)(UDP_Iaddr[0]<<24 | UDP_Iaddr[1]<<16 | UDP_Iaddr[2]<<8 | UDP_Iaddr[3]);
  //  psocket.IP=ip;
    psocket.IP=IPPOS.IP;
    psocket.port=5000;
    psocket.oport=5000;
   // ETH_TCP_Server_lisiten_EMAC(&psocket);
    ETH_TCP_Server_bind_EMAC(&psocket);
    while(1)
    {
       if(revice_ok!=0)
       {
           for(num=0;num<ETH_TCP_connect_num_EMAC;num++)
           {
               if(psocket_server_EMAC[num].isConnected==1 && ETH_TCP_EMAC[num].isRevice==1)
               {
                   psocket.oIP=psocket_server_EMAC[num].oIP;
                   psocket.oport=psocket_server_EMAC[num].oport;
                   ETH_TCP_Tx_EMAC(&psocket, ETH_TCP_EMAC[num].revice_data, ETH_TCP_EMAC[num].revice_data_len);
               }
           }

       }
    }

*/
}


