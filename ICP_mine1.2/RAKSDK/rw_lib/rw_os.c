/**
  ******************************************************************************
  * @file    rw_os.c
  * @author  RAK439 module Design Team
  * @version V1.0.2
  * @date    13-Jun-2015
  * @brief   RAK439 module FreeRtos OS Entity filling C File.
  *
  *          This file contains:
  *           - FreeRtos OS Entity filling include task,mutex,sem operations
  * 
  ******************************************************************************
**/
#include "rw_os.h"
#include "cmsis_os.h"
#include "bsp.h"
//#define RW_DRIVER_TASK_PRIO  (configMAX_PRIORITIES - 3) 
//#define RW_DRIVER_TASK_STACK_SIZE (800>>2)
#define RW_DRIVER_TASK_PRIO  (tskIDLE_PRIORITY + 4)
#define RW_DRIVER_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 12//(800>>2)

//��������ӿ�
void* rw_creat_task(RW_OS_TASK_PTR p_task)
{	
  osThreadId p_tcb;
	
	/*��ڲ���*/
	//DPRINTF("rw_creat_task\r\n");

	osThreadDef(task_wifi, (os_pthread)p_task, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 7);
  p_tcb = osThreadCreate (osThread(task_wifi), NULL);
  return p_tcb;
}

//ɾ������ӿ�
int rw_del_task(void* p_tcb)
{
	/*��ڲ���*/
	//DPRINTF("rw_del_task\r\n");
	
  osThreadTerminate(p_tcb);
	
  return RW_OS_OK;
  
}

//����һ��������
static osMutexDef(mutex);

//����һ��������
void* rw_creat_mutex(void)
{
  osMutexId p_mutex;
  p_mutex = osMutexCreate(osMutex(mutex));
  return (void *)p_mutex;	
}

//�ͷ�һ��������
int rw_del_mutex(void* p_mutex)
{
  osMutexDelete(p_mutex);
  return RW_OS_OK;
}

//��ȡ��
int rw_lock_mutex(void* p_mutex, uint32_t timeout)
{
  if (timeout ==0) {  //wait forever
    timeout = osWaitForever;
  }
  osMutexWait(p_mutex,timeout);
  return RW_OS_OK;
}

//����
int rw_unlock_mutex(void* p_mutex)
{
  osMutexRelease(p_mutex);
  return RW_OS_OK;
}

//����һ���ź���
static osSemaphoreDef(sem);

//����һ���ź���
void* rw_creat_sem(void)
{
  osSemaphoreId p_sem;
  p_sem = osSemaphoreCreate(osSemaphore(sem),1);
  return p_sem;
}

//ɾ��һ���ź���
int rw_del_sem(void* p_sem)
{
  osSemaphoreDelete(p_sem);
  return RW_OS_OK;
}

//����һ���ź���
int rw_post_sem(void* p_sem)
{
  osSemaphoreRelease(p_sem);
  return RW_OS_OK;
}


//�ȴ�һ���ź���
int rw_pend_sem(void* p_sem, uint32_t timeout)
{
   int32_t oserr;
    
   if (timeout ==0) {  //wait forever
      timeout = osWaitForever;
    }
    oserr =osSemaphoreWait(p_sem,timeout);  
    if (oserr == osOK) {
        return RW_OS_OK;     
    } else if (oserr == osErrorOS) {
        return RW_OS_TIME_OUT;
    }
    return RW_OS_ERROR;
}

//��1.07�Ŀ����
void rw_enter_critical()
{
		portENTER_CRITICAL();
}

void rw_exit_critical()
{
		portEXIT_CRITICAL();
}

int rw_post_drv_sem(void* p_sem)
{
    portBASE_TYPE taskWoken = pdFALSE;

    if(p_sem == NULL) {
        return RW_OS_ERROR;
    }

    if (xSemaphoreGiveFromISR(p_sem, &taskWoken) != pdPASS) {
        return RW_OS_ERROR;
    }

    if(taskWoken == pdTRUE) {
        portEND_SWITCHING_ISR(taskWoken);

    }
    return RW_OS_OK;
}




