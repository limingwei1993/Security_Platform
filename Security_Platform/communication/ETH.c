/*
 * ETH.c
 *
 *  Created on: 2021��10��13��
 *      Author: 15755
 */

#include "ETH.h"
#include "DM9000.h"
#include "stdio.h"
#include "string.h"
uint32 TCPTimer=0;
uint32 ARPTimer=0;
uint32 lwip_localtime;/*LWIPʱ�����*/
/*ΪLWIP�����ڴ�*/
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
struct netif lwip_netif;                //����һ��ȫ�ֵ�����ӿ�
socket  psocket_server[ETH_TCP_NUM];
uint8_t ETHx_MSG_buff[ETH_REVICE_BUFF_MAX_LEN]={0};
uint32_t ETH_send_data_len=0;
ETH_FRAM ETH_TCP[ETH_TCP_NUM];
uint8_t ETH_TCP_connect_num=0;

extern uint32 memp_get_memorysize(void);
extern u8_t *memp_memory;
extern u8_t *ram_heap;
/**
 * lwip ��mem��memp���ڴ��ͷ�
 * */
void lwip_comm_mem_free(void)
{

}
/**
 * lwip ��mem��memp���ڴ�����
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
 * ETH��ʼ�����̶�IP
 * ethx���������ڲ�����
 * */
void ETH_init(ETH_Info * ethx)
{
        struct netif *Netif_Init_Flag;      //����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
        struct ip_addr ipaddr;              //ip��ַ
        struct ip_addr netmask;             //��������
        struct ip_addr gw;                  //Ĭ������
        if(lwip_comm_mem_malloc())return ; //�ڴ�����ʧ��
        if(DM9000_Init())return ;      //��ʼ��DM9000AEP
        lwip_init();                        //��ʼ��LWIP�ں�
        //Ĭ��Զ��IP
        lwipdev.remoteip[0]=192;
        lwipdev.remoteip[1]=168;
        lwipdev.remoteip[2]=1;
        lwipdev.remoteip[3]=104;
        //MAC��ַ
        lwipdev.mac[0]=0;
        lwipdev.mac[1]=1;
        lwipdev.mac[2]=2;
        lwipdev.mac[3]=3;
        lwipdev.mac[4]=4;
        lwipdev.mac[5]=5;
        //Ĭ�ϱ���IPΪ
        lwipdev.ip[0]=(ethx->IP>>24)&0xff;
        lwipdev.ip[1]=(ethx->IP>>16)&0xff;
        lwipdev.ip[2]=(ethx->IP>>8)&0xff;
        lwipdev.ip[3]=(ethx->IP>>0)&0xff;
        //Ĭ����������
        lwipdev.netmask[0]=(ethx->mask>>24)&0xff;
        lwipdev.netmask[1]=(ethx->mask>>16)&0xff;
        lwipdev.netmask[2]=(ethx->mask>>8)&0xff;
        lwipdev.netmask[3]=(ethx->mask>>0)&0xff;
        //Ĭ������
        lwipdev.gateway[0]=(ethx->gate>>24)&0xff;
        lwipdev.gateway[1]=(ethx->gate>>16)&0xff;
        lwipdev.gateway[2]=(ethx->gate>>8)&0xff;
        lwipdev.gateway[3]=(ethx->gate>>0)&0xff;
        lwipdev.dhcpstatus=0;//û��DHCP
        //ʹ�þ�̬IP
        IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
        IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
        IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
        Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&ethernet_input);//�������б������һ������
        if(Netif_Init_Flag==NULL)return;//�������ʧ��
        else//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
        {
            netif_set_default(&lwip_netif); //����netifΪĬ������
            netif_set_up(&lwip_netif);      //��netif����
        }
}
/**
 * ETH��ʼ����DHCP����
 * timeout ��ʱ
 * */
