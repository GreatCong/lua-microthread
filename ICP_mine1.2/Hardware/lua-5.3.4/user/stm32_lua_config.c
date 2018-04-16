/**
  ******************************************************************************
  * @file   
  * @author  liucongjun
  * @version 
  * @date    
  * @brief ����MicroLibû�еĺ���  
	         lua.c      ---> PC��lua��main����
					 luac.c     ---> lua�Ľ�����main����
					 lint.c     ---> lua��ӵĿ�
					 loslib.c   --> os ���õĿ⣨���ã�
					 liolib.c   --> io�ļ����õĿ�
					 lauxlib.c  ---> 1008�� l_alloc���� �޸�myfree��myrealloc�ڴ���� �����޸������ļ�heap��stack�㹻��
					 Lua print��ʵ����lbaselib.c��luaB_print���� --> �޸���luaB_print������ʹ���ӡ������ʱ�򲻻��һ��'\t'
					 lauxlib.h  ---> �����˴����ض����Ĭ��ֵ,������luaconf.h������޸� lua�����ļ���·��Ҳ��luaconf.h���޸�
					 lua.h -->�����xprintf��֧�֣�������xprintf�滻fprintf���
	--> 2017.11.23
					 luaconf.h -->�޸���lua�ļ���Ĭ��·��,����stdout��stderr�Ѿ��ض��򣬿��Բ��ø���lua��string���
					 bug:
					    1.file����֧��һ���ļ��Ķ�д(Ĭ��handles=0)
							2.����ض�����_sys_write��_sys_read,��Ҫ�����㹻���heap����Ȼ�޷�������д
							  ���������������ض���sys_read��ִ��2�Σ�����fread�����⣬fwriteû������
	--> 2018.04.09
	         ��macrolib��,_sys_open��ϵͳ�����޷��ض�������ֱ���ض�����fopen�Ⱥ���
					 Ŀǰ֧�ֶ���ļ��Ķ�д(���֧��MMCFS_MAX_FDS = 4)
					 luaconf.h --> �����˺� LUA_USE_XPRINTF ѡ���Ƿ�ʹ��xprintf
	--> 2018.04.10
	         luaconf.h --> �滻lua_readlineΪlinenoise��֧��putty shell����
	--> 2018.04.14
	         ���Ĳ����ض��庯��,֧��feof��flag����
					 luaconf.h --> �����˺� LUA_USE_MYMALLOC ѡ���Ƿ�ʹ���Զ��������mymalloc
******************************************************************************
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//#include "my_malloc.h"

#include <rt_sys.h>
#include "mmcfs.h"
#include "usart.h"

#define PRINT_USART USART6

//typedef int FILEHANDLE;

//#pragma import(__use_no_semihosting_swi)
//#pragma import(_main_redirection)
//const char __stdin_name[150];
//const char __stdout_name[150];
//const char __stderr_name[150];

//#if   defined ( __CC_ARM ) /* ARM Compiler */
////#if   defined (__ICCARM__)    /* IAR Compiler */
//#ifdef __GNUC__
///* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
//set to 'Yes') calls __io_putchar() */
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */
///**
//* @brief  Retargets the C library printf function to the USART.
//* @param  None
//* @retval None
//*/
////   PUTCHAR_PROTOTYPE
////   {
////     /* Place your implementation of fputc here */
////     /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
////     USART_SendData(PRINT_USART, (uint16_t) ch);
////           
////     /* Loop until the end of transmission */
////     while (USART_GetFlagStatus(PRINT_USART, USART_FLAG_TC) == RESET)
//// 		
////     {};

////     return ch;
////   }

//PUTCHAR_PROTOTYPE
//{
//    /* Place your implementation of fputc here */
//    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
//    /* Place your implementation of fputc here */
//    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
//  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//���ж��е��û����������ʹ�üĴ�������
//	
//	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
//	USART6->DR = (uint8_t) ch;      
//	return ch;

//}

//#endif

////�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
//#if 1
////#pragma import(__use_no_semihosting)             
////��׼����Ҫ��֧�ֺ���                 
//struct __FILE 
//{
//	int handle; 
//}; 

//FILE __stdout;       
////����_sys_exit()�Ա���ʹ�ð�����ģʽ    
//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 
// //�ض���fputc���� 
//int fputc(int ch, FILE *f)
//{ 	
//  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//���ж��е��û����������ʹ�üĴ�������
//	
//	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
//	USART6->DR = (uint8_t) ch;      
//	return ch;
//}
//
//#endif


//�����ļ��ľ��
struct __FILE 
{
	int handle; 
	int eof_flagNone;//eof no flag
}; 

int fputc(int ch, FILE *f){
	if(f==stdout || f==stderr){
   //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//���ж��е��û����������ʹ�üĴ�������	
		while((PRINT_USART->SR&0X40)==0);//ѭ������,ֱ���������   
		PRINT_USART->DR = (uint8_t) ch;      
		return ch;
	}
	else{
		int handles=0;//Ĭ��Ϊ0	
		handles = f->handle;
		if(mmc_write(handles, &ch, 1))
		 return ch;
		else
		return EOF;
  }
}

int fgetc(FILE *stream){
	int ch=0;
	if(stream==stdin){
//	  while(1);//������վ�ֱ����while
		HAL_UART_Receive(&huart6,(uint8_t*)&ch,1,5000);//���ж��е��û����������ʹ�üĴ�������	
//		if(0==(USART6->SR&0X0020)){//�ж��Ƿ���յ�������
//		ch = USART6->DR;
//	}
		return ch;
	}
	else{
		int handles=0;//Ĭ��Ϊ0	
		handles = stream->handle;
		if (mmc_read(handles, &ch, 1)){
			//����f_eofû��flag,��һ������
		  if(mmcfs_feof(handles)){stream->eof_flagNone = 0;}
			return ch;
		}
		else{
		  stream->eof_flagNone = 1;
		return EOF;
		}
 }
}

