/**
*  \file lwiplib.c
*
*  \brief lwip related initializations
*/
/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
*/

/*
** Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
** ALL RIGHTS RESERVED
*/

/*
** lwIP Compile Time Options for HDK.
*/
#include "lwiplib.h"

/*
** lwIP high-level API/Stack/IPV4/SNMP/Network Interface/PPP codes
*/
#include "src/api/api_lib.c"
#include "src/api/api_msg.c"
#include "src/api/err.c"
#include "src/api/netbuf.c"
#include "src/api/netdb.c"
#include "src/api/netifapi.c"
#include "src/api/tcpip.c"
#include "src/api/sockets.c"

#include "src/core/dhcp.c"
#include "src/core/dns.c"
#include "src/core/init.c"
#include "src/core/mem.c"
#include "src/core/memp.c"
#include "src/core/netif.c"
#include "src/core/pbuf.c"
#include "src/core/raw.c"
#include "src/core/stats.c"
#include "src/core/sys.c"
#include "src/core/tcp.c"
#include "src/core/tcp_in.c"
#include "src/core/tcp_out.c"
#include "src/core/udp.c"

#include "src/core/ipv4/autoip.c"
#include "src/core/ipv4/icmp.c"
#include "src/core/ipv4/igmp.c"
#include "src/core/ipv4/inet.c"
#include "src/core/ipv4/inet_chksum.c"
#include "src/core/ipv4/ip.c"
#include "src/core/ipv4/ip_addr.c"
#include "src/core/ipv4/ip_frag.c"

#include "src/core/snmp/asn1_dec.c"
#include "src/core/snmp/asn1_enc.c"
#include "src/core/snmp/mib2.c"
#include "src/core/snmp/mib_structs.c"
#include "src/core/snmp/msg_in.c"
#include "src/core/snmp/msg_out.c"

#include "src/netif/etharp.c"
#include "src/netif/loopif.c"

#include "src/netif/ppp/auth.c"
#include "src/netif/ppp/chap.c"
#include "src/netif/ppp/chpms.c"
#include "src/netif/ppp/fsm.c"
#include "src/netif/ppp/ipcp.c"
#include "src/netif/ppp/lcp.c"
#include "src/netif/ppp/magic.c"
#include "src/netif/ppp/md5.c"
#include "src/netif/ppp/pap.c"
#include "src/netif/ppp/ppp.c"
#include "src/netif/ppp/ppp_oe.c"
#include "src/netif/ppp/randm.c"
#include "src/netif/ppp/vj.c"

/*
** HDK-specific lwIP interface/porting layer code.
*/
#include "ports/hdk/perf.c"
#include "ports/hdk/sys_arch.c"
#include "ports/hdk/netif/hdkif.c"
#include "locator.c"

/******************************************************************************
**                       INTERNAL VARIABLE DEFINITIONS
******************************************************************************/
/*
** The lwIP network interface structure for the HDK Ethernet MAC.
*/
static struct netif hdkNetIF[MAX_EMAC_INSTANCE];

 static uint32_t g_ui32IPMode = IPADDR_USE_STATIC;
 static struct netif g_sNetIF;
 //*****************************************************************************
 //
 // The local time for the lwIP Library Abstraction layer, used to support the
 // Host and lwIP periodic callback functions.
 //
 //*****************************************************************************
 #if NO_SYS
 uint32_t g_ui32LocalTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the TCP timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS
 static uint32_t g_ui32TCPTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the HOST timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && HOST_TMR_INTERVAL
 static uint32_t g_ui32HostTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the ARP timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && LWIP_ARP
 static uint32_t g_ui32ARPTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the AutoIP timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && LWIP_AUTOIP
 static uint32_t g_ui32AutoIPTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the DHCP Coarse timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && LWIP_DHCP
 static uint32_t g_ui32DHCPCoarseTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the DHCP Fine timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && LWIP_DHCP
 static uint32_t g_ui32DHCPFineTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the IP Reassembly timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && IP_REASSEMBLY
 static uint32_t g_ui32IPReassemblyTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the IGMP timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && LWIP_IGMP
 static uint32_t g_ui32IGMPTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the DNS timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && LWIP_DNS
 static uint32_t g_ui32DNSTimer = 0;
 #endif

 //*****************************************************************************
 //
 // The local time when the link detect timer was last serviced.
 //
 //*****************************************************************************
 #if NO_SYS && (LWIP_AUTOIP || LWIP_DHCP)
 static uint32_t g_ui32LinkTimer = 0;
 #endif
 #define IPADDR_USE_STATIC       0
 #define IPADDR_USE_DHCP         1
 #define IPADDR_USE_AUTOIP       2
 //*****************************************************************************
 //
 // The default IP address acquisition mode.
 //
 //*****************************************************************************
 //*****************************************************************************
 //
 // The most recently detected link state.
 //
 //*****************************************************************************
 #if LWIP_AUTOIP || LWIP_DHCP
 static bool g_bLinkActive = false;
 #endif
