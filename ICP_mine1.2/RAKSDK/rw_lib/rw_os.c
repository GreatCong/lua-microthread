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
#if 0 //hal库

#include "rw_os.h"
#include "cmsis_os.h"
#include "bsp.h"
//#define RW_DRIVER_TASK_PRIO  (configMAX_PRIORITIES - 3) 
//#define RW_DRIVER_TASK_STACK_SIZE (800>>2)
#define RW_DRIVER_TASK_PRIO  (tskIDLE_PRIORITY + 4)
#define RW_DRIVER_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 12//(800>>2)

//创建任务接口
void* rw_creat_task(RW_OS_TASK_PTR p_task)
{	
  osThreadId p_tcb;
	
	/*入口测试*/
	//DPRINTF("rw_creat_task\r\n");

	osThreadDef(task_wifi, (os_pthread)p_task, osPriorityHigh, 0, configMINIMAL_STACK_SIZE * 7);
  p_tcb = osThreadCreate (osThread(task_wifi), NULL);
  return p_tcb;
}

//删除任务接口
int rw_del_task(void* p_tcb)
{
	/*入口测试*/
	//DPRINTF("rw_del_task\r\n");
	
  osThreadTerminate(p_tcb);
	
  return RW_OS_OK;
  
}

//定义一个互斥锁
static osMutexDef(mutex);

//创建一个互斥锁
void* rw_creat_mutex(void)
{
  osMutexId p_mutex;
  p_mutex = osMutexCreate(osMutex(mutex));
  return (void *)p_mutex;	
}

//释放一个互斥锁
int rw_del_mutex(void* p_mutex)
{
  osMutexDelete(p_mutex);
  return RW_OS_OK;
}

//获取锁
int rw_lock_mutex(void* p_mutex, uint32_t timeout)
{
  if (timeout ==0) {  //wait forever
    timeout = osWaitForever;
  }
  osMutexWait(p_mutex,timeout);
  return RW_OS_OK;
}

//解锁
int rw_unlock_mutex(void* p_mutex)
{
  osMutexRelease(p_mutex);
  return RW_OS_OK;
}

//定义一个信号量
static osSemaphoreDef(sem);

//创建一个信号量
void* rw_creat_sem(void)
{
  osSemaphoreId p_sem;
  p_sem = osSemaphoreCreate(osSemaphore(sem),1);
  return p_sem;
}

//删除一个信号量
int rw_del_sem(void* p_sem)
{
  osSemaphoreDelete(p_sem);
  return RW_OS_OK;
}

//发送一个信号量
int rw_post_sem(void* p_sem)
{
  osSemaphoreRelease(p_sem);
  return RW_OS_OK;
}


//等待一个信号量
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

//与1.07的库兼容
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
#endif

#include "rw_os.h"
#include "cmsis_os.h"

//#include "rw_app.h"

//#define RW_DRIVER_TASK_PRIO  (tskIDLE_PRIORITY + 4)
#define RW_DRIVER_TASK_PRIO (tskIDLE_PRIORITY + 5) //osPriorityHigh
#define RW_DRIVER_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 7//(800>>2)


void* rw_creat_task(RW_OS_TASK_PTR p_task)
{
    xTaskHandle p_tcb = NULL;
    xTaskCreate(p_task, "wifi_task", RW_DRIVER_TASK_STACK_SIZE , NULL, RW_DRIVER_TASK_PRIO, &p_tcb);

    return p_tcb;
}

int rw_del_task(void* p_tcb)
{
    vTaskDelete(p_tcb);
    return RW_OS_OK;
}

void* rw_creat_mutex(void)
{
    SemaphoreHandle_t p_mutex;
    p_mutex = xSemaphoreCreateMutex();
    return (void *)p_mutex;
}

int rw_del_mutex(void* p_mutex)
{
    vSemaphoreDelete(p_mutex);
    return RW_OS_OK;
}

int rw_lock_mutex(void* p_mutex, uint32_t timeout)
{
    if(p_mutex == NULL) {
        return RW_OS_ERROR;
    }
    if (timeout ==0) {  //wait forever
        timeout = portMAX_DELAY;
    }

    if(xSemaphoreTake(p_mutex,timeout) != pdPASS) {
        return RW_OS_TIME_OUT;
    }
    return RW_OS_OK;
}

int rw_unlock_mutex(void* p_mutex)
{
    if(p_mutex == NULL) {
        return RW_OS_ERROR;
    }
    if(xSemaphoreGive(p_mutex) != pdPASS) {
        return RW_OS_ERROR;
    }
    return RW_OS_OK;
}

//与hal库不同的是采用了xSemaphoreCreateCounting
void* rw_creat_sem(void)
{
    xSemaphoreHandle p_sem;
    p_sem = xSemaphoreCreateCounting(0xffffffff,0);
    return p_sem;
}

int rw_del_sem(void* p_sem)
{
    vSemaphoreDelete(p_sem);
    return RW_OS_OK;
}

int rw_post_sem(void* p_sem)
{
    if(p_sem == NULL) {
        return RW_OS_ERROR;
    }

    if(xSemaphoreGive(p_sem) != pdPASS) {
        return RW_OS_ERROR;
    }
    return RW_OS_OK;
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


int rw_pend_sem(void* p_sem, uint32_t timeout)
{
    if(p_sem == NULL) {
        return RW_OS_ERROR;
    }
    if (timeout ==0) {  //wait forever
        timeout = portMAX_DELAY;
    }

    if(xSemaphoreTake(p_sem,timeout) != pdPASS) {
        return RW_OS_TIME_OUT;
    }
    return RW_OS_OK;
}


void rw_enter_critical()
{
		portENTER_CRITICAL();
}

void rw_exit_critical()
{
		portEXIT_CRITICAL();
}




