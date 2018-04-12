/**
 *************************************************************************************
 * @file task_utilities.c
 * @author liucongjun
 * @version 0.1
 * @date 2018-3-7
 * @brief FreeRTOS�ü��ĺ���
 *************************************************************************************
*/
#include "my_task.h"

#include "rw_app.h"
#include "tim.h"

/************************************
// Method:    get_xtask_state
// Date:  	  2016/06/27
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: void
// Function:  ��ȡÿ�������״̬��Ϣ
************************************/
void get_xtask_state(void)
{
    char tmp[128] = "";
    int res_getstate = 0;
    const char task_state[]={'r','R','B','S','D'};  
    volatile UBaseType_t uxArraySize, index;  
    uint32_t ulTotalRunTime,ulStatsAsPercentage;  
    TaskStatus_t *pxTaskStatusArray = NULL;

    /*��ȡ������Ŀ��������һ�����񣨸�����Ϊfreertos��IDLE��������*/
    uxArraySize = uxTaskGetNumberOfTasks(); 
    assert_param(uxArraySize>0);

    /*Ϊ������Ϣ���������ڴ�*/
    pxTaskStatusArray = pvPortMalloc(sizeof(TaskStatus_t)*uxArraySize);
    assert_param(pxTaskStatusArray);

    /*��ȡ������Ϣ*/
    res_getstate = uxTaskGetSystemState(pxTaskStatusArray,uxArraySize,&ulTotalRunTime);
    assert_param(res_getstate);

    //DPRINTF("********************������Ϣͳ�Ʊ�*********************\r\n\r\n");
    //DPRINTF("������    ״̬    ID  ���ȼ�   ��ջ  CPUʹ����\r\n");
    if (ulTotalRunTime>0)
    {
        for (index=0; index<uxArraySize; index++)
        {
            ulStatsAsPercentage = (uint64_t)(pxTaskStatusArray[index].ulRunTimeCounter)*100 / ulTotalRunTime;
            if (ulStatsAsPercentage > 0UL)
            {
                sprintf(tmp,"%-12s%-6c%-6ld%-8ld%-8d%d%%",
                    pxTaskStatusArray[index].pcTaskName,
                    task_state[pxTaskStatusArray[index].eCurrentState],  
                    pxTaskStatusArray[index].xTaskNumber,
                    pxTaskStatusArray[index].uxCurrentPriority,  
                    pxTaskStatusArray[index].usStackHighWaterMark,
                    ulStatsAsPercentage);
            }
            else
            {
                sprintf(tmp,"%-12s%-6c%-6ld%-8ld%-8dt<1%%",
                    pxTaskStatusArray[index].pcTaskName,
                    task_state[pxTaskStatusArray[index].eCurrentState],  
                    pxTaskStatusArray[index].xTaskNumber,
                    pxTaskStatusArray[index].uxCurrentPriority,  
                    pxTaskStatusArray[index].usStackHighWaterMark); 
            }
            //DPRINTF("%s\r\n",tmp);
        }
    }
    //DPRINTF("����״̬:   r-����  R-����  B-����  S-����  D-ɾ��\r\n"); 
    DPRINTF("�ڴ�ʣ����:%d Byte\r\n\r\n",xPortGetFreeHeapSize());

    /*�ͷ��ڴ�*/
    vPortFree(pxTaskStatusArray);
}

/*FreeeRTOS �ü�*/
void configureTimerForRunTimeStats(void)
{
    HAL_TIM_Base_Start_IT(&htim3);
}

unsigned long getRunTimeCounterValue(void)
{
    /*���ض�ʱ��3�жϴ������ı���*/
    return Tim3Counter;
}

void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
    called if a stack overflow is detected. */
    DPRINTF("%s StackOverFlow!\r\n",pcTaskName);
}

void vApplicationMallocFailedHook(void)
{
    /* vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created. It is also called by various parts of the
    demo application. If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented). */
    DPRINTF("vApplicationMallocFailedHook!\r\n");
}

/*FreeeRTOS �ü� end */
