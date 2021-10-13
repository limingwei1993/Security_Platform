#include "udp_demo.h" 
#include "stdio.h"
#include "string.h" 
extern u8 save;

u8 udp_demo_flag;
u8 udp_demo_recvbuf[UDP_DEMO_RX_BUFSIZE]={0};
extern u8 test_data[6*1025];
//UDP测试
void udp_demo_test(u16 port,u16 portl)
{
 	err_t err;
	struct udp_pcb *udppcb;  	//定义一个  UDP服务器控制块
	struct ip_addr rmtipaddr;  	//远端ip地址
 	u8 ip[4]={192,168,1,113};
	udppcb=udp_new();
	if(udppcb)//创建成功
	{ 
		IP4_ADDR(&rmtipaddr,ip[0],ip[1],ip[2],ip[3]); /*将远端IP转换为需要的格式*/
		err=udp_connect(udppcb,&rmtipaddr,port);//UDP客户端连接到指定IP地址和端口号的服务器
		if(err==ERR_OK)
		{
			err=udp_bind(udppcb,IP_ADDR_ANY,portl);//绑定本地IP地址与端口号
			if(err==ERR_OK)	//绑定完成
			{				
				udp_recv(udppcb,udp_demo_recv,NULL);//注册接收回调函数
			}
		}		
	}
} 

//UDP回调函数
void udp_demo_recv(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port)
{
	u32 data_len = 0;
	struct pbuf *q;
	if(p!=NULL)	//接收到不为空的数据时
	{
		memset(udp_demo_recvbuf,0,UDP_DEMO_RX_BUFSIZE);  //数据接收缓冲区清零
		for(q=p;q!=NULL;q=q->next)  //遍历完整个pbuf链表
		{
			//判断要拷贝到UDP_DEMO_RX_BUFSIZE中的数据是否大于UDP_DEMO_RX_BUFSIZE的剩余空间，如果大于
			//的话就只拷贝UDP_DEMO_RX_BUFSIZE中剩余长度的数据，否则的话就拷贝所有的数据
			if(q->len > (UDP_DEMO_RX_BUFSIZE-data_len)) memcpy(udp_demo_recvbuf+data_len,q->payload,(UDP_DEMO_RX_BUFSIZE-data_len));//拷贝数据
			else memcpy(udp_demo_recvbuf+data_len,q->payload,q->len);
			data_len += q->len;  	
			if(data_len > UDP_DEMO_RX_BUFSIZE) break; //超出TCP客户端接收数组,跳出	
		}
		udp_demo_senddata(upcb);
		upcb->remote_ip=*addr; 				//记录远程主机的IP地址
		upcb->remote_port=port;  			//记录远程主机的端口号
		lwipdev.remoteip[0]=upcb->remote_ip.addr&0xff; 		//IADDR4
		lwipdev.remoteip[1]=(upcb->remote_ip.addr>>8)&0xff; //IADDR3
		lwipdev.remoteip[2]=(upcb->remote_ip.addr>>16)&0xff;//IADDR2
		lwipdev.remoteip[3]=(upcb->remote_ip.addr>>24)&0xff;//IADDR1
		pbuf_free(p);//释放内存
	}else
	{
		udp_disconnect(upcb); 
	} 
} 
//UDP服务器发送数据
void udp_demo_senddata(struct udp_pcb *upcb)
{
	struct pbuf *ptr;
	
	u8 i=0;
	u8 * senddata=test_data;
	for(i=0;i<6;i++)
	{
	   ptr=pbuf_alloc(PBUF_TRANSPORT,(1024),PBUF_POOL); //申请内存
	   if(ptr)
	   {
            ptr->payload=senddata;
            udp_send(upcb,ptr);	//udp发送数据
            pbuf_free(ptr);//释放内存
            senddata+=1024;
	   }
  }
} 
//关闭UDP连接
void udp_demo_connection_close(struct udp_pcb *upcb)
{
	udp_disconnect(upcb); 
	udp_remove(upcb);			//断开UDP连接 
}

























