/*
 * ETH_EMAC.c
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
#include "ETH_EMAC.h"
uint8_t ETH_TCP_connect_num_EMAC=0;
ETH_FRAM ETH_TCP_EMAC[ETH_TCP_NUM_EMAC];
socket  psocket_server_EMAC[ETH_TCP_NUM];
uint8_t revice_ok=0;

/**
 * ETH初始化，固定IP
 * ethx：本机网口参数。
 * */
void ETH_init_EMAC(ETH_Info * ethx)
{
        unsigned int   ipAddr;
        uint8 UDP_Iaddr[4] = { 192, 168, 0, 2 };
        uint8 netmask[4] = { 255, 255, 255, 0 };
        uint8 gateway[4] = { 192, 168, 0, 4 };
        uint8 macAddress[6] = { 0x00U, 0x08U, 0xEEU, 0x03U, 0xA6U, 0x6CU };
        //默认本地IP为
        UDP_Iaddr[0]=(ethx->IP>>24)&0xff;
        UDP_Iaddr[1]=(ethx->IP>>16)&0xff;
        UDP_Iaddr[2]=(ethx->IP>>8)&0xff;
        UDP_Iaddr[3]=(ethx->IP>>0)&0xff;
        //默认子网掩码
        netmask[0]=(ethx->mask>>24)&0xff;
        netmask[1]=(ethx->mask>>16)&0xff;
        netmask[2]=(ethx->mask>>8)&0xff;
        netmask[3]=(ethx->mask>>0)&0xff;
        //默认网关
        gateway[0]=(ethx->gate>>24)&0xff;
        gateway[1]=(ethx->gate>>16)&0xff;
        gateway[2]=(ethx->gate>>8)&0xff;
        gateway[3]=(ethx->gate>>0)&0xff;

        //使用静态IP
        IntMasterIRQEnable();
        ipAddr = lwIPInit(0, macAddress, *((uint32_t *)UDP_Iaddr), *((uint32_t *)netmask), *((uint32_t *)gateway), IPADDR_USE_STATIC);
}
/**
 * ETH初始化，DHCP分配
 * timeout 超时
 * */
ETH_Info ETH_DHCP_init_EMAC(uint32 timeout)
{
    unsigned int  ipAddr;
    ETH_Info ethx;
    ethx.IP=0;
    ethx.gate=0;
    uint8 macAddress[6] = { 0x00U, 0x08U, 0xEEU, 0x03U, 0xA6U, 0x6CU };
    IntMasterIRQEnable();
    while(timeout--)
    {
        ipAddr = lwIPInit(0, macAddress, 0, 0, 0, IPADDR_USE_DHCP);
        if(ipAddr==0)
        {
            return ethx;//网卡添加失败
        }
        else//网口添加成功后
        {
            ethx.IP=(uint32_t)(ipAddr & 0xffffffff);
            ethx.mask=(uint32_t)(0xffffff00);
            ethx.gate=(uint32_t)((ipAddr & 0xffffff00)+2);
            return ethx;
        }
    }
    return ethx;
}
/**
 * 基于TCP协议，作为客户端连接服务器.
 * psocket:服务器参数
 * 工作：创建一个客户端，根据传入的参数连接服务器。
 * */
void ETH_TCP_Server_bind_EMAC(socket * psocket)
{
    err_t err;
    struct tcp_pcb *tcppcb;     //定义一个TCP服务器控制块
    uint8_t UDP_Oaddr[4]={0,0,0,0};
    uint8_t UDP_Iaddr[4]={0,0,0,0};
    UDP_Oaddr[0]=(uint8_t)((psocket->oIP>>24)&0xff);
    UDP_Oaddr[1]=(uint8_t)((psocket->oIP>>16)&0xff);
    UDP_Oaddr[2]=(uint8_t)((psocket->oIP>>8)&0xff);
    UDP_Oaddr[3]=(uint8_t)((psocket->oIP>>0)&0xff);

    UDP_Iaddr[0]=(uint8_t)((psocket->IP>>24)&0xff);
    UDP_Iaddr[1]=(uint8_t)((psocket->IP>>16)&0xff);
    UDP_Iaddr[2]=(uint8_t)((psocket->IP>>8)&0xff);
    UDP_Iaddr[3]=(uint8_t)((psocket->IP>>0)&0xff);
    tcppcb=tcp_new_EMAC();   //创建一个新的pcb
    if(tcppcb)          //创建成功
    {
        err=tcp_bind_EMAC(tcppcb,(ip_addr_t *)UDP_Iaddr,psocket->port);   //将本地IP与指定的端口号绑定在一起,IP_ADDR_ANY为绑定本地所有的IP地址
        if(err==ERR_OK) //绑定完成
        {
            tcp_connect_EMAC(tcppcb,(ip_addr_t *)UDP_Oaddr,psocket->oport,ETH_TCP_client_connected_EMAC);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
        }
    }
}
/**
 * 基于TCP协议，作为服务器监听客户端.
 * psocket:服务器参数
 * 工作：根据传入的参数创建一个服务器，并进入监听状态。
 * */
