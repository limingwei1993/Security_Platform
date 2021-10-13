#ifndef __UDP_DEMO_H
#define __UDP_DEMO_H
#include "lwip_comm.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

 
#define UDP_DEMO_RX_BUFSIZE		1024	//定义udp最大接收数据长度 
#define UDP_DEMO_PORT			8089	//定义udp连接的端口 

 
void udp_demo_test(u16 port,u16 portl);
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port);
void udp_demo_senddata(struct udp_pcb *upcb);
void udp_demo_connection_close(struct udp_pcb *upcb);

#endif

