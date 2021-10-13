#include "lwip_comm.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "ethernetif.h" 
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h" 
#include "lwip/raw.h" 
#include "lwip/udp.h"
#include <stdio.h>

   
__lwip_dev lwipdev;						//lwip控制结构体 
struct netif lwip_netif;				//定义一个全局的网络接口

extern u32 memp_get_memorysize(void);	//在memp.c里面定义
extern u8_t *memp_memory;				//在memp.c里面定义.
extern u8_t *ram_heap;					//在mem.c里面定义.

u32 TCPTimer=0;			//TCP查询计时器
u32 ARPTimer=0;			//ARP查询计时器
u32 lwip_localtime;		//lwip本地时间计数器,单位:ms

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
u32 DHCPfineTimer=0;	//DHCP精细处理计时器
u32 DHCPcoarseTimer=0;	//DHCP粗糙处理计时器
#endif

//lwip中mem和memp的内存申请
//返回值:0,成功;
//    其他,失败
u8 lwip_comm_mem_malloc(void)
{
	memp_memory=memp_memory_buff;
	ram_heap=ram_heap_buff;	//为ram_heap申请内存 
	if(!memp_memory||!ram_heap)//有申请失败的
	{
		lwip_comm_mem_free();
		return 1;
	}
	return 0;	
}
//lwip中mem和memp内存释放
void lwip_comm_mem_free(void)
{ 	

}
//lwip 默认IP设置
//lwipx:lwip控制结构体指针
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	//默认远端IP为:192.168.1.100
	lwipx->remoteip[0]=192;	
	lwipx->remoteip[1]=168;
	lwipx->remoteip[2]=1;
	lwipx->remoteip[3]=104;
	//MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
	lwipx->mac[0]=dm9000cfg.mac_addr[0];
	lwipx->mac[1]=dm9000cfg.mac_addr[1];
	lwipx->mac[2]=dm9000cfg.mac_addr[2];
	lwipx->mac[3]=dm9000cfg.mac_addr[3];
	lwipx->mac[4]=dm9000cfg.mac_addr[4];
	lwipx->mac[5]=dm9000cfg.mac_addr[5]; 
	//默认本地IP为:192.168.1.30
	lwipx->ip[0]=192;	
	lwipx->ip[1]=168;
	lwipx->ip[2]=1;
	lwipx->ip[3]=31;
	//默认子网掩码:255.255.255.0
	lwipx->netmask[0]=255;	
	lwipx->netmask[1]=255;
	lwipx->netmask[2]=255;
	lwipx->netmask[3]=0;
	//默认网关:192.168.1.1
	lwipx->gateway[0]=192;	
	lwipx->gateway[1]=168;
	lwipx->gateway[2]=1;
	lwipx->gateway[3]=1;	
	lwipx->dhcpstatus=0;//没有DHCP	
} 

//LWIP初始化(LWIP启动的时候使用)
//返回值:0,成功
//      1,内存错误
//      2,DM9000初始化失败
//      3,网卡添加失败.
u8 lwip_comm_init(void)
{
	struct netif *Netif_Init_Flag;		//调用netif_add()函数时的返回值,用于判断网络初始化是否成功
	struct ip_addr ipaddr;  			//ip地址
	struct ip_addr netmask; 			//子网掩码
	struct ip_addr gw;      			//默认网关 
	if(lwip_comm_mem_malloc())return 1;	//内存申请失败
	if(DM9000_Init())return 2;		//初始化DM9000AEP
	lwip_init();						//初始化LWIP内核
	lwip_comm_default_ip_set(&lwipdev);	//设置默认IP等信息

#if LWIP_DHCP		//使用动态IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else				//使用静态IP
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
//	printf("网卡en的MAC地址为:................%d.%d.%d.%d.%d.%d\r\n",lwipdev.mac[0],lwipdev.mac[1],lwipdev.mac[2],lwipdev.mac[3],lwipdev.mac[4],lwipdev.mac[5]);
//	printf("静态IP地址........................%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
//	printf("子网掩码..........................%d.%d.%d.%d\r\n",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
//	printf("默认网关..........................%d.%d.%d.%d\r\n",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
#endif
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&ethernet_input);//向网卡列表中添加一个网口
	
#if LWIP_DHCP			//如果使用DHCP的话
	lwipdev.dhcpstatus=0;	//DHCP标记为0
	dhcp_start(&lwip_netif);	//开启DHCP服务
#endif
	
	if(Netif_Init_Flag==NULL)return 3;//网卡添加失败 
	else//网口添加成功后,设置netif为默认值,并且打开netif网口
	{
		netif_set_default(&lwip_netif); //设置netif为默认网口
		netif_set_up(&lwip_netif);		//打开netif网口
	}
	return 0;//操作OK.
}   