/*�ض����ļ����*/
// @ function: 
// @ description:feof
// @ input:
// @ note: 1.�����feof��c�����е�feof��Щ��ͬ,��ʹ��fseek���ļ�ָ���ƶ���ĩβʱ��feof��Ȼ����0,
//         ���ǵ���ͼ��ȡfsize+1ʱ��feof�᷵�ط���ֵ
//         2.�Ѿ��޸�Ϊ֧��flag
int feof(FILE * stream){
  int handles=0;//Ĭ��Ϊ0	
	handles = stream->handle;
	int eof_cur = mmcfs_feof(handles);
	if(!eof_cur) {stream->eof_flagNone = 1;} //flags
  return  (stream->eof_flagNone && eof_cur);
}

// @ function: 
// @ description:fclose
// @ input:
// @ note:
int fclose(FILE * stream){
  int handles=0;//Ĭ��Ϊ0
  handles = stream->handle;
	free(stream);//��fopen��ʹ����malloc
  return mmc_close(handles);
}

// @ function: 
// @ description:fopen
// @ input:
// @ note:
FILE *fopen(const char * filename,const char * mode) {
	FILE *file = (FILE *)malloc(sizeof(FILE));
	file->eof_flagNone = 0;
	int mode_int = FA_READ;
	switch(mode[0]){
	  case 'r':
			  if(mode[1]=='\0')
					mode_int = FA_READ;//r
				else 
					mode_int = FA_READ | FA_WRITE;// r+
			break;
	  case 'w':
			if(mode[1]=='\0')
					mode_int = FA_CREATE_ALWAYS | FA_WRITE;//w
				else if(mode[1]=='+')
					mode_int = FA_CREATE_ALWAYS | FA_WRITE | FA_READ;//w+
				else 
					mode_int = FA_CREATE_NEW | FA_WRITE;//wx ignore w+x
			break;
		case 'a':
			if(mode[1]=='\0')
					mode_int = FA_OPEN_APPEND | FA_WRITE;//a
				else
					mode_int = FA_OPEN_APPEND | FA_WRITE | FA_READ;//a+
			break;
		default:
			break;
	}
	file->handle = mmc_open(filename, mode_int);
  if(file->handle <0){
		free(file);
		return NULL;
	}
	else
    return file;
}

// @ function: 
// @ description:fseek
// @ input:
// @ note:
int fseek(FILE * stream, long int offset, int whence){
	int handles=0;//Ĭ��Ϊ0
  handles = stream->handle;
	if(mmc_lseek(handles, offset, whence)<0)
		return EOF;
	else
		return 0;
}

// @ function: 
// @ description:ftell
// @ input:
// @ note:
long int ftell(FILE * stream){
	int handles=0;//Ĭ��Ϊ0
  handles = stream->handle;
  return mmc_ltell(handles);
}

//FILEHANDLE _sys_open(const char *name, int openmode)//ʹ��microlib �޷���_sys_open�ض���
//{


//	return mmc_open(name, openmode);
//}

//int _sys_close(FILEHANDLE fh)
//{
//	 return mmc_close(fh);
//}

////due to use fputc, not use the function
//int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
//{
//	xprintf("_sys_write=%d ",len);
//	return mmc_write(fh, buf, len);
//}
////due to use fgetc, not use the function
//int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
//{
//	xprintf("_sys_read=%d ",len);
//	return mmc_read(fh, buf, len);
//}
////������Ƿ�Ϊ�ն�
//int _sys_istty(FILEHANDLE fh)
//{
//	return 0;
//}
//int _sys_seek(FILEHANDLE fh, long pos)
//{
//	return mmc_lseek(fh, pos, SEEK_SET);
//}
////ˢ�¾�������Ļ�����
//int _sys_ensure(FILEHANDLE fh)
//{
//	return 0;
//}

////�����ļ���ǰ����
//long _sys_flen(FILEHANDLE fh)
//{
//	return mmc_file_len(fh);
//}
//void _sys_exit(int status)
//{
//	//while(1);

//}
//int _sys_tmpnam(char *name, int fileno, unsigned maxlength)
//{
//	return 0;
//}
////��һ���ַ�д�����̨
//void _ttywrch(int ch)
//{
//	//��д֮����޷���ʼ��
////	while((USART6->SR&0X40)==0);//ѭ������,ֱ���������   
////	USART6->DR = (uint8_t) ch;      
//}
//int remove(const char *filename)
//{
//	return 0;
//}
//char *_sys_command_string(char *cmd, int len)
//{
// return NULL;
//}

//clock_t clock(void){
//  return 0;
//}

/* ����lua_os lib�еĲ��ֺ��� */

//time������loslib.c��lstate.c�е���,���ڲ��������
time_t time(time_t * time) {     
  return 0; 
}  

//exit()��system()������loslib.c�е���
void exit(int status) {      
  while(1);
}  

int system(const char * string) {     
  return 0; 
}

/* �ض����ڴ���� */
//�ض�����OS����Ӱ�죿��
//void free(void * ptr){
//  myfree(SRAMIN,ptr);
//}
//   
//void *malloc(size_t size){
//  return mymalloc(SRAMIN,size);
//}
//   
//void *realloc(void * ptr, size_t size){
//  return myrealloc(SRAMIN,ptr,size);
//}
