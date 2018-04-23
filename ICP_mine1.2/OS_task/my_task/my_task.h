#ifndef __MY_TASK_H__
#define __MY_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/*define*/
#define TX_BUF_LEN_USB 1024*6 //USB���������С
#define AD_QUEUE_SIZE 1024*2 //AD���ζ��еĴ�С

/*function*/
void get_xtask_state(void);//��ȡÿ�������״̬��Ϣ

void test_netWorkTask(void);
void StartClientTask(void const * argument);

int creat_tcpcTask1(void);
int creat_tcpsTask1(void);

#include "NetDef.h"
extern Wifi_state Lua_wifi_state;//in lua wifi lib

#endif
