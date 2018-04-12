#include "my_fatfs.h"

#include "fatfs.h"


//��Ҫ��sd_diskio.c�м������������ص�������DMA+FATFS������
//ʹ��fatfs+FreeRTOSʱ,������Ҫ��stm32f405xx.h��__NVIC_PRIO_BITS��4U�ĳ�4������������
//void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
//{
//  osMessagePut(SDQueueID, WRITE_CPLT_MSG, osWaitForever);
//}


//void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
//{

//  osMessagePut(SDQueueID, READ_CPLT_MSG, osWaitForever);
//}

// @ function: fat_test
// @ description:����fatfs
// @ input:
// @ note:��os���ù�����,��ջҪ��һЩ��512����,��Ҫ1024
void fat_test(void){
  FATFS fs;                 // Work area (file system object) for logical drive
	FIL fil;                  // file objects
	 
	uint32_t byteswritten;                /* File write counts */
	uint32_t bytesread;                   /* File read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; /* File write buffer */
	uint8_t rtext[100];                     /* File read buffers */
	char filename[] = "STM32cube.txt";
//	char filename[] = "S.txt";
	
    printf("\r\n ****** FatFs Example ******\r\n\r\n");
 
    /*##-1- Register the file system object to the FatFs module ##############*/
    retSD = f_mount(&fs, "", 0);
    if(retSD)
    {
        printf(" mount error : %d \r\n",retSD);
        Error_Handler();
    }
    else
        printf(" mount sucess!!! \r\n");
     
    /*##-2- Create and Open new text file objects with write access ######*/
    retSD = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if(retSD)
        //xprintf(" open file error : %d\r\n",retSD);
		fat_error_test(retSD);
    else
        printf(" open file sucess!!! \r\n");
     
    /*##-3- Write data to the text files ###############################*/
    retSD = f_write(&fil, wtext, sizeof(wtext), (void *)&byteswritten);
    if(retSD)
        //xprintf(" write file error : %d\r\n",retSD);
		fat_error_test(retSD);
    else
    {
        printf(" write file sucess!!! \r\n");
        printf(" write Data : %s\r\n",wtext);
    }
     
    /*##-4- Close the open text files ################################*/
    retSD = f_close(&fil);
    if(retSD)
        //xprintf(" close error : %d\r\n",retSD);
		fat_error_test(retSD);
    else
        printf(" close sucess!!! \r\n");
     
    /*##-5- Open the text files object with read access ##############*/
    retSD = f_open(&fil, filename, FA_READ);
    if(retSD)
        //xprintf(" open file error : %d\r\n",retSD);
		fat_error_test(retSD);
    else
        printf(" open file sucess!!! \r\n");
     
    /*##-6- Read data from the text files ##########################*/
    retSD = f_read(&fil, rtext, sizeof(rtext), (UINT*)&bytesread);
    if(retSD)
        //xprintf(" read error!!! %d\r\n",retSD);
		fat_error_test(retSD);
    else
    {
        printf(" read sucess!!! \r\n");
        printf(" read Data : %s\r\n",rtext);
    }
     
    /*##-7- Close the open text files ############################*/
    retSD = f_close(&fil);
    if(retSD)  
        //xprintf(" close error!!! %d\r\n",retSD);
		fat_error_test(retSD);
    else
        printf(" close sucess!!! \r\n");
     
    /*##-8- Compare read data with the expected data ############*/
    if(bytesread == byteswritten)
    { 
        printf(" FatFs is working well!!!\r\n");
    }
}

// @ function: fat_error_test
// @ description:
// @ input:
// @ note:
void fat_error_test(uint8_t ret){
  switch(ret){
		case FR_OK :
			printf("��������\r\n");
			break;
		case FR_INVALID_DRIVE:
			printf("��������Ч\r\n");
			break;
		case FR_NOT_READY:
			printf("�����������޷�����\r\n");
			break;
		case FR_DISK_ERR:
			printf("�ײ����IO����\r\n");
			break;
		case FR_NO_PATH:
			printf("�Ҳ���·��\r\n");
			break;
		case FR_INVALID_NAME:
			printf("�ļ�����Ч\r\n");
			break;
		case FR_NO_FILE:
			printf("�Ҳ������ļ�\r\n");
			break;
		case FR_EXIST:
			printf("�ļ��Ѵ���\r\n");
			break;
		default:
			printf("��������%d\r\n",ret);
			break;
	}
		
}
