#include "tcp_server_demo.h" 
#include "stdio.h"
#include "string.h"  
extern u8 save;

u8 tcp_server_flag;	 
u8 tcp_server_recvbuf[50]={0};
 extern u8 test_data[6*1025];
//TCP Server ����
void tcp_server_test(u16_t port)
{
	err_t err;  
	struct tcp_pcb *tcppcbnew;  	//����һ��TCP���������ƿ�
	struct tcp_pcb *tcppcbconn;  	//����һ��TCP���������ƿ�
	
	tcppcbnew=tcp_new();	//����һ���µ�pcb
	if(tcppcbnew)			//�����ɹ�
	{ 
		err=tcp_bind(tcppcbnew,IP_ADDR_ANY,port);	//������IP��ָ���Ķ˿ںŰ���һ��,IP_ADDR_ANYΪ�󶨱������е�IP��ַ
		if(err==ERR_OK)	//�����
		{
			tcppcbconn=tcp_listen(tcppcbnew); 			//����tcppcb�������״̬
			tcp_accept(tcppcbconn,tcp_server_accept); 	//��ʼ��LWIP��tcp_accept�Ļص�����
		} 
	}
} 
//lwIP tcp_accept()�Ļص�����
err_t tcp_server_accept(void *arg,struct tcp_pcb *newpcb,err_t err)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
 	LWIP_UNUSED_ARG(arg);
	LWIP_UNUSED_ARG(err);
	tcp_setprio(newpcb,TCP_PRIO_MIN);//�����´�����pcb���ȼ�
	es=(struct tcp_server_struct*)mem_malloc(sizeof(struct tcp_server_struct)); //�����ڴ�
 	if(es!=NULL) //�ڴ����ɹ�
	{
		es->state=ES_TCPSERVER_ACCEPTED;  	//��������
		es->pcb=newpcb;
		es->p=NULL;
		
		tcp_arg(newpcb,es);
		tcp_recv(newpcb,tcp_server_recv);	//��ʼ��tcp_recv()�Ļص�����
		tcp_err(newpcb,tcp_server_error); 	//��ʼ��tcp_err()�ص�����
		tcp_poll(newpcb,tcp_server_poll,1);	//��ʼ��tcp_poll�ص�����
		tcp_sent(newpcb,tcp_server_sent);  	//��ʼ�����ͻص�����
		  
		tcp_server_flag|=1<<5;				//����пͻ���������
		lwipdev.remoteip[0]=newpcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(newpcb->remote_ip.addr>>8)&0xff;  	//IADDR3
		lwipdev.remoteip[2]=(newpcb->remote_ip.addr>>16)&0xff; 	//IADDR2
		lwipdev.remoteip[3]=(newpcb->remote_ip.addr>>24)&0xff; 	//IADDR1 
		ret_err=ERR_OK;
	}else ret_err=ERR_MEM;
	return ret_err;
}
//lwIP tcp_recv()�����Ļص�����
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	err_t ret_err;
	u32 data_len = 0;
	struct pbuf *q;
  	struct tcp_server_struct *es;
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(struct tcp_server_struct *)arg;
	if(p==NULL) //�ӿͻ��˽��յ�������
	{
		es->state=ES_TCPSERVER_CLOSING;//��Ҫ�ر�TCP ������
		es->p=p; 
		ret_err=ERR_OK;
	}else if(err!=ERR_OK)	//�ӿͻ��˽��յ�һ���ǿ�����,��������ĳ��ԭ��err!=ERR_OK
	{
		if(p)pbuf_free(p);	//�ͷŽ���pbuf
		ret_err=err;
	}else if(es->state==ES_TCPSERVER_ACCEPTED) 	//��������״̬
	{
		if(p!=NULL)  //����������״̬���ҽ��յ������ݲ�Ϊ��ʱ�����ӡ����
		{
			memset(tcp_server_recvbuf,0,TCP_SERVER_RX_BUFSIZE);  //���ݽ��ջ���������
			for(q=p;q!=NULL;q=q->next)  //����������pbuf����
			{
				//�ж�Ҫ������TCP_SERVER_RX_BUFSIZE�е������Ƿ����TCP_SERVER_RX_BUFSIZE��ʣ��ռ䣬�������
				//�Ļ���ֻ����TCP_SERVER_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
				if(q->len > (TCP_SERVER_RX_BUFSIZE-data_len)) 
					memcpy(tcp_server_recvbuf+data_len,q->payload,(TCP_SERVER_RX_BUFSIZE-data_len));//��������
				else 
					memcpy(tcp_server_recvbuf+data_len,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_SERVER_RX_BUFSIZE) break; //����TCP�ͻ��˽�������,����	
			}
			tcp_server_flag|=1<<6;	//��ǽ��յ�������
			lwipdev.remoteip[0]=tpcb->remote_ip.addr&0xff; 		//IADDR4
			lwipdev.remoteip[1]=(tpcb->remote_ip.addr>>8)&0xff; //IADDR3
			lwipdev.remoteip[2]=(tpcb->remote_ip.addr>>16)&0xff;//IADDR2
			lwipdev.remoteip[3]=(tpcb->remote_ip.addr>>24)&0xff;//IADDR1 
			tcp_server_flag|=1<<7;//���Ҫ��������
 			tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
			pbuf_free(p);  	//�ͷ��ڴ�
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
//lwIP tcp_err�����Ļص�����
void tcp_server_error(void *arg,err_t err)
{  
	LWIP_UNUSED_ARG(err);  
	if(arg!=NULL)mem_free(arg);//�ͷ��ڴ�
} 
//lwIP tcp_poll�Ļص�����
err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_server_struct *es; 
	es=(struct tcp_server_struct *)arg; 
	if(es!=NULL)
	{
		if(tcp_server_flag&(1<<7))	//�ж��Ƿ�������Ҫ����
		{
			tcp_server_flag&=~(1<<7);  			//������ݷ��ͱ�־λ
				tcp_write(tpcb,test_data,(12*512),1); //��Ҫ���͵����ݼ��뵽���ͻ��������
		}else if(es->state==ES_TCPSERVER_CLOSING)//��Ҫ�ر�����?ִ�йرղ���
		{
			tcp_server_connection_close(tpcb,es);//�ر�����
		}
		ret_err=ERR_OK;
	}else
	{
		tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
		ret_err=ERR_ABRT; 
	}
	return ret_err;
} 
//lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_server_struct *es;
	LWIP_UNUSED_ARG(len); 
	es = (struct tcp_server_struct *) arg;
	if(es->p)tcp_server_senddata(tpcb,es);//��������
	return ERR_OK;
} 
//�˺���������������
void tcp_server_senddata(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	struct pbuf *ptr;
	u16 plen;
	err_t wr_err=ERR_OK;
	 while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb)))
	 {
		ptr=es->p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
		if(wr_err==ERR_OK)
		{ 
			plen=ptr->len;
			es->p=ptr->next;			//ָ����һ��pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
			pbuf_free(ptr);
			tcp_recved(tpcb,plen); 		//����tcp���ڴ�С
		}else if(wr_err==ERR_MEM)es->p=ptr;
	 }
} 
//�ر�tcp����
void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *es)
{
	tcp_close(tpcb);
	tcp_arg(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_recv(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);
	if(es)mem_free(es); 
	tcp_server_flag&=~(1<<5);//������ӶϿ���
}
extern void tcp_pcb_purge(struct tcp_pcb *pcb);	//�� tcp.c���� 
extern struct tcp_pcb *tcp_active_pcbs;			//�� tcp.c���� 
extern struct tcp_pcb *tcp_tw_pcbs;				//�� tcp.c����  
//ǿ��ɾ��TCP Server�����Ͽ�ʱ��time wait
void tcp_server_remove_timewait(void)
{
	struct tcp_pcb *pcb,*pcb2; 
	
	while(tcp_active_pcbs!=NULL)
	{
		lwip_periodic_handle();	//������ѯ
		lwip_pkt_handle();
 		delay_ms(10);			//�ȴ�tcp_active_pcbsΪ��  
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




































