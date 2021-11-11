/*
 * ETH.h
 *
 *  Created on: 2021年10月13日
 *      Author: 15755
 */

#ifndef COMMUNICATION_ETH_H_
#define COMMUNICATION_ETH_H_
#include <LWIP/lwip-1.4.1/src/include/ipv4/lwip/ip_frag.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/dhcp.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/init.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/mem.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/memp.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/raw.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/tcp.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/tcp_impl.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/tcpip.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/timers.h>
#include <LWIP/lwip-1.4.1/src/include/lwip/udp.h>
#include <LWIP/lwip-1.4.1/src/include/netif/etharp.h>
#include "HL_hal_stdtypes.h"
#include "LWIP/lwip-1.4.1/src/include/netif/ethernetif.h"
#define ETH_REVICE_BUFF_MAX_LEN 1024 /*ETH TCP 接收数据缓冲区长度*/
#define ETH_SEND_BUFF_MAX_LEN 1024   /*ETH TCP 发送数据缓冲区长度*/
#define ETH_TCP_NUM 10               /*ETH TCP 连接的数量*/
#define LWIP_MAX_DHCP_TRIES     4   //DHCP服务器最大重试次数
//lwip控制结构体
typedef struct
{
    uint8 mac[6];      //MAC地址
    uint8 remoteip[4]; //远端主机IP地址
    uint8 ip[4];       //本机IP地址
    uint8 netmask[4];  //子网掩码
    uint8 gateway[4];  //默认网关的IP地址

    volatile uint8 dhcpstatus; //dhcp状态
                    //0,未获取DHCP地址;
                    //1,进入DHCP获取状态
                    //2,成功获取DHCP地址
                    //0XFF,获取失败.
}__lwip_dev;
extern __lwip_dev lwipdev;  //lwip控制结构体

typedef struct  ETH_Info_s
{
    uint8 ch;
    uint32 IP;
    uint32 mask;
    uint32 gate;
} ETH_Info;

typedef struct  SOCKET_s
{
    uint32 IP;        /*对面网络IP*/
    uint16 port;       /*本机网络端口号*/
    uint32 oIP;        /*对面网络IP*/
    uint16 oport;      /*对面网络端口号*/
    uint8 isConnected; /*网络连接状态*/
} socket;

typedef struct  ETH_FRAM_s
{
    uint32 oIP;        /*对面网络IP*/
    uint16 oport;      /*对面网络端口号*/
    uint8_t isSend;    /*发送缓冲区是否有数据发送*/
    uint8_t send_data[ETH_SEND_BUFF_MAX_LEN];  /*发送缓冲区的数据*/
    uint32_t send_data_len; /*发送缓冲区的数据长度*/
    uint8_t isRevice;  /*接收缓冲区是否有数据*/
    uint8_t revice_data[ETH_REVICE_BUFF_MAX_LEN]; /*接收缓冲区的数据*/
    uint32_t revice_data_len; /*接收缓冲区的数据长度*/
} ETH_FRAM;

enum tcp_server_states
{
    ES_TCPSERVER_NONE = 0,
    ES_TCPSERVER_ACCEPTED,
    ES_TCPSERVER_CLOSING,
};

struct tcp_server_struct
{
    uint8 state;
    struct tcp_pcb *pcb;
    struct pbuf *p;
};

enum tcp_client_states
{
    ES_TCPCLIENT_NONE = 0,
    ES_TCPCLIENT_CONNECTED,
    ES_TCPCLIENT_CLOSING,
};

struct tcp_client_struct
{
    uint8 state;
    struct tcp_pcb *pcb;
    struct pbuf *p;
};

void ETH_init(ETH_Info * ethx);
ETH_Info ETH_DHCP_init(uint32 timeout);
void ETH_TCP_Server_bind(socket * psocket);
void ETH_TCP_Server_lisiten(socket * psocket);
err_t ETH_TCP_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
err_t ETH_TCP_Server_accept(void *arg,struct tcp_pcb *newpcb,err_t err);
err_t ETHx_TCP_Rx(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void ETH_server_error(void *arg,err_t err);
err_t ETH_server_poll(void *arg, struct tcp_pcb *tpcb);
err_t ETH_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void ETH_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void ETH_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es);
void ETH_server_remove_timewait(void);
void lwip_pkt_handle(void);
void lwip_periodic_handle();
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es);
#endif /* COMMUNICATION_ETH_H_ */