ETH_Info ETH_DHCP_init(uint32 timeout)
{
    struct netif *Netif_Init_Flag;      //����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
    struct ip_addr ipaddr;              //ip��ַ
    struct ip_addr netmask;             //��������
    struct ip_addr gw;                  //Ĭ������
    ETH_Info ethx;
    ethx.IP=0;
    ethx.gate=0;
    if(lwip_comm_mem_malloc())return ethx; //�ڴ�����ʧ��
    if(DM9000_Init())return ethx;      //��ʼ��DM9000AEP
    lwip_init();                        //��ʼ��LWIP�ں�
    //Ĭ��Զ��IP
    lwipdev.remoteip[0]=192;
    lwipdev.remoteip[1]=168;
    lwipdev.remoteip[2]=1;
    lwipdev.remoteip[3]=104;
    //MAC��ַ
    lwipdev.mac[0]=0;
    lwipdev.mac[1]=1;
    lwipdev.mac[2]=2;
    lwipdev.mac[3]=3;
    lwipdev.mac[4]=4;
    lwipdev.mac[5]=5;
    //Ĭ�ϱ���IPΪ
    lwipdev.ip[0]=192;
    lwipdev.ip[1]=168;
    lwipdev.ip[2]=1;
    lwipdev.ip[3]=2;
    //Ĭ����������
    lwipdev.netmask[0]=255;
    lwipdev.netmask[1]=255;
    lwipdev.netmask[2]=255;
    lwipdev.netmask[3]=0;
    //Ĭ������
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
        Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&ethernet_input);//�������б������һ������
        lwipdev.dhcpstatus=0;   //DHCP���Ϊ0
        dhcp_start(&lwip_netif);    //����DHCP����
        if(Netif_Init_Flag==NULL)return ethx;//�������ʧ��
        else//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
        {
            netif_set_default(&lwip_netif); //����netifΪĬ������
            netif_set_up(&lwip_netif);      //��netif����
            ethx.IP=(uint32_t)(lwip_netif.ip_addr.addr & 0xffffffff);
            ethx.mask=(uint32_t)(lwip_netif.netmask.addr & 0xffffffff);
            ethx.gate=(uint32_t)(lwip_netif.gw.addr & 0xffffffff);
            return ethx;
        }
    }
    return ethx;
}
/**
 * ����TCPЭ�飬��Ϊ�ͻ������ӷ�����.
 * psocket:����������
 * ����������һ���ͻ��ˣ����ݴ���Ĳ������ӷ�������
 * */
void ETH_TCP_Server_bind(socket * psocket)
{
    struct tcp_pcb *tcppcb;     //����һ��TCP���������ƿ�
    struct ip_addr rmtipaddr;   //Զ��ip��ַ
    u8 ip[4]={0,0,0,0};
    ip[0]=(uint8_t)((psocket->oIP>>24)&0xff);
    ip[1]=(uint8_t)((psocket->oIP>>16)&0xff);
    ip[2]=(uint8_t)((psocket->oIP>>8)&0xff);
    ip[3]=(uint8_t)((psocket->oIP>>0)&0xff);
    tcppcb=tcp_new();   //����һ���µ�pcb
    if(tcppcb)          //�����ɹ�
    {
        IP4_ADDR(&rmtipaddr,ip[0],ip[1],ip[2],ip[3]);
        tcp_connect(tcppcb,&rmtipaddr,psocket->oport,ETH_TCP_client_connected);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
    }
}
/**
 * ����TCPЭ�飬��Ϊ�����������ͻ���.
 * psocket:����������
 * ���������ݴ���Ĳ�������һ�������������������״̬��
 * */
void ETH_TCP_Server_lisiten(socket * psocket)
{
    err_t err;
    struct tcp_pcb *tcppcbnew;      //����һ��TCP���������ƿ�
    struct tcp_pcb *tcppcbconn;     //����һ��TCP���������ƿ�

    tcppcbnew=tcp_new();    //����һ���µ�pcb
    if(tcppcbnew)           //�����ɹ�
    {
        err=tcp_bind(tcppcbnew,IP_ADDR_ANY,psocket->port);   //������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
        if(err==ERR_OK) //�����
        {
            tcppcbconn=tcp_listen(tcppcbnew);           //����tcppcb�������״̬
            tcp_accept(tcppcbconn,ETH_TCP_Server_accept);   //��ʼ��LWIP��tcp_accept�Ļص�����
        }
    }
}
/**
 * ����TCPЭ�飬��Ϊ�ͻ������ӷ�������Ļص�����.
 * �����������ӳɹ��ķ�������IP�Ͷ˿ںŽ��б��档
 * */
err_t ETH_TCP_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct tcp_client_struct *es=NULL;
    if(err==ERR_OK)
    {
        es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //�����ڴ�
        if(es) //�ڴ�����ɹ�
        {
            es->state=ES_TCPCLIENT_CONNECTED;//״̬Ϊ���ӳɹ�
            es->pcb=tpcb;
            es->p=NULL;
            tcp_arg(tpcb,es);                   //ʹ��es����tpcb��callback_arg
            tcp_recv(tpcb,ETHx_TCP_Rx);   //��ʼ��tcp_recv()�Ļص�����
            tcp_err(tpcb,ETH_server_error);   //��ʼ��tcp_err()�ص�����
            tcp_poll(tpcb,ETH_server_poll,1); //��ʼ��tcp_poll�ص�����
            tcp_sent(tpcb,ETH_server_sent);   //��ʼ�����ͻص�����
            psocket_server[ETH_TCP_connect_num].isConnected=1;              //����пͻ���������
            psocket_server[ETH_TCP_connect_num].oIP=tpcb->remote_ip.addr;
            psocket_server[ETH_TCP_connect_num].oport=tpcb->remote_port;
            ETH_TCP[ETH_TCP_connect_num].oport=tpcb->remote_port;
            ETH_TCP[ETH_TCP_connect_num].oIP=tpcb->remote_ip.addr;
            ETH_TCP_connect_num++;
            err=ERR_OK;
        }else
        {
            tcp_client_connection_close(tpcb,es);//�ر�����
            err=ERR_MEM;    //�����ڴ�������
        }
    }else
    {
        tcp_client_connection_close(tpcb,0);//�ر�����
    }
    return err;
}
/**
 * ����TCPЭ�飬��Ϊ���������ӿͻ��˺�Ļص�����.
 * �����������ӳɹ��Ŀͻ��˵�IP�Ͷ˿ںŽ��б��档
 * */
