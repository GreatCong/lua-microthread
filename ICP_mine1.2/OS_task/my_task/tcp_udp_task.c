/**
 *************************************************************************************
 * @file tcp_udp_task.c
 * @author liucongjun
 * @version 1.0
 * @date 2018-4-19
 * @brief tcp_udp task handle function 
 *************************************************************************************
*/
#include "my_task.h"

#include "rw_app.h"
#include "AD7606.h"

#define SIZE 53
//#define MY_TCP_SERVER_PORT 6001
//#define MY_TCP_SERVER_IP 0xC0A81701 //192.168.23.1 笔记本模拟的
//#define MY_TCPC_SERVER_PORT 60090
//#define MY_TCPC_SERVER_IP 0xC0A88901 //192.168.137.1 台式机模拟的
//#define MY_TCPS_SERVER_PORT 25000
uint16_t Wifi_TCPC_SERVER_PORT = 60090;
int Wifi_TCPC_SERVER_IP = 0xC0A88901;//192.168.137.1 台式机模拟的
uint16_t Wifi_TCPS_SERVER_PORT = 25000;


//typedef uint8_t Elemtype;
typedef int16_t Elemtype;	

const Elemtype myAD_test_data[20] = {50,65,79,90,97,100,97,90,79,65,50,34,20,9,2,0,2,10,21,35};//正弦
	                      //hex={32,41,4f,5A,61,64 ,61,5A,4F,41,32,22,14,9,2,0,2,a ,15,23}
const Elemtype myAD_test_data1[60]={0x258,0x28c,0x2bf,0x2f2,0x323,0x351,0x37d,0x3a6,0x3cb,0x3ec,
	                            0x409,0x420,0x433,0x441,0x449,0x44b,0x449,0x441,0x433,0x420,
	                            0x409,0x3ec,0x3cb,0x3a6,0x37d,0x352,0x323,0x2f2,0x2bf,0x28c,
	                            0x258,0x223,0x1f0,0x1bd,0x18c,0x15e,0x132,0x109,0xe4, 0xc3,
	                            0xa6, 0x8f, 0x7c, 0x6e, 0x66, 0x64, 0x66, 0x6e, 0x7c, 0x8f,
	                            0xa6, 0xc3, 0xe4, 0x109,0x132,0x15d,0x18c,0x1bd,0x1f0,0x223,};
#include "my_queue.h"
//#define AD_QUEUE_SIZE 1200
extern QueueArray_type myAD_queue;
Elemtype my_send[AD_QUEUE_SIZE+100] = {0};

/************************************
// Method:    platform_init
// Date:  	  2016/06/27
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: void
// Function:  配置RAK439参数和初始化驱动 获取RAK439库版本与物理地址 
************************************/
int platform_init(void)
{
    rw_DriverParams_t     params;
    int                   ret =0;
    char                  libVersion[20]="";
    char                  module_mac[6] ="";

    /*RAK439用户参数配置*/
    wifi_init_params(&params);

    /*RAK439驱动初始化*/
    ret =rw_sysDriverInit(&params);
    DPRINTF("rw_sysDriverInit ret:%d\r\n",ret);
    if(ret != RW_OK)
    {
        DPRINTF("RAK module platform init...failed\r\n");
        while(1);
    }

    /*获取RAK439库版本*/
    rw_getLibVersion(libVersion); 
    DPRINTF("rak wifi LibVersion:%s\r\n", libVersion);

    /*获取RAK439物理地址*/
    rw_getMacAddr(module_mac);
    DPRINTF("rak wifi module-MAC:%02X:%02X:%02X:%02X:%02X:%02X\r\n", module_mac[0],module_mac[1],module_mac[2],module_mac[3],module_mac[4],module_mac[5]);

    return RW_OK;
}

