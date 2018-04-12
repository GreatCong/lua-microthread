//这个函数仅仅在linux中使用
//int usleep(__useconds_t useconds)
//{
//	struct timespec ts;
//
//	ts.tv_sec = (long int)useconds / 1000000;
//	ts.tv_nsec = ((long int)useconds % 1000000) * 1000;
//	if (!nanosleep(&ts, &ts)) return 0;
//	if (errno == EINTR) return ts.tv_sec;
//	return -1;
//}

#include "stdint.h"

//#ifndef __useconds_t
//#define __useconds_t uint32_t
//#endif

//int usleep(int useconds)
//{
//	for (int i = 0; i < useconds; i++) { };
//
//	return 0;
//}
//
//#include "Windows.h"
//int sleep(int n){
//	Sleep(n);
//	return 0;
//}

#include "FreeRTOS.h"
#include "task.h"

int sleep(int secs) {
	vTaskDelay((secs * 1000) / ((TickType_t)1000 / configTICK_RATE_HZ));

	return 0;
}

int usleep(int usec) {
	vTaskDelay(usec / ((TickType_t)1000000 / configTICK_RATE_HZ));

	return 0;
}


/*定义lua-rtos中断相关*/
#if defined __CC_ARM 
#    if __CM4_CMSIS_VERSION < 0x00020000
bool tt_is_in_irq (void)                              //__get_IPSR()得到当前程序状态寄存器的值，用以判断是否处于中断状态
{
    register uint32_t __regIPSR          __asm("ipsr");
    return(__regIPSR);
}    
#    else
bool tt_is_in_irq (void)
{
    return __get_IPSR ();
}
#endif
#elif defined __GNUC__
__attribute__((always_inline )) TT_INLINE bool tt_is_in_irq (void)
{
    uint32_t result;
    __asm__ volatile ("MRS %0, ipsr" : "=r" (result) );
    return(result);
}
#elif defined __ICCARM__
#    pragma diag_suppress=Pe940
bool tt_is_in_irq (void)
{
    __asm("    MRS R0, IPSR    ");
}
#endif

///*
// * ARM Compiler 4/5
// */
//#if   defined ( __CC_ARM )

//  #define __ASM            __asm                                      
//  #define __INLINE         __inline                                     
//  #define __STATIC_INLINE  static __inline

//  #include "cmsis_armcc.h"

///*
// * GNU Compiler
// */
//#elif defined ( __GNUC__ )

//  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
//  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
//  #define __STATIC_INLINE  static inline

//  #include "cmsis_gcc.h"


///*
// * IAR Compiler
// */
//#elif defined ( __ICCARM__ )

//  #ifndef   __ASM
//    #define __ASM                     __asm
//  #endif
//  #ifndef   __INLINE
//    #define __INLINE                  inline
//  #endif
//  #ifndef   __STATIC_INLINE
//    #define __STATIC_INLINE           static inline
//  #endif

//  #include <cmsis_iar.h>
//#endif
//		
///* Determine whether we are in thread mode or handler mode. */
//int inISRMode (void)//add by lcj keil
//{
//  return __get_IPSR();
//}