err_t ETH_TCP_Server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
    err_t ret_err;
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    tcp_setprio(newpcb,TCP_PRIO_MIN);//�����´�����pcb���ȼ�
    es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct)); //�����ڴ�
    if(es!=NULL) //�ڴ����ɹ�
    {
        es->state=ES_TCPSERVER_ACCEPTED;    //��������
        es->pcb=newpcb;
        es->p=NULL;
        tcp_arg(newpcb,es);
        tcp_recv(newpcb,ETHx_TCP_Rx);   //��ʼ��tcp_recv()�Ļص�����
        tcp_err(newpcb,ETH_server_error);   //��ʼ��tcp_err()�ص�����
        tcp_poll(newpcb,ETH_server_poll,1); //��ʼ��tcp_poll�ص�����
        tcp_sent(newpcb,ETH_server_sent);   //��ʼ�����ͻص�����
        psocket_server[ETH_TCP_connect_num].isConnected=1;              //����пͻ���������
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
 * ����TCPЭ�飬�������ݺ�Ļص�����.
 * �������Խ��յ������ڲ��������ж��Ƿ��Խ������ӣ�������������������ݴ�ŵ����ջ��������ȴ�ʹ�á�
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
    if(p==NULL) //�ӿͻ��˽��յ�������
    {
        es->state=ES_TCPSERVER_CLOSING;//��Ҫ�ر�TCP ������
        es->p=p;
        ret_err=ERR_OK;
    }else if(err!=ERR_OK)   //�ӿͻ��˽��յ�һ���ǿ�����,��������ĳ��ԭ��err!=ERR_OK
    {
        if(p)pbuf_free(p);  //�ͷŽ���pbuf
        ret_err=err;
    }else if(es->state==ES_TCPSERVER_ACCEPTED)  //��������״̬
    {
        if(p!=NULL)  //����������״̬���ҽ��յ������ݲ�Ϊ��ʱ�����ӡ����
        {
            for(i=0;i<ETH_TCP_connect_num;i++)
            {
                if(ETH_TCP[i].oIP==tpcb->remote_ip.addr && ETH_TCP[i].oport==tpcb->remote_port)
                {
                    memset(ETH_TCP[i].revice_data,0,ETH_REVICE_BUFF_MAX_LEN);  //���ݽ��ջ���������
                    for(q=p;q!=NULL;q=q->next)  //����������pbuf����
                    {
                        //�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
                        //�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
                        if(q->len > (ETH_REVICE_BUFF_MAX_LEN-data_len))
                        {
                            memcpy(ETH_TCP[i].revice_data+data_len,q->payload,(ETH_REVICE_BUFF_MAX_LEN-data_len));//��������
                        }
                        else
                        {
                            memcpy(ETH_TCP[i].revice_data+data_len,q->payload,q->len);
                        }
                        data_len += q->len;
                        if(data_len > ETH_REVICE_BUFF_MAX_LEN) break; //����TCP�ͻ��˽�������,����
                    }
                    ETH_TCP[i].isRevice=1;
                    ETH_TCP[i].revice_data_len=data_len>ETH_REVICE_BUFF_MAX_LEN?ETH_REVICE_BUFF_MAX_LEN:data_len;
                }
            }
            tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
            pbuf_free(p);   //�ͷ��ڴ�
            ret_err=ERR_OK;
        }
    }else//�������ر���
    {
        tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
        es->p=NULL;
        pbuf_free(p); //�ͷ��ڴ�
        ret_err=ERR_OK;
    }
    return ret_err;
}

/**
 * ����TCPЭ�飬����Ļص�����.
 *
 * */
