/**
 *************************************************************************************
 * @file my_task.c
 * @author liucongjun
 * @version 1.0
 * @date 2018-3-7
 * @brief 自定义的task
 *************************************************************************************
*/
#include "my_task.h"

#include "rw_app.h"
#include "tim.h"
#include "usbd_cdc_if.h"
#include "fatfs.h"
#include "my_fatfs.h"
#include "AD7606.h"

#include "lua_test.h"

#define SIZE 53

//extern int Lua_wifi_on;//in lua wifi lib
extern osThreadId appTaskHandle;

/************************ Tasks **************************/

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  //MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
//		get_xtask_state();//获取任务的状态
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin);
		//HAL_Delay(1000);
    osDelay(5000);//每隔5秒获取一次每个任务的状态信息
  }
  /* USER CODE END StartDefaultTask */
}

/* StartAppTask function */
void StartAppTask(void const * argument)
{
  /* USER CODE BEGIN StartAppTask */
//	int ret = RW_OK;
//    /*RAK439驱动初始化*/
//    platform_init();

//    /*初始化app变量*/
//    rw_appdemo_context_init();

//    /*连接路由器*/ 
//    //此功能函数会阻塞一段时间
//    ret = rw_network_startSTA();
//    osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 2048);
//    clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

//    /* Infinite loop */
//    for(;;)
//    {
//        if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
//            DPRINTF("reconnect and ipquery...\r\n");
//            /*删除clientTask*/
//            //osThreadTerminate(clientTaskHandle);
//            //rw_appdemo_context_init();   
//            //rw_sysDriverReset();
//            //也可以直接调用rw_network_startSTA()实现路由器重连功能
//            //rw_network_startSTA();
//            //osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 256);
//            //clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

//        }
//        rw_sysSleep(100);

//    }
   test_netWorkTask();
//	 for(;;)
//  {
//    osDelay(5000);
//  }
  /* USER CODE END StartAppTask */
}

/* StartTest_Task function */
void StartTest_Task(void const * argument)
{
  /* USER CODE BEGIN StartTest_Task */
  /* Infinite loop */
  for(;;)
  {
//		uint8_t xx[] = "abc";
//	  UsbSendData(xx,3);
		//如果此Task有文件读写，需要将stack = 128 --> 1024
//		fat_test();//写文件会影响到wifi,添加这个函数偶尔会出现丢包现象
//		do_file_script1("autorun.lua");//测试lua SD卡读入
		osDelay(5000);
  }
  /* USER CODE END StartTest_Task */
}

/* StartInquireTask function */
void StartInquireTask(void const * argument)//不断查询的Task,优先级为High
{
  /* USER CODE BEGIN StartInquireTask */
	uint8_t buf;
  /* Infinite loop */
  for(;;)
  {
		//在整合的板卡上出现无法接收的情况，将此任务改成osPriorityHigh，能够解决这个问题		
    UsbReceiveData(&buf);
		if(buf=='Y')
		{			
//			AD_rst_handle();
			HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
			buf=0;

		}
		else if(buf=='N') 
		{
			HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_4);
			buf=0;
      AD_rst_handle();		
		}
		
		if(Lua_wifi_state.net_on > 0){
		    osThreadDef(appTask, StartAppTask, osPriorityAboveNormal, 0, 256);
        appTaskHandle = osThreadCreate(osThread(appTask), NULL);
			  Lua_wifi_state.net_on = -1;//-1表示初始化过了
		}
		osDelay(10);
  }
  /* USER CODE END StartInquireTask */
}

/* StartClientTask function */
void StartClientTask(void const * argument)
{
  /* USER CODE BEGIN StartClientTask */
//  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
	//这个Task有问题
	int      ret = 0;
    char str[SIZE];
		memset(str, 0, sizeof(str));
		int tmpIndex = 0;
		for (int i = 0; i < SIZE; ++i)
		{
			str[i] = ('0' + tmpIndex);
			++tmpIndex;
			if (tmpIndex == 10)
			{
				tmpIndex = 0;
			}
		}
		str[SIZE - 3] = '\r';
		str[SIZE - 2] = '\n';
		str[SIZE - 1] = '\0';
	  DPRINTF("str:%s\r\n",str);
		char revBuff[SIZE];
		memset(revBuff,0,SIZE);
    /* Infinite loop */
    for(;;)
    {      
reconnect: 
        //
        if(app_demo_ctx.rw_connect_status != STATUS_OK && app_demo_ctx.rw_ipquery_status != STATUS_OK) 
        {
            DPRINTF("未连接服务器\r\n");
            rw_sysSleep(1000);
            goto reconnect;      
        }


        if(app_demo_ctx.rw_connect_status == STATUS_OK && app_demo_ctx.rw_ipquery_status == STATUS_OK) 
        {
            if (app_demo_ctx.tcp_cloud_sockfd == INVAILD_SOCK_FD)
            {
                //if((ret =RAK_TcpClient(8000, 0xC0A80469)) >= 0)//192.168.04.105
									if((ret =RAK_TcpClient(6001, 0xC0A81701)) >= 0)//192.168.23.1
                {
                    app_demo_ctx.tcp_cloud_sockfd = ret;
                    app_demo_ctx.tcp_cloud_status = STATUS_OK;
                    DPRINTF("RAK_TcpClient sockfd = %u creat\r\n",app_demo_ctx.tcp_cloud_sockfd);
										DPRINTF("SEND START***************************************\r\n\r\n");
                    ret = send(app_demo_ctx.tcp_cloud_sockfd,str,strlen(str),0);
										DPRINTF("SEND End***************************************\r\n\r\n");
										DPRINTF("send ret1:%d\r\n",ret);
									
										ret = send(app_demo_ctx.tcp_cloud_sockfd,str,strlen(str),0);
										DPRINTF("send ret2:%d\r\n",ret);
										
									  recv(app_demo_ctx.tcp_cloud_sockfd,revBuff,sizeof(revBuff),0);
										DPRINTF("revFromServer:%s\r\n",revBuff);
									  
									  close(app_demo_ctx.tcp_cloud_sockfd);//这个Task有问题，会卡死在这里
                    app_demo_ctx.tcp_cloud_status = STATUS_INIT;
                    app_demo_ctx.tcp_cloud_sockfd = INVAILD_SOCK_FD; //close tcp ,for next reconnect.
										break;

                }
                else
                {
                    if(ret == RW_ERR || ret==RW_ERR_TIME_OUT) 
                    { 
                        DPRINTF("RAK_TcpClient creat failed code=%d\r\n", ret);
                        rw_sysSleep(100);
                        goto reconnect;
                    }
                }    
            }
        }
        rw_sysSleep(1000);
    }
  /* USER CODE END StartClientTask */
}