void ETH_TCP_Server_lisiten_EMAC(socket * psocket)
{
    err_t err;
    struct tcp_pcb *tcppcbnew;      //定义一个TCP服务器控制块
    struct tcp_pcb *tcppcbconn;     //定义一个TCP服务器控制块
    uint8 UDP_Iaddr[4] = { 192, 168, 0, 2 };
    UDP_Iaddr[0]=(uint8_t)((psocket->IP>>24)&0xff);
    UDP_Iaddr[1]=(uint8_t)((psocket->IP>>16)&0xff);
    UDP_Iaddr[2]=(uint8_t)((psocket->IP>>8)&0xff);
    UDP_Iaddr[3]=(uint8_t)((psocket->IP>>0)&0xff);
    tcppcbnew=tcp_new_EMAC();    //创建一个新的pcb
    if(tcppcbnew)           //创建成功
    {
        err=tcp_bind_EMAC(tcppcbnew,(ip_addr_t *)UDP_Iaddr,psocket->port);   //将本地IP与指定的端口号绑定在一起,IP_ADDR_ANY为绑定本地所有的IP地址
        if(err==ERR_OK) //绑定完成
        {
            tcppcbconn=tcp_listen(tcppcbnew);           //设置tcppcb进入监听状态
            tcp_accept_EMAC(tcppcbconn,ETH_TCP_Server_accept_EMAC);   //初始化LWIP的tcp_accept的回调函数
        }
    }
}
/**
 * 基于TCP协议，作为客户端连接服务器后的回调函数.
 * 工作：对连接成功的服务器的IP和端口号进行保存。
 * */
err_t ETH_TCP_client_connected_EMAC(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct tcp_client_struct *es=NULL;
    if(err==ERR_OK)
    {
        es=(struct tcp_client_struct*)mem_malloc_EMAC(sizeof(struct tcp_client_struct));  //申请内存
        if(es) //内存申请成功
        {
            es->state=ES_TCPCLIENT_CONNECTED;//状态为连接成功
            es->pcb=tpcb;
            es->p=NULL;
            tcp_arg_EMAC(tpcb,es);                   //使用es更新tpcb的callback_arg
            tcp_recv_EMAC(tpcb,ETHx_TCP_Rx_EMAC);   //初始化tcp_recv()的回调函数
            tcp_err_EMAC(tpcb,ETH_server_error_EMAC);   //初始化tcp_err()回调函数
            tcp_poll_EMAC(tpcb,ETH_server_poll_EMAC,1); //初始化tcp_poll回调函数
            tcp_sent_EMAC(tpcb,ETH_server_sent_EMAC);   //初始化发送回调函数
            psocket_server_EMAC[ETH_TCP_connect_num_EMAC].isConnected=1;              //标记有客户端连上了
            psocket_server_EMAC[ETH_TCP_connect_num_EMAC].oIP=tpcb->remote_ip.addr;
            psocket_server_EMAC[ETH_TCP_connect_num_EMAC].oport=tpcb->remote_port;
            ETH_TCP_EMAC[ETH_TCP_connect_num_EMAC].oport=tpcb->remote_port;
            ETH_TCP_EMAC[ETH_TCP_connect_num_EMAC].oIP=tpcb->remote_ip.addr;
            ETH_TCP_connect_num_EMAC++;
            err=ERR_OK;
        }else
        {
            tcp_client_connection_close_EMAC(tpcb,es);//关闭连接
            err=ERR_MEM;    //返回内存分配错误
        }
    }else
    {
        tcp_client_connection_close_EMAC(tpcb,0);//关闭连接
    }
    return err;
}
/**
 * 基于TCP协议，作为服务器连接客户端后的回调函数.
 * 工作：对连接成功的客户端的IP和端口号进行保存。
 * */
