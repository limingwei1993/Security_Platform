/*
 * ETH_EMAC.c
 *
 *  Created on: 2021��11��11��
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
 * ETH��ʼ�����̶�IP
 * ethx���������ڲ�����
 * */
void ETH_init_EMAC(ETH_Info * ethx)
{
        unsigned int   ipAddr;
        uint8 UDP_Iaddr[4] = { 192, 168, 0, 2 };
        uint8 netmask[4] = { 255, 255, 255, 0 };
        uint8 gateway[4] = { 192, 168, 0, 4 };
        uint8 macAddress[6] = { 0x00U, 0x08U, 0xEEU, 0x03U, 0xA6U, 0x6CU };
        //Ĭ�ϱ���IPΪ
        UDP_Iaddr[0]=(ethx->IP>>24)&0xff;
        UDP_Iaddr[1]=(ethx->IP>>16)&0xff;
        UDP_Iaddr[2]=(ethx->IP>>8)&0xff;
        UDP_Iaddr[3]=(ethx->IP>>0)&0xff;
        //Ĭ����������
        netmask[0]=(ethx->mask>>24)&0xff;
        netmask[1]=(ethx->mask>>16)&0xff;
        netmask[2]=(ethx->mask>>8)&0xff;
        netmask[3]=(ethx->mask>>0)&0xff;
        //Ĭ������
        gateway[0]=(ethx->gate>>24)&0xff;
        gateway[1]=(ethx->gate>>16)&0xff;
        gateway[2]=(ethx->gate>>8)&0xff;
        gateway[3]=(ethx->gate>>0)&0xff;

        //ʹ�þ�̬IP
        IntMasterIRQEnable();
        ipAddr = lwIPInit(0, macAddress, *((uint32_t *)UDP_Iaddr), *((uint32_t *)netmask), *((uint32_t *)gateway), IPADDR_USE_STATIC);
}
/**
 * ETH��ʼ����DHCP����
 * timeout ��ʱ
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
            return ethx;//�������ʧ��
        }
        else//������ӳɹ���
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
 * ����TCPЭ�飬��Ϊ�ͻ������ӷ�����.
 * psocket:����������
 * ����������һ���ͻ��ˣ����ݴ���Ĳ������ӷ�������
 * */
void ETH_TCP_Server_bind_EMAC(socket * psocket)
{
    err_t err;
    struct tcp_pcb *tcppcb;     //����һ��TCP���������ƿ�
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
    tcppcb=tcp_new_EMAC();   //����һ���µ�pcb
    if(tcppcb)          //�����ɹ�
    {
        err=tcp_bind_EMAC(tcppcb,(ip_addr_t *)UDP_Iaddr,psocket->port);   //������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
        if(err==ERR_OK) //�����
        {
            tcp_connect_EMAC(tcppcb,(ip_addr_t *)UDP_Oaddr,psocket->oport,ETH_TCP_client_connected_EMAC);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
        }
    }
}
/**
 * ����TCPЭ�飬��Ϊ�����������ͻ���.
 * psocket:����������
 * ���������ݴ���Ĳ�������һ�������������������״̬��
 * */
void ETH_TCP_Server_lisiten_EMAC(socket * psocket)
{
    err_t err;
    struct tcp_pcb *tcppcbnew;      //����һ��TCP���������ƿ�
    struct tcp_pcb *tcppcbconn;     //����һ��TCP���������ƿ�
    uint8 UDP_Iaddr[4] = { 192, 168, 0, 2 };
    UDP_Iaddr[0]=(uint8_t)((psocket->IP>>24)&0xff);
    UDP_Iaddr[1]=(uint8_t)((psocket->IP>>16)&0xff);
    UDP_Iaddr[2]=(uint8_t)((psocket->IP>>8)&0xff);
    UDP_Iaddr[3]=(uint8_t)((psocket->IP>>0)&0xff);
    tcppcbnew=tcp_new_EMAC();    //����һ���µ�pcb
    if(tcppcbnew)           //�����ɹ�
    {
        err=tcp_bind_EMAC(tcppcbnew,(ip_addr_t *)UDP_Iaddr,psocket->port);   //������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
        if(err==ERR_OK) //�����
        {
            tcppcbconn=tcp_listen(tcppcbnew);           //����tcppcb�������״̬
            tcp_accept_EMAC(tcppcbconn,ETH_TCP_Server_accept_EMAC);   //��ʼ��LWIP��tcp_accept�Ļص�����
        }
    }
}
/**
 * ����TCPЭ�飬��Ϊ�ͻ������ӷ�������Ļص�����.
 * �����������ӳɹ��ķ�������IP�Ͷ˿ںŽ��б��档
 * */