//当接收到数据后调用 
void lwip_pkt_handle(void)
{
	//从网络缓冲区中读取接收到的数据包并将其发送给LWIP处理 
	ethernetif_input(&lwip_netif);
}
unsigned int test_time_250ms_count[30]={0};
unsigned int test_time_500ms_count[30]={0};
unsigned int test_time_5s_count[30]={0};
u8 test_250ms_num =0;
u8 test_500ms_num =0;
u8 test_5s_num =0;
extern u32 time_250ms;
extern u32 time_500ms;
extern u32 time_5s;
static num=0;
//LWIP轮询任务
void lwip_periodic_handle()
{

#if LWIP_TCP
	//每250ms调用一次tcp_tmr()函数
	if (lwip_localtime - TCPTimer >= TCP_TMR_INTERVAL)
	{
			time_250ms=0;
		if(num==1)
		{
			time_500ms=0;
		}
		TCPTimer =  lwip_localtime;
		tcp_tmr();
		if(num==1)
		{
			num=0;
			if(test_500ms_num>=30)
		  test_500ms_num=0;	   		  
		  test_time_500ms_count[test_500ms_num]=time_500ms;	
			test_500ms_num++;
		}
		else
			num=1;
		if(test_250ms_num>=30)
		test_250ms_num=0;
		test_time_250ms_count[test_250ms_num]=time_250ms;	
    test_250ms_num++;
	}
#endif
	//ARP每5s周期性调用一次
	if ((lwip_localtime - ARPTimer) >= ARP_TMR_INTERVAL)
	{
		time_5s=0;
		
		
		ARPTimer =  lwip_localtime;
		etharp_tmr();
		
		if(test_5s_num>=30)
		test_5s_num=0;
		test_time_5s_count[test_5s_num]=time_5s;	
    test_5s_num++;
		
	}
		

#if LWIP_DHCP //如果使用DHCP的话
	//每500ms调用一次dhcp_fine_tmr()
	if (lwip_localtime - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
	{
		DHCPfineTimer =  lwip_localtime;
		dhcp_fine_tmr();
		if ((lwipdev.dhcpstatus != 2)&&(lwipdev.dhcpstatus != 0XFF))
		{ 
			lwip_dhcp_process_handle();  //DHCP处理
		}
	}

	//每60s执行一次DHCP粗糙处理
	if (lwip_localtime - DHCPcoarseTimer >= DHCP_COARSE_TIMER_MSECS)
	{
		DHCPcoarseTimer =  lwip_localtime;
		dhcp_coarse_tmr();
	}  
#endif
}


//如果使能了DHCP
#if LWIP_DHCP

//DHCP处理任务
void lwip_dhcp_process_handle(void)
{
	u32 ip=0,netmask=0,gw=0;
	switch(lwipdev.dhcpstatus)
	{
		case 0: 	//开启DHCP
			dhcp_start(&lwip_netif);
			lwipdev.dhcpstatus = 1;		//等待通过DHCP获取到的地址
			break;
		case 1:		//等待获取到IP地址
		{
			ip=lwip_netif.ip_addr.addr;		//读取新IP地址
			netmask=lwip_netif.netmask.addr;//读取子网掩码
			gw=lwip_netif.gw.addr;			//读取默认网关 
			
			if(ip!=0)			//正确获取到IP地址的时候
			{
				lwipdev.dhcpstatus=2;	//DHCP成功
				//解析出通过DHCP获取到的IP地址
				lwipdev.ip[3]=(uint8_t)(ip>>24); 
				lwipdev.ip[2]=(uint8_t)(ip>>16);
				lwipdev.ip[1]=(uint8_t)(ip>>8);
				lwipdev.ip[0]=(uint8_t)(ip);
				//解析通过DHCP获取到的子网掩码地址
				lwipdev.netmask[3]=(uint8_t)(netmask>>24);
				lwipdev.netmask[2]=(uint8_t)(netmask>>16);
				lwipdev.netmask[1]=(uint8_t)(netmask>>8);
				lwipdev.netmask[0]=(uint8_t)(netmask);
				//解析出通过DHCP获取到的默认网关
				lwipdev.gateway[3]=(uint8_t)(gw>>24);
				lwipdev.gateway[2]=(uint8_t)(gw>>16);
				lwipdev.gateway[1]=(uint8_t)(gw>>8);
				lwipdev.gateway[0]=(uint8_t)(gw);
			}else if(lwip_netif.dhcp->tries>LWIP_MAX_DHCP_TRIES) //通过DHCP服务获取IP地址失败,且超过最大尝试次数
			{
				lwipdev.dhcpstatus=0XFF;//DHCP超时失败.
				//使用静态IP地址
				IP4_ADDR(&(lwip_netif.ip_addr),lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
				IP4_ADDR(&(lwip_netif.netmask),lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);
				IP4_ADDR(&(lwip_netif.gw),lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
			}
		}
		break;
		default : break;
	}
}
#endif 



