err_t ETH_TCP_Server_accept_EMAC(void *arg,struct tcp_pcb *newpcb,err_t err)
{
    err_t ret_err;
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    tcp_setprio_EMAC(newpcb,TCP_PRIO_MIN);//设置新创建的pcb优先级
    es=(struct tcp_server_struct*)mem_malloc_EMAC(sizeof(struct tcp_server_struct)); //分配内存
    if(es!=NULL) //内存分配成功
    {
        es->state=ES_TCPSERVER_ACCEPTED;    //接收连接
        es->pcb=newpcb;
        es->p=NULL;
        tcp_arg_EMAC(newpcb,es);
        tcp_recv_EMAC(newpcb,ETHx_TCP_Rx_EMAC);   //初始化tcp_recv()的回调函数
        tcp_err_EMAC(newpcb,ETH_server_error_EMAC);   //初始化tcp_err()回调函数
        tcp_poll_EMAC(newpcb,ETH_server_poll_EMAC,1); //初始化tcp_poll回调函数
        tcp_sent_EMAC(newpcb,ETH_server_sent_EMAC);   //初始化发送回调函数
        psocket_server_EMAC[ETH_TCP_connect_num_EMAC].isConnected=1;              //标记有客户端连上了
        psocket_server_EMAC[ETH_TCP_connect_num_EMAC].oIP=newpcb->remote_ip.addr;
        psocket_server_EMAC[ETH_TCP_connect_num_EMAC].oport=newpcb->remote_port;
        ETH_TCP_EMAC[ETH_TCP_connect_num_EMAC].oport=newpcb->remote_port;
        ETH_TCP_EMAC[ETH_TCP_connect_num_EMAC].oIP=newpcb->remote_ip.addr;
        ETH_TCP_connect_num_EMAC++;
        ret_err=ERR_OK;
    };
    return ret_err;
}

/**
 * 基于TCP协议，接收数据后的回调函数.
 * 工作：对接收到的网口参数进行判断是否以建立连接，如果建立了连接则将数据存放到接收缓冲区，等待使用。
 * */
