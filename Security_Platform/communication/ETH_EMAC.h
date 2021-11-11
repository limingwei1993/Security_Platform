/*
 * ETH_EMAC.h
 *
 *  Created on: 2021年11月11日
 *      Author: 15755
 */
#include "HL_sys_common.h"
#include "HL_system.h"
#include "HL_emac.h"
#include "HL_mdio.h"
#include "HL_phy_dp83640.h"
#include "HL_sci.h"


#include "lwipopts.h"
#include "lwiplib.h"
#include "httpd.h"
#include "lwip\inet.h"
#include "locator.h"
#include "string.h"
#include "ETH.h"
#ifndef COMMUNICATION_ETH_EMAC_H_
#define COMMUNICATION_ETH_EMAC_H_

#define ETH_REVICE_BUFF_MAX_LEN_EMAC 1024 /*ETH TCP 接收数据缓冲区长度*/
#define ETH_SEND_BUFF_MAX_LEN_EMAC 1024   /*ETH TCP 发送数据缓冲区长度*/
#define ETH_TCP_NUM_EMAC 10               /*ETH TCP 连接的数量*/
#define LWIP_MAX_DHCP_TRIES_EMAC     4   //DHCP服务器最大重试次数
extern ETH_FRAM ETH_TCP_EMAC[ETH_TCP_NUM_EMAC];
extern socket  psocket_server_EMAC[ETH_TCP_NUM];
extern uint8_t ETH_TCP_connect_num_EMAC;
void ETH_init_EMAC(ETH_Info * ethx);
ETH_Info ETH_DHCP_init_EMAC(uint32 timeout);
void ETH_TCP_Server_bind_EMAC(socket * psocket);
void ETH_TCP_Server_lisiten_EMAC(socket * psocket);
err_t ETH_TCP_client_connected_EMAC(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t ETH_TCP_Server_accept_EMAC(void *arg,struct tcp_pcb *newpcb,err_t err);
err_t ETHx_TCP_Rx_EMAC(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void ETH_server_error_EMAC(void *arg,err_t err);
void ETH_TCP_Tx_EMAC(socket * psocket, uint8_t *buff, int len);
err_t ETH_server_poll_EMAC(void *arg, struct tcp_pcb *tpcb);
err_t ETH_server_sent_EMAC(void *arg, struct tcp_pcb *tpcb, u16_t len);
void ETH_server_senddata_EMAC(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void ETH_server_connection_close_EMAC(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void EMAC_lwip_periodic_handle(void);
#endif /* COMMUNICATION_ETH_EMAC_H_ */