/******************************************************************************
**                          FUNCTION DEFINITIONS
******************************************************************************/
/**
 *
 * \brief Initializes the lwIP TCP/IP stack.
 *
 * \param instNum  The instance index of the EMAC module
 * \param macArray Pointer to the MAC Address
 * \param ipAddr   The IP address to be used 
 * \param netMask  The network mask to be used 
 * \param gwAddr   The Gateway address to be used 
 * \param ipMode   The IP Address Mode.
 *        ipMode can take the following values\n
 *             IPADDR_USE_STATIC - force static IP addressing to be used \n
 *             IPADDR_USE_DHCP - force DHCP with fallback to Link Local \n
 *             IPADDR_USE_AUTOIP - force  Link Local only
 *
 * This function performs initialization of the lwIP TCP/IP stack for the
 * HDK EMAC, including DHCP and/or AutoIP, as configured.
 *
 * \return IP Address.
*/
 unsigned int lwIPInit(unsigned int instNum, unsigned char *macArray,
                       unsigned int ipAddr, unsigned int netMask,
                       unsigned int gwAddr, unsigned int ipMode)
 {
     struct ip_addr ip_addr;
     struct ip_addr net_mask;
     struct ip_addr gw_addr;
     volatile unsigned char *state;
     unsigned int *ipAddrPtr;
     volatile unsigned int cnt = 0x3FFFFFFF;

     lwip_init_EMAC();

     /* Setup the network address values. */
     if(ipMode == IPADDR_USE_STATIC)
     {
         ip_addr.addr = htonl(ipAddr);
         net_mask.addr = htonl(netMask);
         gw_addr.addr = htonl(gwAddr);
     }

     else
     {
         ip_addr.addr = 0;
         net_mask.addr = 0;
         gw_addr.addr = 0;
     }

     hdkif_macaddrset(instNum, macArray);

     /*
     ** Create, configure and add the Ethernet controller interface with
     ** default settings.  ip_input_EMAC should be used to send packets directly to
     ** the stack. The lwIP will internaly call the hdkif_init function.
     */
     if(NULL ==
        netif_add_EMAC(&hdkNetIF[instNum], &ip_addr, &net_mask, &gw_addr, &instNum,
               hdkif_init, ip_input_EMAC))
     {
         return 0;
     };

     netif_set_default_EMAC(&hdkNetIF[instNum]);

     /* Start DHCP, if enabled. */
 #if LWIP_DHCP
 if(ipMode == IPADDR_USE_DHCP)
 {
 	unsigned int dhcp_flag = 0;
 	unsigned int dhcp_tries = 5;
 	unsigned int count;
 	unsigned int delay;
 	while(dhcp_tries--)
 	{


 	dhcp_start_EMAC(&hdkNetIF[instNum]);
 	count = 10;
 	/* Check for DHCP completion for 'count' number of times, each for the given delay. */
 		while(count--)
 		{
 			delay = 0x8FFFFU;
 			while(delay--);
 			state = &(hdkNetIF[instNum].dhcp->state);
 			if(DHCP_BOUND == *state)
 			{
 				dhcp_flag = 1;
 			    ipAddrPtr = (unsigned int*)&(hdkNetIF[instNum].ip_addr);
 			    return (*ipAddrPtr);
 			}
 		}

 	}
 	/* In case of DHCP failure, return 0. */
 	if(dhcp_flag == 0)
 		return 0;
 }
 #endif

     /* Start AutoIP, if enabled and DHCP is not. */
 #if LWIP_AUTOIP
     if(ipMode == IPADDR_USE_AUTOIP)
     {
         autoip_start(&hdkNetIF[instNum]);
     }
 #endif

     if((ipMode == IPADDR_USE_STATIC)
     	       ||(ipMode == IPADDR_USE_AUTOIP))
     {
        /* Bring the interface up */
        netif_set_up_EMAC(&hdkNetIF[instNum]);
     }

     ipAddrPtr = (unsigned int*)&(hdkNetIF[instNum].ip_addr);

     return (*ipAddrPtr);
 }