err_t ETH_TCP_client_connected_EMAC(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct tcp_client_struct *es=NULL;
    if(err==ERR_OK)
    {
        es=(struct tcp_client_struct*)mem_malloc_EMAC(sizeof(struct tcp_client_struct));  //�����ڴ�
        if(es) //�ڴ�����ɹ�
        {
            es->state=ES_TCPCLIENT_CONNECTED;//״̬Ϊ���ӳɹ�
            es->pcb=tpcb;
            es->p=NULL;
            tcp_arg_EMAC(tpcb,es);                   //ʹ��es����tpcb��callback_arg
            tcp_recv_EMAC(tpcb,ETHx_TCP_Rx_EMAC);   //��ʼ��tcp_recv()�Ļص�����
            tcp_err_EMAC(tpcb,ETH_server_error_EMAC);   //��ʼ��tcp_err()�ص�����
            tcp_poll_EMAC(tpcb,ETH_server_poll_EMAC,1); //��ʼ��tcp_poll�ص�����
            tcp_sent_EMAC(tpcb,ETH_server_sent_EMAC);   //��ʼ�����ͻص�����
            psocket_server_EMAC[ETH_TCP_connect_num_EMAC].isConnected=1;              //����пͻ���������
            psocket_server_EMAC[ETH_TCP_connect_num_EMAC].oIP=tpcb->remote_ip.addr;
            psocket_server_EMAC[ETH_TCP_connect_num_EMAC].oport=tpcb->remote_port;
            ETH_TCP_EMAC[ETH_TCP_connect_num_EMAC].oport=tpcb->remote_port;
            ETH_TCP_EMAC[ETH_TCP_connect_num_EMAC].oIP=tpcb->remote_ip.addr;
            ETH_TCP_connect_num_EMAC++;
            err=ERR_OK;
        }else
        {
            tcp_client_connection_close_EMAC(tpcb,es);//�ر�����
            err=ERR_MEM;    //�����ڴ�������
        }
    }else
    {
        tcp_client_connection_close_EMAC(tpcb,0);//�ر�����
    }
    return err;
}
/**
 * ����TCPЭ�飬��Ϊ���������ӿͻ��˺�Ļص�����.
 * �����������ӳɹ��Ŀͻ��˵�IP�Ͷ˿ںŽ��б��档
 * */
