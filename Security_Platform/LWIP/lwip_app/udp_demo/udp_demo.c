#include "udp_demo.h" 
#include "stdio.h"
#include "string.h" 
extern u8 save;

u8 udp_demo_flag;
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE]={0};
extern u8 test_data[6*1025];
//UDP����
void udp_demo_test(u16 port,u16 portl)
{
 	err_t err;
	struct udp_pcb *udppcb;  	//����һ��  UDP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ
 	u8 ip[4]={192,168,1,113};
	udppcb=udp_new();
	if(udppcb)//�����ɹ�
	{ 
		IP4_ADDR(&rmtipaddr,ip[0],ip[1],ip[2],ip[3]); /*��Զ��IPת��Ϊ��Ҫ�ĸ�ʽ*/
		err=udp_connect(udppcb,&rmtipaddr,port);//UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,portl);//�󶨱���IP��ַ��˿ں�
			if(err==ERR_OK)	//�����
			{				
				udp_recv(udppcb,udp_demo_recv,NULL);//ע����ջص�����
			}
		}		
	}
} 

//UDP�ص�����
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//���յ���Ϊ�յ�����ʱ
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //���ݽ��ջ���������
		for(q=p;q!=NULL;q=q->next)  //����������pbuf����
		{
			//�ж�Ҫ������UDP_DEMO_RX_BUFSIZE�е������Ƿ����UDP_DEMO_RX_BUFSIZE��ʣ��ռ䣬�������
			//�Ļ���ֻ����UDP_DEMO_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//��������
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
		}
		udp_demo_senddata(upcb);
		upcb->remote_ip=*addr; 				//��¼Զ��������IP��ַ
		upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1
		pbuf_free(p);//�ͷ��ڴ�
	}else
	{
		udp_disconnect(upcb); 
	} 
} 
//UDP��������������
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	
	u8 i=0;
	u8 * senddata=test_data;
	for(i=0;i<6;i++)
	{
	   ptr=pbuf_alloc(PBUF_TRANSPORT,(1024),PBUF_POOL); //�����ڴ�
	   if(ptr)
	   {
            ptr->payload=senddata;
            udp_send(upcb,ptr);	//udp��������
            pbuf_free(ptr);//�ͷ��ڴ�
            senddata+=1024;
	   }
  }
} 
//�ر�UDP����
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//�Ͽ�UDP���� 
}

























