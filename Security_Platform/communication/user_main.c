/*
 * user_main.c
 *
 *  Created on: 2021Äê10ÔÂ9ÈÕ
 *      Author: 15755
 */


#include"user_main.h"
ETH_Info  ethx;
socket  psocket;
ETH_Info IPPOS;
extern uint8_t revice_ok;
void user_mian(void)
{
    /*
    uint8 UDP_Iaddr[4] = { 192, 168, 0, 2 };
    uint8 UDP_Oaddr[4] = { 192, 168, 0, 44 };
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
    ip=0;
    ip  =(uint32_t)(UDP_Iaddr[0]<<24 | UDP_Iaddr[1]<<16 | UDP_Iaddr[2]<<8 | UDP_Iaddr[3]);
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
    }*/
}