// @ function: 
// @ description:测试wifi传输性能的函数
// @ input:
// @ note:
void test_netWorkTask(void)//测试Wifi传输
{
	     platform_init(); 
    rw_appdemo_context_init();

    rw_network_startSTA();
//	rw_network_startAP();
//		#define TCPC_TEST//测试TCP clint
//	#define TCPS_TEST//测试TCP Server
		 
	switch(Lua_wifi_state.net_protocol){
	  case NET_TCP_SERVER:
			  #ifdef  TCPS_TEST
        //creat_tcpsTask();
	      creat_tcpsTask1();
		    #endif  
			break;
		case NET_TCP_CLIENT:
					#ifdef  TCPC_TEST
//				  creat_tcpcTask();
          creat_tcpcTask1();
		      #endif
			break;
		case NET_UDP_SERVER:
					#ifdef  UDPS_TEST
				  creat_udpsTask();
		      #endif
			break;
		case NET_UDP_CLIENT:
					#ifdef  UDPC_TEST
				  creat_udpcTask();
		      #endif 
			break;
		default: //默认是client
          #ifdef  TCPC_TEST
//				  creat_tcpcTask();
          creat_tcpcTask1();
		      #endif
			break;
	}
//		#ifdef  TCPS_TEST
////				creat_tcpsTask();
//	creat_tcpsTask1();
//		#endif  
//		#ifdef  TCPC_TEST
//				creat_tcpcTask();
////	creat_tcpcTask1();
//		#endif
//		#ifdef  UDPS_TEST
//				 creat_udpsTask();
//		#endif
//		#ifdef  UDPC_TEST
//				 creat_udpcTask();
//		#endif 
    
    while(1) {
      
        if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
          DPRINTF("reconnect and ipquery...\r\n");
          rw_appdemo_context_init();   
          rw_sysDriverReset();
          rw_network_init(&conn, DHCP_CLIENT, NULL);
        }
        rw_sysSleep(100);
    }
		
}

#ifdef  TCPC_TEST 
#define RW_TCPC_TASK_PRIO   (configMAX_PRIORITIES - 5) 
static  TaskHandle_t        g_tcpclient_task1;  
#endif

#ifdef  TCPC_TEST 
void RAK_TcpClient_EventHandle1(void *p_arg)
{
	int      ret = 0;
	static int      send_recvCnt = 0;
	int      temp_len;

//	char* c_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);//RAK439 TCP MAX_RECV_PACKET_LEN=1024
//	Elemtype* c_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);//RAK439 TCP MAX_RECV_PACKET_LEN=1024

//	if(NULL == c_buffer){
//		DPRINTF("RAK_TcpClient: Allocate buffer failed.\n");
//		while(1);
//	}  

	while(1)
	{      
reconnect: 
		//连不上WIFI或者分配不到IP地址
		if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
		{
			rw_sysSleep(100);
			goto reconnect;      
		}

		if (app_demo_ctx.tcp_cloud_sockfd == INVAILD_SOCK_FD)
		{
      if((ret =RAK_TcpClient(Wifi_TCPC_SERVER_PORT, Wifi_TCPC_SERVER_IP)) >= 0)//192.168.23.1 笔记本模拟的
			//if((ret =RAK_TcpClient(25001, 0xC0A80389)) >= 0)
			{
				app_demo_ctx.tcp_cloud_sockfd = ret;
				app_demo_ctx.tcp_cloud_status = STATUS_OK;
				DPRINTF("RAK_TcpClient sockfd = %u creat \n\r",app_demo_ctx.tcp_cloud_sockfd);
			}
			else
			{
				if(ret == RW_ERR || ret==RW_ERR_TIME_OUT) 
				{ 
					DPRINTF("RAK_TcpClient creat failed code=%d\n", ret);
					rw_sysSleep(100);
					goto reconnect;
				}
			}    
		}

		if (app_demo_ctx.tcp_cloud_status <= STATUS_INIT)
		{
			if(app_demo_ctx.tcp_cloud_status < STATUS_INIT)
			{
				close(app_demo_ctx.tcp_cloud_sockfd);
				app_demo_ctx.tcp_cloud_status = STATUS_INIT;
				app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
			}
			goto reconnect;
		}

//		ret = recv(app_demo_ctx.tcp_cloud_sockfd, c_buffer, MAX_RECV_PACKET_LEN, 0);//recv返回接受到的数据
//		if (ret <= 0 )
//		{
//			if(ret ==RW_ERR_SOCKET_INVAILD){
//				DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.tcp_cloud_sockfd);
//				app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
//			}

//		}else
//		{
//			//      DPRINTF("recv packets on sockfd=%d  data_len=%d\n\r", app_demo_ctx.tcp_cloud_sockfd, ret );              
//			send_recvCnt += ret;
//		}
//   send_recvCnt = 60*2;
//	 c_buffer = myAD_test_data1;
				 send_recvCnt = GetQueue_length(myAD_queue);
//		if(send_recvCnt > 0){ReadQueue_n(&myAD_queue,my_send);}
	 
		if(send_recvCnt > 0)
		{
			temp_len = send_recvCnt >MAX_SEND_PACKET_LEN/2 ? MAX_SEND_PACKET_LEN/2:send_recvCnt;
			ReadQueue_n(&myAD_queue,my_send,temp_len);
			ret = send(app_demo_ctx.tcp_cloud_sockfd, my_send, temp_len*2, 0);//c_buffer是OS分配的内存，大小为1024
//			c_buffer = my_send;
//			ret = send(app_demo_ctx.tcp_cloud_sockfd, c_buffer, temp_len, 0);//c_buffer是OS分配的内存，大小为1024
			//temp_len = send_recvCnt >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:send_recvCnt;
			if (ret <= 0 )
			{
				//        DPRINTF("send errorcode =%d\n\r",ret);
				if(ret ==RW_ERR_SOCKET_INVAILD)
				{
					app_demo_ctx.tcp_cloud_sockfd =INVAILD_SOCK_FD;
//					send_recvCnt = 0;
				}                  
			}else
			{
//				send_recvCnt -= ret;
				osDelay(5);
			}
		}
	}
}