/*
 * \brief   Checks if the ethernet link is up
 *
 * \param   instNum  The instance number of EMAC module 
 *
 * \return  Interface status.
*/
unsigned int lwIPNetIfStatusGet(unsigned int instNum) 
{
    
    return (hdkif_netif_status(&hdkNetIF[instNum]));
}

/*
 * \brief   Checks if the ethernet link is up
 *
 * \param   instNum  The instance number of EMAC module 
 *
 * \return  The link status.
*/
unsigned int lwIPLinkStatusGet(unsigned int instNum) 
{
    return (hdkif_link_status(&hdkNetIF[instNum]));     
}

/**
 * \brief   Interrupt handler for Receive Interrupt. Directly calls the
 *          HDK interface receive interrupt handler.
 *
 * \param   instNum  The instance number of EMAC module for which receive 
 *                   interrupt happened
 *
 * \return  None.
*/
void lwIPRxIntHandler(unsigned int instNum) 
{
    hdkif_rx_inthandler(&hdkNetIF[instNum]);
}

/**
 * \brief   Interrupt handler for Transmit Interrupt. Directly calls the 
 *          HDK interface transmit interrupt handler.
 *
 * \param   instNum  The instance number of EMAC module for which transmit
 *                   interrupt happened
 *
 * \return  None.
*/
void lwIPTxIntHandler(unsigned int instNum)
{
    hdkif_tx_inthandler(&hdkNetIF[instNum]);
}

#if LWIP_AUTOIP || LWIP_DHCP
static void lwIPLinkDetect_EMAC(void)
{
    bool bHaveLink;
    struct ip_addr ip_addr;
    struct ip_addr net_mask;
    struct ip_addr gw_addr;
    // See if there is an active link.
    bHaveLink = lwIPLinkStatusGet(0);
    // Return without doing anything else if the link state hasn't changed.
    if(bHaveLink == g_bLinkActive)
    {
        return;
    }
    // Save the new link state.
    g_bLinkActive = bHaveLink;
    // Clear any address information from the network interface.
    ip_addr.addr = 0;
    net_mask.addr = 0;
    gw_addr.addr = 0;
    netif_set_addr_EMAC(&g_sNetIF, &ip_addr, &net_mask, &gw_addr);
    // See if there is a link now.
    if(bHaveLink)
    {
        // Start DHCP, if enabled.
#if LWIP_DHCP
        if(g_ui32IPMode == IPADDR_USE_DHCP)
        {
            dhcp_start_EMAC(&g_sNetIF);
        }
#endif
        // Start AutoIP, if enabled and DHCP is not.
#if LWIP_AUTOIP
        if(g_ui32IPMode == IPADDR_USE_AUTOIP)
        {
            autoip_start_EMAC(&g_sNetIF);
        }
#endif
    }
    else
    {
        // Stop DHCP, if enabled.
#if LWIP_DHCP
        if(g_ui32IPMode == IPADDR_USE_DHCP)
        {
            dhcp_stop_EMAC(&g_sNetIF);
        }
#endif
        // Stop AutoIP, if enabled and DHCP is not.
#if LWIP_AUTOIP
        if(g_ui32IPMode == IPADDR_USE_AUTOIP)
        {
            autoip_stop_EMAC(&g_sNetIF);
        }
#endif
    }
}
#endif

/**
 * 基于TCP协议，lwip维护.
 * 使用：对基于lwip协议的调度维护。用于主循环中使用。
 * */
