/**
 *************************************************************************************
 * @file my_task.c
 * @author liucongjun
 * @version 1.0
 * @date 2018-3-7
 * @brief �Զ����task
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
//		get_xtask_state();//��ȡ�����״̬
//		HAL_GPIO_TogglePin(LED_RED_GPIO_Port,LED_RED_Pin);
		//HAL_Delay(1000);
    osDelay(5000);//ÿ��5���ȡһ��ÿ�������״̬��Ϣ
  }
  /* USER CODE END StartDefaultTask */
}

/* StartAppTask function */
void StartAppTask(void const * argument)
{
  /* USER CODE BEGIN StartAppTask */
//	int ret = RW_OK;
//    /*RAK439������ʼ��*/
//    platform_init();

//    /*��ʼ��app����*/
//    rw_appdemo_context_init();

//    /*����·����*/ 
//    //�˹��ܺ���������һ��ʱ��
//    ret = rw_network_startSTA();
//    osThreadDef(clientTask, StartClientTask, osPriorityNormal, 0, 2048);
//    clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

//    /* Infinite loop */
//    for(;;)
//    {
//        if (app_demo_ctx.rw_connect_status == STATUS_FAIL || app_demo_ctx.rw_ipquery_status == STATUS_FAIL) {
//            DPRINTF("reconnect and ipquery...\r\n");
//            /*ɾ��clientTask*/
//            //osThreadTerminate(clientTaskHandle);
//            //rw_appdemo_context_init();   
//            //rw_sysDriverReset();
//            //Ҳ����ֱ�ӵ���rw_network_startSTA()ʵ��·������������
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
		//�����Task���ļ���д����Ҫ��stack = 128 --> 1024
//		fat_test();//д�ļ���Ӱ�쵽wifi,����������ż������ֶ�������
//		do_file_script1("autorun.lua");//����lua SD������
		osDelay(5000);
  }
  /* USER CODE END StartTest_Task */
}

/* StartInquireTask function */
void StartInquireTask(void const * argument)//���ϲ�ѯ��Task,���ȼ�ΪHigh
{
  /* USER CODE BEGIN StartInquireTask */
	uint8_t buf;
  /* Infinite loop */
  for(;;)
  {
		//�����ϵİ忨�ϳ����޷����յ��������������ĳ�osPriorityHigh���ܹ�����������		
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
			  Lua_wifi_state.net_on = -1;//-1��ʾ��ʼ������
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
	//���Task������
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
            DPRINTF("δ���ӷ�����\r\n");
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
									  
									  close(app_demo_ctx.tcp_cloud_sockfd);//���Task�����⣬�Ῠ��������
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

