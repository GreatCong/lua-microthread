/**
  ******************************************************************************
  * @file   
  * @author  liucongjun
  * @version 
  * @date    
  * @brief 定义MicroLib没有的函数  
	         lua.c      ---> PC中lua的main函数
					 luac.c     ---> lua的解释器main函数
					 lint.c     ---> lua添加的库
					 loslib.c   --> os 调用的库（不用）
					 liolib.c   --> io文件调用的库
					 lauxlib.c  ---> 1008行 l_alloc函数 修改myfree和myrealloc内存管理 否则修改启动文件heap和stack足够大
					 Lua print的实现在lbaselib.c的luaB_print函数 --> 修改了luaB_print函数，使其打印参数的时候不会多一个'\t'
					 lauxlib.h  ---> 包含了串口重定向的默认值,可以在luaconf.h中添加修改 lua查找文件的路径也在luaconf.h中修改
					 lua.h -->添加了xprintf的支持，可以用xprintf替换fprintf输出
	--> 2017.11.23
					 luaconf.h -->修改了lua文件的默认路径,由于stdout和stderr已经重定向，可以不用更改lua的string输出
					 bug:
					    1.file仅仅支持一个文件的读写(默认handles=0)
							2.如果重定向了_sys_write和_sys_read,需要更改足够大的heap，不然无法大量读写
							  若更改了上述的重定向，sys_read会执行2次？？即fread有问题，fwrite没有问题
	--> 2018.04.09
	         在macrolib下,_sys_open等系统函数无法重定向，所以直接重定向了fopen等函数
					 目前支持多个文件的读写(最大支持MMCFS_MAX_FDS = 4)
					 luaconf.h --> 增加了宏 LUA_USE_XPRINTF 选择是否使用xprintf
	--> 2018.04.10
	         luaconf.h --> 替换lua_readline为linenoise，支持putty shell交互
	--> 2018.04.14
	         更改部分重定义函数,支持feof的flag功能
					 luaconf.h --> 增加了宏 LUA_USE_MYMALLOC 选择是否使用自定义管理函数mymalloc
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
//  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//在中断中调用会造成阻塞，使用寄存器不会
//	
//	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
//	USART6->DR = (uint8_t) ch;      
//	return ch;

//}

//#endif

////加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
//#if 1
////#pragma import(__use_no_semihosting)             
////标准库需要的支持函数                 
//struct __FILE 
//{
//	int handle; 
//}; 

//FILE __stdout;       
////定义_sys_exit()以避免使用半主机模式    
//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 
// //重定义fputc函数 
//int fputc(int ch, FILE *f)
//{ 	
//  //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//在中断中调用会造成阻塞，使用寄存器不会
//	
//	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
//	USART6->DR = (uint8_t) ch;      
//	return ch;
//}
//
//#endif


//定义文件的句柄
struct __FILE 
{
	int handle; 
	int eof_flagNone;//eof no flag
}; 

int fputc(int ch, FILE *f){
	if(f==stdout || f==stderr){
   //HAL_UART_Transmit(&huart6,(uint8_t*)&ch,1,2);//在中断中调用会造成阻塞，使用寄存器不会	
		while((PRINT_USART->SR&0X40)==0);//循环发送,直到发送完毕   
		PRINT_USART->DR = (uint8_t) ch;      
		return ch;
	}
	else{
		int handles=0;//默认为0	
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
//	  while(1);//不想接收就直接用while
		HAL_UART_Receive(&huart6,(uint8_t*)&ch,1,5000);//在中断中调用会造成阻塞，使用寄存器不会	
//		if(0==(USART6->SR&0X0020)){//判断是否接收到数据了
//		ch = USART6->DR;
//	}
		return ch;
	}
	else{
		int handles=0;//默认为0	
		handles = stream->handle;
		if (mmc_read(handles, &ch, 1)){
			//由于f_eof没有flag,加一个缓冲
		  if(mmcfs_feof(handles)){stream->eof_flagNone = 0;}
			return ch;
		}
		else{
		  stream->eof_flagNone = 1;
		return EOF;
		}
 }
}

/*重定向文件相关*/
// @ function: 
// @ description:feof
// @ input:
// @ note: 1.这里的feof与c语言中的feof有些不同,当使用fseek将文件指针移动到末尾时，feof仍然返回0,
//         但是当试图读取fsize+1时，feof会返回非零值
//         2.已经修改为支持flag
int feof(FILE * stream){
  int handles=0;//默认为0	
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
  int handles=0;//默认为0
  handles = stream->handle;
	free(stream);//在fopen中使用了malloc
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
	int handles=0;//默认为0
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
	int handles=0;//默认为0
  handles = stream->handle;
  return mmc_ltell(handles);
}

//FILEHANDLE _sys_open(const char *name, int openmode)//使用microlib 无法对_sys_open重定向
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
////检查句柄是否为终端
//int _sys_istty(FILEHANDLE fh)
//{
//	return 0;
//}
//int _sys_seek(FILEHANDLE fh, long pos)
//{
//	return mmc_lseek(fh, pos, SEEK_SET);
//}
////刷新句柄关联的缓冲区
//int _sys_ensure(FILEHANDLE fh)
//{
//	return 0;
//}

////返回文件当前长度
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
////将一个字符写入控制台
//void _ttywrch(int ch)
//{
//	//重写之后会无法初始化
////	while((USART6->SR&0X40)==0);//循环发送,直到发送完毕   
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

/* 定义lua_os lib中的部分函数 */

//time函数在loslib.c和lstate.c中调用,用于产生随机数
time_t time(time_t * time) {     
  return 0; 
}  

//exit()和system()仅仅在loslib.c中调用
void exit(int status) {      
  while(1);
}  

int system(const char * string) {     
  return 0; 
}

/* 重定向内存管理 */
//重定向后对OS会有影响？？
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