void lwip_periodic_handle_EMAC()
{
#if HOST_TMR_INTERVAL
    if((g_ui32LocalTimer - g_ui32HostTimer) >= HOST_TMR_INTERVAL)
    {
        g_ui32HostTimer = g_ui32LocalTimer;
        lwIPHostTimerHandler_EMAC();
    }
#endif
    // Service the ARP timer.
#if LWIP_ARP
    if((g_ui32LocalTimer - g_ui32ARPTimer) >= ARP_TMR_INTERVAL)
    {
        g_ui32ARPTimer = g_ui32LocalTimer;
        etharp_tmr_EMAC();
    }
#endif
    // Service the TCP timer.
#if LWIP_TCP
    if((g_ui32LocalTimer - g_ui32TCPTimer) >= TCP_TMR_INTERVAL)
    {
        g_ui32TCPTimer = g_ui32LocalTimer;
        tcp_tmr_EMAC();
    }
#endif
    // Service the AutoIP timer.
#if LWIP_AUTOIP
    if((g_ui32LocalTimer - g_ui32AutoIPTimer) >= AUTOIP_TMR_INTERVAL)
    {
        g_ui32AutoIPTimer = g_ui32LocalTimer;
        autoip_tmr_EMAC();
    }
#endif
    // Service the DCHP Coarse Timer.
#if LWIP_DHCP
    if((g_ui32LocalTimer - g_ui32DHCPCoarseTimer) >= DHCP_COARSE_TIMER_MSECS)
    {
        g_ui32DHCPCoarseTimer = g_ui32LocalTimer;
        dhcp_coarse_tmr_EMAC();
    }
#endif
    // Service the DCHP Fine Timer.
#if LWIP_DHCP
    if((g_ui32LocalTimer - g_ui32DHCPFineTimer) >= DHCP_FINE_TIMER_MSECS)
    {
        g_ui32DHCPFineTimer = g_ui32LocalTimer;
        dhcp_fine_tmr_EMAC();
    }
#endif
    // Service the IP Reassembly Timer
#if IP_REASSEMBLY
    if((g_ui32LocalTimer - g_ui32IPReassemblyTimer) >= IP_TMR_INTERVAL)
    {
        g_ui32IPReassemblyTimer = g_ui32LocalTimer;
        ip_reass_tmr_EMAC();
    }
#endif
    // Service the IGMP Timer
#if LWIP_IGMP
    if((g_ui32LocalTimer - g_ui32IGMPTimer) >= IGMP_TMR_INTERVAL)
    {
        g_ui32IGMPTimer = g_ui32LocalTimer;
        igmp_tmr_EMAC();
    }
#endif
    // Service the DNS Timer
#if LWIP_DNS
    if((g_ui32LocalTimer - g_ui32DNSTimer) >= DNS_TMR_INTERVAL)
    {
        g_ui32DNSTimer = g_ui32LocalTimer;
        dns_tmr_EMAC();
    }
#endif
    // Service the link timer.
#if LWIP_AUTOIP || LWIP_DHCP
    if((g_ui32LocalTimer - g_ui32LinkTimer) >= LINK_TMR_INTERVAL)
    {
        g_ui32LinkTimer = g_ui32LocalTimer;
        lwIPLinkDetect_EMAC();
    }
#endif

}

/**
 * 基于TCP协议，客户端断开.
 * 使用：强制删除TCP Server主动断开时的time wait
 * */
void tcp_client_connection_close_EMAC(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
    tcp_abort_EMAC(tpcb);
    tcp_arg_EMAC(tpcb,NULL);
    tcp_recv_EMAC(tpcb,NULL);
    tcp_sent_EMAC(tpcb,NULL);
    tcp_err_EMAC(tpcb,NULL);
    tcp_poll_EMAC(tpcb,NULL,0);
    if(es)mem_free_EMAC(es);
}

/*
** Interrupt Handler for Core 0 Receive interrupt
*/
volatile int countEMACCore0RxIsr = 0;
#pragma INTERRUPT(EMACCore0RxIsr, IRQ)
void EMACCore0RxIsr(void)
{
        countEMACCore0RxIsr++;
        lwIPRxIntHandler(0);
}


void iommUnlock(void) {
    /*Unlock the IOMM Register*/
    *(int *) 0xFFFFEA38  = 0x83E70B13;  /* kicker 0 register, unlock CPU write access to PINMMR registers */
    *(int *) 0xFFFFEA3C  = 0x95A4F1E0;  /* kicker 1 register, */
}

void iommLock(void) {
    /*lock the IOMM Register*/
    *(int *) 0xFFFFEA38  = 0x00000000;  /* kicker 0 register, lock CPU write access to PINMMR registers */
    *(int *) 0xFFFFEA3C  = 0x00000000;  /* kicker 1 register, */
}