err_t ETHx_TCP_Rx_EMAC(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    err_t ret_err;
    uint32 data_len = 0;
    uint8_t i=0;
    struct pbuf *q;
    struct tcp_server_struct *es;
    LWIP_ASSERT("arg != NULL",arg != NULL);
    es=(struct tcp_server_struct *)arg;
    if(p==NULL) //从客户端接收到空数据
    {
        es->state=ES_TCPSERVER_CLOSING;//需要关闭TCP 连接了
        es->p=p;
        ret_err=ERR_OK;
    }else if(err!=ERR_OK)   //从客户端接收到一个非空数据,但是由于某种原因err!=ERR_OK
    {
        if(p)pbuf_free_EMAC(p);  //释放接收pbuf
        ret_err=err;
    }else if(es->state==ES_TCPSERVER_ACCEPTED)  //处于连接状态
    {
        if(p!=NULL)  //当处于连接状态并且接收到的数据不为空时将其打印出来
        {
            for(i=0;i<ETH_TCP_connect_num_EMAC;i++)
            {
                if(ETH_TCP_EMAC[i].oIP==tpcb->remote_ip.addr && ETH_TCP_EMAC[i].oport==tpcb->remote_port)
                {
                    memset(ETH_TCP_EMAC[i].revice_data,0,ETH_REVICE_BUFF_MAX_LEN_EMAC);  //数据接收缓冲区清零
                    for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
                    {
                        //判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
                        //的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
                        if(q->len > (ETH_REVICE_BUFF_MAX_LEN_EMAC-data_len))
                        {
                            memcpy(ETH_TCP_EMAC[i].revice_data+data_len,q->payload,(ETH_REVICE_BUFF_MAX_LEN_EMAC-data_len));//拷贝数据
                        }
                        else
                        {
                            memcpy(ETH_TCP_EMAC[i].revice_data+data_len,q->payload,q->len);
                        }
                        data_len += q->len;
                        if(data_len > ETH_REVICE_BUFF_MAX_LEN_EMAC) break; //超出TCP客户端接收数组,跳出
                    }
                    ETH_TCP_EMAC[i].isRevice=1;
                    ETH_TCP_EMAC[i].revice_data_len=data_len>ETH_REVICE_BUFF_MAX_LEN_EMAC?ETH_REVICE_BUFF_MAX_LEN_EMAC:data_len;
                    revice_ok=1;
                }
            }
            tcp_recved_EMAC(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
            pbuf_free_EMAC(p);   //释放内存
            ret_err=ERR_OK;
        }
    }else//服务器关闭了
    {
        tcp_recved_EMAC(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
        es->p=NULL;
        pbuf_free_EMAC(p); //释放内存
        ret_err=ERR_OK;
    }
    return ret_err;
}

/**
 * 基于TCP协议，出错的回调函数.
 *
 * */
void ETH_server_error_EMAC(void *arg,err_t err)
{
    LWIP_UNUSED_ARG(err);
    if(arg!=NULL)mem_free_EMAC(arg);//释放内存
}
/**
 * 基于TCP协议，发送数据.
 * psocket：发送数据的网口参数
 * buff：发送的数据
 * len：发送的数据长度
 * 使用：此函数根据传入的网口参数判断网口是否已建立连接，如果建立了连接就将数据存放到发送缓冲区中，等待调度发送出去。
 * */
void ETH_TCP_Tx_EMAC(socket * psocket, uint8_t *buff, int len)
{
    uint8_t i=0;
    for(i=0;i<ETH_TCP_connect_num_EMAC;i++)
    {
        if(psocket->oport==ETH_TCP_EMAC[i].oport && psocket->oIP==ETH_TCP_EMAC[i].oIP)
        {
            if(len<ETH_SEND_BUFF_MAX_LEN_EMAC)
            {
                memcpy(ETH_TCP_EMAC[i].send_data,buff,len);
                ETH_TCP_EMAC[i].send_data_len=len;
            }
            else
            {
                memcpy(ETH_TCP_EMAC[i].send_data,buff,ETH_SEND_BUFF_MAX_LEN_EMAC);
                ETH_TCP_EMAC[i].send_data_len=ETH_SEND_BUFF_MAX_LEN_EMAC;
            }
            ETH_TCP_EMAC[i].isSend=1;
            break;
        }
    }
}
/**
 * 基于TCP协议，发送数据.
 * 工作：TCP维护，当发现发送缓冲区有发送数据时，将其发送。
 * */
err_t ETH_server_poll_EMAC(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    uint8_t i=0;
    struct tcp_server_struct *es;
    es=(struct tcp_server_struct *)arg;
    if(es!=NULL)
    {
        for(i=0;i<ETH_TCP_connect_num_EMAC;i++)
        {
            if(tpcb->remote_port==ETH_TCP_EMAC[i].oport && tpcb->remote_ip.addr==ETH_TCP_EMAC[i].oIP && ETH_TCP_EMAC[i].isSend==1)
            {
                ETH_TCP_EMAC[i].isSend=0;
                tcp_write_EMAC(tpcb,ETH_TCP_EMAC[i].send_data,ETH_TCP_EMAC[i].send_data_len,1);
            }
        }
        if(es->state==ES_TCPSERVER_CLOSING)//需要关闭连接?执行关闭操作
        {
            ETH_server_connection_close_EMAC(tpcb,es);//关闭连接
        }
        ret_err=ERR_OK;
    }else
    {
        tcp_abort_EMAC(tpcb);//终止连接,删除pcb控制块
        ret_err=ERR_ABRT;
    }
    return ret_err;
}
/**
 * 基于TCP协议，发送数据.
 * 使用：用于调度使用
 * */
err_t ETH_server_sent_EMAC(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(len);
    es = (struct tcp_server_struct *) arg;
    if(es->p)ETH_server_senddata_EMAC(tpcb,es);//发送数据
    return ERR_OK;
}
/**
 * 基于TCP协议，发送数据.
 * 使用：用于调度使用
 * */
void ETH_server_senddata_EMAC(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
    struct pbuf *ptr;
    uint16 plen;
    err_t wr_err=ERR_OK;
     while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
     {
        ptr=es->p;
        wr_err=tcp_write_EMAC(tpcb,ptr->payload,ptr->len,1);
        if(wr_err==ERR_OK)
        {
            plen=ptr->len;
            es->p=ptr->next;            //指向下一个pbuf
            if(es->p)pbuf_ref_EMAC(es->p);   //pbuf的ref加一
            pbuf_free_EMAC(ptr);
            tcp_recved_EMAC(tpcb,plen);      //更新tcp窗口大小
        }else if(wr_err==ERR_MEM)es->p=ptr;
     }
}
/**
 * 基于TCP协议，断开服务器.
 * 使用：用于调度使用
 * */
void ETH_server_connection_close_EMAC(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
    tcp_close_EMAC(tpcb);
    tcp_arg_EMAC(tpcb,NULL);
    tcp_sent_EMAC(tpcb,NULL);
    tcp_recv_EMAC(tpcb,NULL);
    tcp_err_EMAC(tpcb,NULL);
    tcp_poll_EMAC(tpcb,NULL,0);
    if(es)mem_free_EMAC(es);
}

/**
 * 基于TCP协议，lwip维护.
 * 使用：对基于lwip协议的调度维护。用于主循环中使用。
 * */
void EMAC_lwip_periodic_handle(void)
{
    EMACTxIntPulseDisable(EMAC_0_BASE, EMAC_CTRL_0_BASE, 0, 0);
    EMACRxIntPulseDisable(EMAC_0_BASE, EMAC_CTRL_0_BASE, 0, 0);
    g_ui32LocalTimer += 10;
    lwip_periodic_handle_EMAC();
    EMACTxIntPulseEnable(EMAC_0_BASE, EMAC_CTRL_0_BASE, 0, 0);
    EMACRxIntPulseEnable(EMAC_0_BASE, EMAC_CTRL_0_BASE, 0, 0);
}