int creat_tcpcTask1(void)
{
	int  xRet = 0;

	xRet =xTaskCreate(RAK_TcpClient_EventHandle1, 
		"TcpClient", 
		configMINIMAL_STACK_SIZE * 4, 
		NULL, 
		RW_TCPC_TASK_PRIO, 
		&g_tcpclient_task1); 

	if(xRet !=pdPASS){
		DPRINTF("xTaskCreate errorcode =%d\n\r",xRet);
		while(1);
	}
	
	return 0;

}
#endif

#ifdef  TCPS_TEST
#define RW_TCPS_TASK_PRIO   (configMAX_PRIORITIES - 5) 
static  TaskHandle_t        g_tcpserver_task1;          
#endif

#ifdef  TCPS_TEST
void RAK_TcpServer_EventHandle1(void *p_arg)
{
    int ret = 0,i=0;
    int send_templen =0;
		static int      send_recvCnt = 0;
    
    char* s_buffer = pvPortMalloc(MAX_RECV_PACKET_LEN);

    if(NULL == s_buffer){
	DPRINTF("RAK_TcpServer: Allocate buffer failed.\n");
	while(1);
    }
   
    while(1)
    {
server_loop:
  
      if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
      {
        rw_sysSleep(100);
        goto server_loop;      
      }
    
      if (app_demo_ctx.ltcps_sockfd == INVAILD_SOCK_FD)
      {
          if((ret =RAK_TcpServer(Wifi_TCPS_SERVER_PORT)) >= 0)
          {
             app_demo_ctx.ltcps_sockfd = ret;
             DPRINTF("RAK_TcpServer sockfd = %u creat \n",app_demo_ctx.ltcps_sockfd);
          }else{
             DPRINTF("RAK_TcpServer creat failed code=%d\n", ret);
             rw_sysSleep(100);
             goto server_loop;
          }
      }
      
      RAK_TcpSRecvClients(app_demo_ctx.ltcps_sockfd , &app_demo_ctx);
      
      // send and recv loopback testing
      if (app_demo_ctx.tcpc_num > 0)
      {
        for (i=0; i< ALLOW_MAX_NUMS; i++)//这里设置的是ALLOW_MAX_NUMS=1,最大支持8
        {
          if (app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd ==INVAILD_SOCK_FD)
             continue;
          
//            ret = recv(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, s_buffer, MAX_RECV_PACKET_LEN, 0);
//            if (ret <= 0 )
//            {
//              if (ret ==RW_ERR_SOCKET_INVAILD){
//                DPRINTF("recv fd = %u  disconnect \n\r", app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd);
//                app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd =INVAILD_SOCK_FD;
//                app_demo_ctx.tcpc_num --;
//                app_demo_ctx.ltcps_clientinfos[i].loopback_count = 0;
//                break;
//              }
//            }else{
//  //            DPRINTF("recv packets on sockfd=%d  data_len=%d\n\r", app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, ret );                 
//              app_demo_ctx.ltcps_clientinfos[i].loopback_count += ret;
//            }
//            
//             if(app_demo_ctx.ltcps_clientinfos[i].loopback_count > 0)
//              {
//                  send_templen = app_demo_ctx.ltcps_clientinfos[i].loopback_count >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:app_demo_ctx.ltcps_clientinfos[i].loopback_count;
//                  ret = send(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, s_buffer, send_templen, 0);
//                  if (ret <= 0 )
//                  {
//  //                  DPRINTF("send errorcode =%d\n\r",ret);
//                    if(ret ==RW_ERR_SOCKET_INVAILD)
//                    {
//                      app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd =INVAILD_SOCK_FD;
//                      app_demo_ctx.tcpc_num --;
//                      app_demo_ctx.ltcps_clientinfos[i].loopback_count = 0;
//                      break;
//                    }                  
//                  }else
//                  {
//                    app_demo_ctx.ltcps_clientinfos[i].loopback_count  -=ret;
//                    break;
//                  }
//              }
				     send_recvCnt = GetQueue_length(myAD_queue);
//	           if(app_demo_ctx.ltcps_clientinfos[i].loopback_count > 0)
               if(send_recvCnt > 0)
              {
//                  send_templen = app_demo_ctx.ltcps_clientinfos[i].loopback_count >MAX_SEND_PACKET_LEN ? MAX_SEND_PACKET_LEN:app_demo_ctx.ltcps_clientinfos[i].loopback_count;
//                  ret = send(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, s_buffer, send_templen, 0);
								 send_templen = send_recvCnt >MAX_SEND_PACKET_LEN/2 ? MAX_SEND_PACKET_LEN/2:send_recvCnt;
						     ReadQueue_n(&myAD_queue,my_send,send_templen);
						     ret = send(app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd, my_send, send_templen*2, 0);//c_buffer是OS分配的内存，大小为1024
                  if (ret <= 0 )
                  {
//                    DPRINTF("send errorcode =%d\n\r",ret);
                    if(ret ==RW_ERR_SOCKET_INVAILD)
                    {
                      app_demo_ctx.ltcps_clientinfos[i].tcpc_sockfd =INVAILD_SOCK_FD;
                      app_demo_ctx.tcpc_num --;
                      app_demo_ctx.ltcps_clientinfos[i].loopback_count = 0;
                      break;
                    }                  
                  }else
                  {
//                    app_demo_ctx.ltcps_clientinfos[i].loopback_count  -=ret;
                    break;
                  }
              }

          }//for ALLOW_MAX_NUMS
				osDelay(30);//发送错误，-19,full buffer,适当的延时
      } //if (app_demo_ctx.tcpc_num > 0)
    } //while       
}

int creat_tcpsTask1(void)
{    
    int  xRet = 0;
    
    xRet =xTaskCreate(RAK_TcpServer_EventHandle1, 
                      "TcpServer", 
                      configMINIMAL_STACK_SIZE * 2, 
                      NULL, 
                      RW_TCPS_TASK_PRIO, 
                      &g_tcpserver_task1); 
    
    if(xRet !=pdPASS){
       DPRINTF("xTaskCreate errorcode =%d\n\r",xRet);
       while(1);
    }
    
}
#endif
