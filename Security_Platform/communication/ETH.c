/*
 * ETH.c
 *
 *  Created on: 2021年10月13日
 *      Author: 15755
 */

#include "ETH.h"
#include "DM9000.h"
#include "stdio.h"
#include "string.h"
uint32 TCPTimer=0;
uint32 ARPTimer=0;
uint32 lwip_localtime;/*LWIP时间计数*/
/*为LWIP开辟内存*/
u8_t memp_memory_buff[MEM_ALIGNMENT - 1
#if LWIP_RAW
+MEMP_NUM_RAW_PCB*((((sizeof(struct raw_pcb)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if LWIP_UDP
+MEMP_NUM_UDP_PCB*((((sizeof(struct udp_pcb)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if LWIP_TCP
+MEMP_NUM_TCP_PCB*((((sizeof(struct tcp_pcb)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
+MEMP_NUM_TCP_PCB_LISTEN*((((sizeof(struct tcp_pcb_listen)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
+MEMP_NUM_TCP_SEG*((((sizeof(struct tcp_seg)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if IP_REASSEMBLY
+MEMP_NUM_REASSDATA*((((sizeof(struct ip_reassdata)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if IP_FRAG && !IP_FRAG_USES_STATIC_BUF && !LWIP_NETIF_TX_SINGLE_PBUF
+MEMP_NUM_FRAG_PBUF*((((sizeof(struct pbuf_custom_ref)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if LWIP_NETCONN
+MEMP_NUM_NETBUF*((((sizeof(struct netbuf)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
+MEMP_NUM_NETCONN*((((sizeof(struct netconn)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if NO_SYS==0
+MEMP_NUM_TCPIP_MSG_API*((((sizeof(struct tcpip_msg)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#if !LWIP_TCPIP_CORE_LOCKING_INPUT
+MEMP_NUM_TCPIP_MSG_INPKT*((((sizeof(struct tcpip_msg)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#endif
#if LWIP_ARP && ARP_QUEUEING
+MEMP_NUM_ARP_QUEUE*((((sizeof(struct etharp_q_entry)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if LWIP_IGMP
+MEMP_NUM_IGMP_GROUP*((((sizeof(struct igmp_group)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if (!NO_SYS || (NO_SYS && !NO_SYS_NO_TIMERS))
+MEMP_NUM_SYS_TIMEOUT*((((sizeof(struct sys_timeo)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if LWIP_SNMP
+MEMP_NUM_SNMP_ROOTNODE*((((sizeof(struct mib_list_rootnode)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
+MEMP_NUM_SNMP_NODE*((((sizeof(struct mib_list_node)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
+MEMP_NUM_SNMP_VARBIND*((((sizeof(struct snmp_varbind)) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
+MEMP_NUM_SNMP_VALUE*((((SNMP_MAX_VALUE_SIZE) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if LWIP_DNS && LWIP_SOCKET
+MEMP_NUM_NETDB*((((NETDB_ELEM_SIZE) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if LWIP_DNS && DNS_LOCAL_HOSTLIST && DNS_LOCAL_HOSTLIST_IS_DYNAMIC
+MEMP_NUM_LOCALHOSTLIST*((((LOCALHOSTLIST_ELEM_SIZE) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
#if PPP_SUPPORT && PPPOE_SUPPORT
+MEMP_NUM_PPPOE_INTERFACES*((((sizeof(struct pppoe_softc))) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
#endif
+PBUF_POOL_SIZE*((((PBUF_POOL_BUFSIZE) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
+MEMP_NUM_PBUF*((((0) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1)))
];
//u8_t memp_memory_buff[0x4713];
u8_t ram_heap_buff[(((MEM_SIZE) + MEM_ALIGNMENT - 1) & ~(MEM_ALIGNMENT-1))+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT];
#if LWIP_DHCP
u32 DHCPfineTimer=0;
u32 DHCPcoarseTimer=0;
#endif

__lwip_dev lwipdev;
struct netif lwip_netif;                //定义一个全局的网络接口
socket  psocket_server[ETH_TCP_NUM];
uint8_t ETHx_MSG_buff[ETH_REVICE_BUFF_MAX_LEN]={0};
uint32_t ETH_send_data_len=0;
ETH_FRAM ETH_TCP[ETH_TCP_NUM];
uint8_t ETH_TCP_connect_num=0;

extern uint32 memp_get_memorysize(void);
extern u8_t *memp_memory;
extern u8_t *ram_heap;
/**
 * lwip 中mem和memp的内存释放
 * */
void lwip_comm_mem_free(void)
{

}
/**
 * lwip 中mem和memp的内存申请
 * */
uint8 lwip_comm_mem_malloc(void)
{
    memp_memory=memp_memory_buff;
    ram_heap=ram_heap_buff;
    if(!memp_memory||!ram_heap)
    {
        lwip_comm_mem_free();
        return 1;
    }
    return 0;
}
/**
 * ETH初始化，固定IP
 * ethx：本机网口参数。
 * */
void ETH_init(ETH_Info * ethx)
{
        struct netif *Netif_Init_Flag;      //调用netif_add()函数时的返回值,用于判断网络初始化是否成功
        struct ip_addr ipaddr;              //ip地址
        struct ip_addr netmask;             //子网掩码
        struct ip_addr gw;                  //默认网关
        if(lwip_comm_mem_malloc())return ; //内存申请失败
        if(DM9000_Init())return ;      //初始化DM9000AEP
        lwip_init();                        //初始化LWIP内核
        //默认远端IP
        lwipdev.remoteip[0]=192;
        lwipdev.remoteip[1]=168;
        lwipdev.remoteip[2]=1;
        lwipdev.remoteip[3]=104;
        //MAC地址
        lwipdev.mac[0]=0;
        lwipdev.mac[1]=1;
        lwipdev.mac[2]=2;
        lwipdev.mac[3]=3;
        lwipdev.mac[4]=4;
        lwipdev.mac[5]=5;
        //默认本地IP为
        lwipdev.ip[0]=(ethx->IP>>24)&0xff;
        lwipdev.ip[1]=(ethx->IP>>16)&0xff;
        lwipdev.ip[2]=(ethx->IP>>8)&0xff;
        lwipdev.ip[3]=(ethx->IP>>0)&0xff;
        //默认子网掩码
        lwipdev.netmask[0]=(ethx->mask>>24)&0xff;
        lwipdev.netmask[1]=(ethx->mask>>16)&0xff;
        lwipdev.netmask[2]=(ethx->mask>>8)&0xff;
        lwipdev.netmask[3]=(ethx->mask>>0)&0xff;
        //默认网关
        lwipdev.gateway[0]=(ethx->gate>>24)&0xff;
        lwipdev.gateway[1]=(ethx->gate>>16)&0xff;
        lwipdev.gateway[2]=(ethx->gate>>8)&0xff;
        lwipdev.gateway[3]=(ethx->gate>>0)&0xff;
        lwipdev.dhcpstatus=0;//没有DHCP
        //使用静态IP
        IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
        IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
        IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
        Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&ethernet_input);//向网卡列表中添加一个网口
        if(Netif_Init_Flag==NULL)return;//网卡添加失败
        else//网口添加成功后,设置netif为默认值,并且打开netif网口
        {
            netif_set_default(&lwip_netif); //设置netif为默认网口
            netif_set_up(&lwip_netif);      //打开netif网口
        }
}
/**
 * ETH初始化，DHCP分配
 * timeout 超时
 * */
ETH_Info ETH_DHCP_init(uint32 timeout)
{
    struct netif *Netif_Init_Flag;      //调用netif_add()函数时的返回值,用于判断网络初始化是否成功
    struct ip_addr ipaddr;              //ip地址
    struct ip_addr netmask;             //子网掩码
    struct ip_addr gw;                  //默认网关
    ETH_Info ethx;
    ethx.IP=0;
    ethx.gate=0;
    if(lwip_comm_mem_malloc())return ethx; //内存申请失败
    if(DM9000_Init())return ethx;      //初始化DM9000AEP
    lwip_init();                        //初始化LWIP内核
    //默认远端IP
    lwipdev.remoteip[0]=192;
    lwipdev.remoteip[1]=168;
    lwipdev.remoteip[2]=1;
    lwipdev.remoteip[3]=104;
    //MAC地址
    lwipdev.mac[0]=0;
    lwipdev.mac[1]=1;
    lwipdev.mac[2]=2;
    lwipdev.mac[3]=3;
    lwipdev.mac[4]=4;
    lwipdev.mac[5]=5;
    //默认本地IP为
    lwipdev.ip[0]=192;
    lwipdev.ip[1]=168;
    lwipdev.ip[2]=1;
    lwipdev.ip[3]=2;
    //默认子网掩码
    lwipdev.netmask[0]=255;
    lwipdev.netmask[1]=255;
    lwipdev.netmask[2]=255;
    lwipdev.netmask[3]=0;
    //默认网关
    lwipdev.gateway[0]=192;
    lwipdev.gateway[1]=168;
    lwipdev.gateway[2]=1;
    lwipdev.gateway[3]=0;
    ipaddr.addr = 0;
    netmask.addr = 0;
    gw.addr = 0;
    IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
    IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
    IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
    while(timeout--)
    {
        Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&ethernet_input);//向网卡列表中添加一个网口
        lwipdev.dhcpstatus=0;   //DHCP标记为0
        dhcp_start(&lwip_netif);    //开启DHCP服务
        if(Netif_Init_Flag==NULL)return ethx;//网卡添加失败
        else//网口添加成功后,设置netif为默认值,并且打开netif网口
        {
            netif_set_default(&lwip_netif); //设置netif为默认网口
            netif_set_up(&lwip_netif);      //打开netif网口
            ethx.IP=(uint32_t)(lwip_netif.ip_addr.addr & 0xffffffff);
            ethx.mask=(uint32_t)(lwip_netif.netmask.addr & 0xffffffff);
            ethx.gate=(uint32_t)(lwip_netif.gw.addr & 0xffffffff);
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
void ETH_TCP_Server_bind(socket * psocket)
{
    struct tcp_pcb *tcppcb;     //定义一个TCP服务器控制块
    struct ip_addr rmtipaddr;   //远端ip地址
    u8 ip[4]={0,0,0,0};
    ip[0]=(uint8_t)((psocket->oIP>>24)&0xff);
    ip[1]=(uint8_t)((psocket->oIP>>16)&0xff);
    ip[2]=(uint8_t)((psocket->oIP>>8)&0xff);
    ip[3]=(uint8_t)((psocket->oIP>>0)&0xff);
    tcppcb=tcp_new();   //创建一个新的pcb
    if(tcppcb)          //创建成功
    {
        IP4_ADDR(&rmtipaddr,ip[0],ip[1],ip[2],ip[3]);
        tcp_connect(tcppcb,&rmtipaddr,psocket->oport,ETH_TCP_client_connected);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
    }
}
/**
 * 基于TCP协议，作为服务器监听客户端.
 * psocket:服务器参数
 * 工作：根据传入的参数创建一个服务器，并进入监听状态。
 * */
void ETH_TCP_Server_lisiten(socket * psocket)
{
    err_t err;
    struct tcp_pcb *tcppcbnew;      //定义一个TCP服务器控制块
    struct tcp_pcb *tcppcbconn;     //定义一个TCP服务器控制块

    tcppcbnew=tcp_new();    //创建一个新的pcb
    if(tcppcbnew)           //创建成功
    {
        err=tcp_bind(tcppcbnew,IP_ADDR_ANY,psocket->port);   //将本地IP与指定的端口号绑定在一起,IP_ADDR_ANY为绑定本地所有的IP地址
        if(err==ERR_OK) //绑定完成
        {
            tcppcbconn=tcp_listen(tcppcbnew);           //设置tcppcb进入监听状态
            tcp_accept(tcppcbconn,ETH_TCP_Server_accept);   //初始化LWIP的tcp_accept的回调函数
        }
    }
}
/**
 * 基于TCP协议，作为客户端连接服务器后的回调函数.
 * 工作：对连接成功的服务器的IP和端口号进行保存。
 * */
err_t ETH_TCP_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct tcp_client_struct *es=NULL;
    if(err==ERR_OK)
    {
        es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //申请内存
        if(es) //内存申请成功
        {
            es->state=ES_TCPCLIENT_CONNECTED;//状态为连接成功
            es->pcb=tpcb;
            es->p=NULL;
            tcp_arg(tpcb,es);                   //使用es更新tpcb的callback_arg
            tcp_recv(tpcb,ETHx_TCP_Rx);   //初始化tcp_recv()的回调函数
            tcp_err(tpcb,ETH_server_error);   //初始化tcp_err()回调函数
            tcp_poll(tpcb,ETH_server_poll,1); //初始化tcp_poll回调函数
            tcp_sent(tpcb,ETH_server_sent);   //初始化发送回调函数
            psocket_server[ETH_TCP_connect_num].isConnected=1;              //标记有客户端连上了
            psocket_server[ETH_TCP_connect_num].oIP=tpcb->remote_ip.addr;
            psocket_server[ETH_TCP_connect_num].oport=tpcb->remote_port;
            ETH_TCP[ETH_TCP_connect_num].oport=tpcb->remote_port;
            ETH_TCP[ETH_TCP_connect_num].oIP=tpcb->remote_ip.addr;
            ETH_TCP_connect_num++;
            err=ERR_OK;
        }else
        {
            tcp_client_connection_close(tpcb,es);//关闭连接
            err=ERR_MEM;    //返回内存分配错误
        }
    }else
    {
        tcp_client_connection_close(tpcb,0);//关闭连接
    }
    return err;
}
/**
 * 基于TCP协议，作为服务器连接客户端后的回调函数.
 * 工作：对连接成功的客户端的IP和端口号进行保存。
 * */
err_t ETH_TCP_Server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
    err_t ret_err;
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    tcp_setprio(newpcb,TCP_PRIO_MIN);//设置新创建的pcb优先级
    es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct)); //分配内存
    if(es!=NULL) //内存分配成功
    {
        es->state=ES_TCPSERVER_ACCEPTED;    //接收连接
        es->pcb=newpcb;
        es->p=NULL;
        tcp_arg(newpcb,es);
        tcp_recv(newpcb,ETHx_TCP_Rx);   //初始化tcp_recv()的回调函数
        tcp_err(newpcb,ETH_server_error);   //初始化tcp_err()回调函数
        tcp_poll(newpcb,ETH_server_poll,1); //初始化tcp_poll回调函数
        tcp_sent(newpcb,ETH_server_sent);   //初始化发送回调函数
        psocket_server[ETH_TCP_connect_num].isConnected=1;              //标记有客户端连上了
        psocket_server[ETH_TCP_connect_num].oIP=newpcb->remote_ip.addr;
        psocket_server[ETH_TCP_connect_num].oport=newpcb->remote_port;
        ETH_TCP[ETH_TCP_connect_num].oport=newpcb->remote_port;
        ETH_TCP[ETH_TCP_connect_num].oIP=newpcb->remote_ip.addr;
        ETH_TCP_connect_num++;
        ret_err=ERR_OK;
    };
    return ret_err;
}

/**
 * 基于TCP协议，接收数据后的回调函数.
 * 工作：对接收到的网口参数进行判断是否以建立连接，如果建立了连接则将数据存放到接收缓冲区，等待使用。
 * */
err_t ETHx_TCP_Rx(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
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
        if(p)pbuf_free(p);  //释放接收pbuf
        ret_err=err;
    }else if(es->state==ES_TCPSERVER_ACCEPTED)  //处于连接状态
    {
        if(p!=NULL)  //当处于连接状态并且接收到的数据不为空时将其打印出来
        {
            for(i=0;i<ETH_TCP_connect_num;i++)
            {
                if(ETH_TCP[i].oIP==tpcb->remote_ip.addr && ETH_TCP[i].oport==tpcb->remote_port)
                {
                    memset(ETH_TCP[i].revice_data,0,ETH_REVICE_BUFF_MAX_LEN);  //数据接收缓冲区清零
                    for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
                    {
                        //判断要拷贝到TCP_SERVER_RX_BUFSIZE中的数据是否大于TCP_SERVER_RX_BUFSIZE的剩余空间，如果大于
                        //的话就只拷贝TCP_SERVER_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
                        if(q->len > (ETH_REVICE_BUFF_MAX_LEN-data_len))
                        {
                            memcpy(ETH_TCP[i].revice_data+data_len,q->payload,(ETH_REVICE_BUFF_MAX_LEN-data_len));//拷贝数据
                        }
                        else
                        {
                            memcpy(ETH_TCP[i].revice_data+data_len,q->payload,q->len);
                        }
                        data_len += q->len;
                        if(data_len > ETH_REVICE_BUFF_MAX_LEN) break; //超出TCP客户端接收数组,跳出
                    }
                    ETH_TCP[i].isRevice=1;
                    ETH_TCP[i].revice_data_len=data_len>ETH_REVICE_BUFF_MAX_LEN?ETH_REVICE_BUFF_MAX_LEN:data_len;
                }
            }
            tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
            pbuf_free(p);   //释放内存
            ret_err=ERR_OK;
        }
    }else//服务器关闭了
    {
        tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
        es->p=NULL;
        pbuf_free(p); //释放内存
        ret_err=ERR_OK;
    }
    return ret_err;
}

/**
 * 基于TCP协议，出错的回调函数.
 *
 * */
void ETH_server_error(void *arg,err_t err)
{
    LWIP_UNUSED_ARG(err);
    if(arg!=NULL)mem_free(arg);//释放内存
}
/**
 * 基于TCP协议，发送数据.
 * psocket：发送数据的网口参数
 * buff：发送的数据
 * len：发送的数据长度
 * 使用：此函数根据传入的网口参数判断网口是否已建立连接，如果建立了连接就将数据存放到发送缓冲区中，等待调度发送出去。
 * */
void ETH_TCP_Tx(socket * psocket, uint8_t *buff, int len)
{
    uint8_t i=0;
    for(i=0;i<ETH_TCP_connect_num;i++)
    {
        if(psocket->oport==ETH_TCP[i].oport && psocket->oIP==ETH_TCP[i].oIP)
        {
            if(len<ETH_SEND_BUFF_MAX_LEN)
            {
                memcpy(ETH_TCP[i].send_data,buff,len);
                ETH_TCP[i].send_data_len=len;
            }
            else
            {
                memcpy(ETH_TCP[i].send_data,buff,ETH_SEND_BUFF_MAX_LEN);
                ETH_TCP[i].send_data_len=ETH_SEND_BUFF_MAX_LEN;
            }
            ETH_TCP[i].isSend=1;
            break;
        }
    }
}
/**
 * 基于TCP协议，发送数据.
 * 工作：TCP维护，当发现发送缓冲区有发送数据时，将其发送。
 * */
err_t ETH_server_poll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    uint8_t i=0;
    struct tcp_server_struct *es;
    es=(struct tcp_server_struct *)arg;
    if(es!=NULL)
    {
        for(i=0;i<ETH_TCP_connect_num;i++)
        {
            if(tpcb->remote_port==ETH_TCP[i].oport && tpcb->remote_ip.addr==ETH_TCP[i].oIP && ETH_TCP[i].isSend==1)
            {
                ETH_TCP[i].isSend=0;
                tcp_write(tpcb,ETH_TCP[i].send_data,ETH_TCP[i].send_data_len,1);
            }
        }
        if(es->state==ES_TCPSERVER_CLOSING)//需要关闭连接?执行关闭操作
        {
            ETH_server_connection_close(tpcb,es);//关闭连接
        }
        ret_err=ERR_OK;
    }else
    {
        tcp_abort(tpcb);//终止连接,删除pcb控制块
        ret_err=ERR_ABRT;
    }
    return ret_err;
}
/**
 * 基于TCP协议，发送数据.
 * 使用：用于调度使用
 * */
err_t ETH_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(len);
    es = (struct tcp_server_struct *) arg;
    if(es->p)ETH_server_senddata(tpcb,es);//发送数据
    return ERR_OK;
}
/**
 * 基于TCP协议，发送数据.
 * 使用：用于调度使用
 * */
void ETH_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
    struct pbuf *ptr;
    uint16 plen;
    err_t wr_err=ERR_OK;
     while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
     {
        ptr=es->p;
        wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
        if(wr_err==ERR_OK)
        {
            plen=ptr->len;
            es->p=ptr->next;            //指向下一个pbuf
            if(es->p)pbuf_ref(es->p);   //pbuf的ref加一
            pbuf_free(ptr);
            tcp_recved(tpcb,plen);      //更新tcp窗口大小
        }else if(wr_err==ERR_MEM)es->p=ptr;
     }
}
/**
 * 基于TCP协议，断开服务器.
 * 使用：用于调度使用
 * */
void ETH_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
    tcp_close(tpcb);
    tcp_arg(tpcb,NULL);
    tcp_sent(tpcb,NULL);
    tcp_recv(tpcb,NULL);
    tcp_err(tpcb,NULL);
    tcp_poll(tpcb,NULL,0);
    if(es)mem_free(es);
}
extern void tcp_pcb_purge(struct tcp_pcb *pcb); //在 tcp.c里面
extern struct tcp_pcb *tcp_active_pcbs;         //在 tcp.c里面
extern struct tcp_pcb *tcp_tw_pcbs;             //在 tcp.c里面
/**
 * 基于TCP协议，服务器断开.
 * 使用：强制删除TCP Server主动断开时的time wait
 * */
void ETH_server_remove_timewait(void)
{
    struct tcp_pcb *pcb,*pcb2;

    while(tcp_active_pcbs!=NULL)
    {
        lwip_periodic_handle(); //继续轮询
        lwip_pkt_handle();
        delay_ms(10);           //等待tcp_active_pcbs为空
    }
    pcb=tcp_tw_pcbs;
    while(pcb!=NULL)//如果有等待状态的pcbs
    {
        tcp_pcb_purge(pcb);
        tcp_tw_pcbs=pcb->next;
        pcb2=pcb;
        pcb=pcb->next;
        memp_free(MEMP_TCP_PCB,pcb2);
    }
}


void lwip_pkt_handle(void)
{
    ethernetif_input(&lwip_netif);
}
/**
 * 基于DHCP的维护.
 * 使用：对基于DHCP协议的调度维护。
 * */
void lwip_dhcp_process_handle(void)
{
    u32 ip=0,netmask=0,gw=0;
    switch(lwipdev.dhcpstatus)
    {
        case 0:     /*开启DHCP*/
            dhcp_start(&lwip_netif);
            lwipdev.dhcpstatus = 1;    /*等待通过DHCP获取到的地址*/
            break;
        case 1:      /*等待获取到IP地址*/
        {
            ip=lwip_netif.ip_addr.addr;      /*读取新IP地址*/
            netmask=lwip_netif.netmask.addr;   /*读取子网掩码*/
            gw=lwip_netif.gw.addr;           /*读取默认网关*/

            if(ip!=0)            /*正确获取到IP地址的时候*/
            {
                lwipdev.dhcpstatus=2;    /*DHCP成功*/
                lwipdev.ip[3]=(uint8_t)(ip>>24);
                lwipdev.ip[2]=(uint8_t)(ip>>16);
                lwipdev.ip[1]=(uint8_t)(ip>>8);
                lwipdev.ip[0]=(uint8_t)(ip);
                lwipdev.netmask[3]=(uint8_t)(netmask>>24);
                lwipdev.netmask[2]=(uint8_t)(netmask>>16);
                lwipdev.netmask[1]=(uint8_t)(netmask>>8);
                lwipdev.netmask[0]=(uint8_t)(netmask);
                lwipdev.gateway[3]=(uint8_t)(gw>>24);
                lwipdev.gateway[2]=(uint8_t)(gw>>16);
                lwipdev.gateway[1]=(uint8_t)(gw>>8);
                lwipdev.gateway[0]=(uint8_t)(gw);
            }else if(lwip_netif.dhcp->tries>LWIP_MAX_DHCP_TRIES)  /*通过DHCP服务获取IP地址失败，且超过最大尝试次数*/
            {
                lwipdev.dhcpstatus=0XFF;  /*DHCP超时失败*/
                IP4_ADDR(&(lwip_netif.ip_addr),lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
                IP4_ADDR(&(lwip_netif.netmask),lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
                IP4_ADDR(&(lwip_netif.gw),lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
            }
        }
        break;
        default : break;
    }
}
/**
 * 基于TCP协议，lwip维护.
 * 使用：对基于lwip协议的调度维护。用于主循环中使用。
 * */
void lwip_periodic_handle()
{
    if (lwip_localtime - TCPTimer >= TCP_TMR_INTERVAL)
    {
        TCPTimer =  lwip_localtime;
        tcp_tmr();
    }
    if ((lwip_localtime - ARPTimer) >= ARP_TMR_INTERVAL)
    {
        ARPTimer =  lwip_localtime;
        etharp_tmr();
    }
    if (lwip_localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS) /*每500ms调用一次dhcp_fine_tmr()*/
    {
        DHCPfineTimer =  lwip_localtime;
        dhcp_fine_tmr();
        if ((lwipdev.dhcpstatus != 2)&&(lwipdev.dhcpstatus != 0XFF))
        {
            lwip_dhcp_process_handle(); /*DHCP处理*/
        }
    }
    if (lwip_localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS) /*每60s执行依次DHCP粗处理*/
    {
        DHCPcoarseTimer =  lwip_localtime;
        dhcp_coarse_tmr();
    }
}

/**
 * 基于TCP协议，客户端断开.
 * 使用：强制删除TCP Server主动断开时的time wait
 * */
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
    tcp_abort(tpcb);
    tcp_arg(tpcb,NULL);
    tcp_recv(tpcb,NULL);
    tcp_sent(tpcb,NULL);
    tcp_err(tpcb,NULL);
    tcp_poll(tpcb,NULL,0);
    if(es)mem_free(es);
}