err_t ETH_TCP_Server_accept_EMAC(void *arg,struct tcp_pcb *newpcb,err_t err)
{
    err_t ret_err;
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(arg);
    LWIP_UNUSED_ARG(err);
    tcp_setprio_EMAC(newpcb,TCP_PRIO_MIN);//�����´�����pcb���ȼ�
    es=(struct tcp_server_struct*)mem_malloc_EMAC(sizeof(struct tcp_server_struct)); //�����ڴ�
    if(es!=NULL) //�ڴ����ɹ�
    {
        es->state=ES_TCPSERVER_ACCEPTED;    //��������
        es->pcb=newpcb;
        es->p=NULL;
        tcp_arg_EMAC(newpcb,es);
        tcp_recv_EMAC(newpcb,ETHx_TCP_Rx_EMAC);   //��ʼ��tcp_recv()�Ļص�����
        tcp_err_EMAC(newpcb,ETH_server_error_EMAC);   //��ʼ��tcp_err()�ص�����
        tcp_poll_EMAC(newpcb,ETH_server_poll_EMAC,1); //��ʼ��tcp_poll�ص�����
        tcp_sent_EMAC(newpcb,ETH_server_sent_EMAC);   //��ʼ�����ͻص�����
        psocket_server_EMAC[ETH_TCP_connect_num_EMAC].isConnected=1;              //����пͻ���������
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
 * ����TCPЭ�飬�������ݺ�Ļص�����.
 * �������Խ��յ������ڲ��������ж��Ƿ��Խ������ӣ�������������������ݴ�ŵ����ջ��������ȴ�ʹ�á�
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
    if(p==NULL) //�ӿͻ��˽��յ�������
    {
        es->state=ES_TCPSERVER_CLOSING;//��Ҫ�ر�TCP ������
        es->p=p;
        ret_err=ERR_OK;
    }else if(err!=ERR_OK)   //�ӿͻ��˽��յ�һ���ǿ�����,��������ĳ��ԭ��err!=ERR_OK
    {
        if(p)pbuf_free_EMAC(p);  //�ͷŽ���pbuf
        ret_err=err;
    }else if(es->state==ES_TCPSERVER_ACCEPTED)  //��������״̬
    {
        if(p!=NULL)  //����������״̬���ҽ��յ������ݲ�Ϊ��ʱ�����ӡ����
        {
            for(i=0;i<ETH_TCP_connect_num_EMAC;i++)
            {
                if(ETH_TCP_EMAC[i].oIP==tpcb->remote_ip.addr && ETH_TCP_EMAC[i].oport==tpcb->remote_port)
                {
                    memset(ETH_TCP_EMAC[i].revice_data,0,ETH_REVICE_BUFF_MAX_LEN_EMAC);  //���ݽ��ջ���������
                    for(q=p;q!=NULL;q=q->next)  //����������pbuf����
                    {
                        //�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
                        //�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
                        if(q->len > (ETH_REVICE_BUFF_MAX_LEN_EMAC-data_len))
                        {
                            memcpy(ETH_TCP_EMAC[i].revice_data+data_len,q->payload,(ETH_REVICE_BUFF_MAX_LEN_EMAC-data_len));//��������
                        }
                        else
                        {
                            memcpy(ETH_TCP_EMAC[i].revice_data+data_len,q->payload,q->len);
                        }
                        data_len += q->len;
                        if(data_len > ETH_REVICE_BUFF_MAX_LEN_EMAC) break; //����TCP�ͻ��˽�������,����
                    }
                    ETH_TCP_EMAC[i].isRevice=1;
                    ETH_TCP_EMAC[i].revice_data_len=data_len>ETH_REVICE_BUFF_MAX_LEN_EMAC?ETH_REVICE_BUFF_MAX_LEN_EMAC:data_len;
                    revice_ok=1;
                }
            }
            tcp_recved_EMAC(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
            pbuf_free_EMAC(p);   //�ͷ��ڴ�
            ret_err=ERR_OK;
        }
    }else//�������ر���
    {
        tcp_recved_EMAC(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
        es->p=NULL;
        pbuf_free_EMAC(p); //�ͷ��ڴ�
        ret_err=ERR_OK;
    }
    return ret_err;
}

/**
 * ����TCPЭ�飬����Ļص�����.
 *
 * */
void ETH_server_error_EMAC(void *arg,err_t err)
{
    LWIP_UNUSED_ARG(err);
    if(arg!=NULL)mem_free_EMAC(arg);//�ͷ��ڴ�
}
/**
 * ����TCPЭ�飬��������.
 * psocket���������ݵ����ڲ���
 * buff�����͵�����
 * len�����͵����ݳ���
 * ʹ�ã��˺������ݴ�������ڲ����ж������Ƿ��ѽ������ӣ�������������Ӿͽ����ݴ�ŵ����ͻ������У��ȴ����ȷ��ͳ�ȥ��
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
 * ����TCPЭ�飬��������.
 * ������TCPά���������ַ��ͻ������з�������ʱ�����䷢�͡�
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
        if(es->state==ES_TCPSERVER_CLOSING)//��Ҫ�ر�����?ִ�йرղ���
        {
            ETH_server_connection_close_EMAC(tpcb,es);//�ر�����
        }
        ret_err=ERR_OK;
    }else
    {
        tcp_abort_EMAC(tpcb);//��ֹ����,ɾ��pcb���ƿ�
        ret_err=ERR_ABRT;
    }
    return ret_err;
}
/**
 * ����TCPЭ�飬��������.
 * ʹ�ã����ڵ���ʹ��
 * */
err_t ETH_server_sent_EMAC(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct tcp_server_struct *es;
    LWIP_UNUSED_ARG(len);
    es = (struct tcp_server_struct *) arg;
    if(es->p)ETH_server_senddata_EMAC(tpcb,es);//��������
    return ERR_OK;
}
/**
 * ����TCPЭ�飬��������.
 * ʹ�ã����ڵ���ʹ��
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
            es->p=ptr->next;            //ָ����һ��pbuf
            if(es->p)pbuf_ref_EMAC(es->p);   //pbuf��ref��һ
            pbuf_free_EMAC(ptr);
            tcp_recved_EMAC(tpcb,plen);      //����tcp���ڴ�С
        }else if(wr_err==ERR_MEM)es->p=ptr;
     }
}
/**
 * ����TCPЭ�飬�Ͽ�������.
 * ʹ�ã����ڵ���ʹ��
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
 * ����TCPЭ�飬lwipά��.
 * ʹ�ã��Ի���lwipЭ��ĵ���ά����������ѭ����ʹ�á�
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