void iommMuxEnableMdio(void) {
    *(int *) 0xFFFFEB2C  = 0x00000400;
    *(int *) 0xFFFFEB30  = 0x00000400;
}

void iommMuxEnableRmii(void) {
    *(int *) 0xFFFFEB38  = 0x02010204;//P10  //RMIIRXER
    *(int *) 0xFFFFEB3C  = 0x08020101;//P11  //RMII_RXD0
    *(int *) 0xFFFFEB40  = 0x01010204;//P12  //RMII RXD1
    *(int *) 0xFFFFEB54  = 0x02040200;//P17  //RMII_RMCRSDV
    *(int *) 0xFFFFEB44  = 0x01080808;//P13  //RMII_TXEN , RMII_TX_D1 ,RMII_TX_D0
    *(int *) 0xFFFFEB48  = 0x01010401;//P14; //RMII_REFCLK
}

void iommMuxEnableMii(void) {
    *(int *) 0xFFFFEB38  &= 0xFFFFFF00; //P10[1]  //Mux 10 Rx_ER
    *(int *) 0xFFFFEB38  |= (1 << 1);   //P10[1]  //Mux 10 Rx_ER

    *(int *) 0xFFFFEB3C  &= 0x00FFFFFF; //P11[26]   //Mux 11 Rx[0]
    *(int *) 0xFFFFEB3C  |= (1 << 26);  //P11[26]   //Mux 11 Rx[0]

    *(int *) 0xFFFFEB40  &= 0x0000FF00;//P12[1,18,26]    //Mux 12 Rx[3],Rx[2],Rx[1]
    *(int *) 0xFFFFEB40  |= ((1<<26) | (1<<18) | (1<<1));//P12[1,18,26]    //Mux 12 Rx[3],Rx[2],Rx[1]

    *(int *) 0xFFFFEB44  &= 0x00000000;//P13[2, 10, 26,18]   //Mux 13 Tx[2],TxEn,Tx[1],Tx[0]
    *(int *) 0xFFFFEB44  |= ((1<<26)|(1<<18)|(1<<10)|(1<<2)); //P13[2, 10, 26,18]   //Mux 13 Tx[2],TxEn,Tx[1],Tx[0]

    *(int *) 0xFFFFEB48  &= 0xFFFF0000; //P14[9,2,11]   //Mux 14 Tx[3],RxClk
    *(int *) 0xFFFFEB48  |= ((1<<9)|(1<<2));    //P14[9,2]   //Mux 14 Tx[3],RxClk

    *(int *) 0xFFFFEB54  &= 0xFF00FF00      ;//P17[17,1,3]   //Mux 17 CRS,TxClk
    *(int *) 0xFFFFEB54  |= ((1<<17)|(1<<1));          //P17[17,1]   //Mux 17 CRS,TxClk

    *(int *) 0xFFFFEB5C  &= 0xFFFF00FF;  //P19[9]   //Mux 19 RxDV
    *(int *) 0xFFFFEB5C  |= (1<<9);      //P19[9]   //Mux 19 RxDV

    *(int *) 0xFFFFEB60  &= 0xFF00FFFF;  //P20[18]   //Mux 20 COL
    *(int *) 0xFFFFEB60  |= (1<<18);     //P20[18]   //Mux 20 COL

    *(int *) 0xFFFFEB84  &= 0x00FFFFFF;//P29[24]  //Mux 29 MII Select pin (24 bit - 0(MII),1(RMII))
    *(int *) 0xFFFFEB84  |= (0<<24);   //P29[24]  //Mux 29 MII Select pin (24 bit - 0(MII),1(RMII))
}
/*
** Interrupt Handler for Core 0 Transmit interrupt
*/
volatile int countEMACCore0TxIsr = 0;
#pragma INTERRUPT(EMACCore0TxIsr, IRQ)
void EMACCore0TxIsr(void)
{
    countEMACCore0TxIsr++;
    lwIPTxIntHandler(0);
}

void IntMasterIRQEnable(void)
{
    _enable_IRQ();
    return;
}

void IntMasterIRQDisable(void)
{
    _disable_IRQ();
    return;
}

unsigned int IntMasterStatusGet(void)
{
    return (0xC0 & _get_CPSR());
}
/***************************** End Of File ***********************************/




