void ETH_server_error(void *arg,err_t err)
{
    LWIP_UNUSED_ARG(err);
    if(arg!=NULL)mem_free(arg);//�ͷ��ڴ�
}
/**
 * ����TCPЭ�飬��������.
 * psocket���������ݵ����ڲ���
 * buff�����͵�����
 * len�����͵����ݳ���
 * ʹ�ã��˺������ݴ�������ڲ����ж������Ƿ��ѽ������ӣ�������������Ӿͽ����ݴ�ŵ����ͻ������У��ȴ����ȷ��ͳ�ȥ��
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
 * ����TCPЭ�飬��������.
 * ������TCPά���������ַ��ͻ������з�������ʱ�����䷢�͡�
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
        if(es->state==ES_TCPSERVER_CLOSING)//��Ҫ�ر�����?ִ�йرղ���
        {
            ETH_server_connection_close(tpcb,es);//�ر�����
        }
        ret_err=ERR_OK;
    }else
    {
        tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
        ret_err=ERR_ABRT;
    }
    return ret_err;
}
/**
 * ����TCPЭ�飬��������.
 * ʹ�ã����ڵ���ʹ��
 * */
err_t ETH_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(len);
    es = (struct tcp_server_struct *) arg;
    if(es->p)ETH_server_senddata(tpcb,es);//��������
    return ERR_OK;
}
/**
 * ����TCPЭ�飬��������.
 * ʹ�ã����ڵ���ʹ��
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
            es->p=ptr->next;            //ָ����һ��pbuf
            if(es->p)pbuf_ref(es->p);   //pbuf��ref��һ
            pbuf_free(ptr);
            tcp_recved(tpcb,plen);      //����tcp���ڴ�С
        }else if(wr_err==ERR_MEM)es->p=ptr;
     }
}
/**
 * ����TCPЭ�飬�Ͽ�������.
 * ʹ�ã����ڵ���ʹ��
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
extern void tcp_pcb_purge(struct tcp_pcb *pcb); //�� tcp.c����
extern struct tcp_pcb *tcp_active_pcbs;         //�� tcp.c����
extern struct tcp_pcb *tcp_tw_pcbs;             //�� tcp.c����
/**
 * ����TCPЭ�飬�������Ͽ�.
 * ʹ�ã�ǿ��ɾ��TCP Server�����Ͽ�ʱ��time wait
 * */
void ETH_server_remove_timewait(void)
{
    struct tcp_pcb *pcb,*pcb2;

    while(tcp_active_pcbs!=NULL)
    {
        lwip_periodic_handle(); //������ѯ
        lwip_pkt_handle();
        delay_ms(10);           //�ȴ�tcp_active_pcbsΪ��
    }
    pcb=tcp_tw_pcbs;
    while(pcb!=NULL)//����еȴ�״̬��pcbs
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
 * ����DHCP��ά��.
 * ʹ�ã��Ի���DHCPЭ��ĵ���ά����
 * */
void lwip_dhcp_process_handle(void)
{
    u32 ip=0,netmask=0,gw=0;
    switch(lwipdev.dhcpstatus)
    {
        case 0:     /*����DHCP*/
            dhcp_start(&lwip_netif);
            lwipdev.dhcpstatus = 1;    /*�ȴ�ͨ��DHCP��ȡ���ĵ�ַ*/
            break;
        case 1:      /*�ȴ���ȡ��IP��ַ*/
        {
            ip=lwip_netif.ip_addr.addr;      /*��ȡ��IP��ַ*/
            netmask=lwip_netif.netmask.addr;   /*��ȡ��������*/
            gw=lwip_netif.gw.addr;           /*��ȡĬ������*/

            if(ip!=0)            /*��ȷ��ȡ��IP��ַ��ʱ��*/
            {
                lwipdev.dhcpstatus=2;    /*DHCP�ɹ�*/
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
            }else if(lwip_netif.dhcp->tries>LWIP_MAX_DHCP_TRIES)  /*ͨ��DHCP�����ȡIP��ַʧ�ܣ��ҳ�������Դ���*/
            {
                lwipdev.dhcpstatus=0XFF;  /*DHCP��ʱʧ��*/
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
 * ����TCPЭ�飬lwipά��.
 * ʹ�ã��Ի���lwipЭ��ĵ���ά����������ѭ����ʹ�á�
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
    if (lwip_localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS) /*ÿ500ms����һ��dhcp_fine_tmr()*/
    {
        DHCPfineTimer =  lwip_localtime;
        dhcp_fine_tmr();
        if ((lwipdev.dhcpstatus != 2)&&(lwipdev.dhcpstatus != 0XFF))
        {
            lwip_dhcp_process_handle(); /*DHCP����*/
        }
    }
    if (lwip_localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS) /*ÿ60sִ������DHCP�ִ���*/
    {
        DHCPcoarseTimer =  lwip_localtime;
        dhcp_coarse_tmr();
    }
}

/**
 * ����TCPЭ�飬�ͻ��˶Ͽ�.
 * ʹ�ã�ǿ��ɾ��TCP Server�����Ͽ�ʱ��time wait
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
