/*
 * Whitecat main configuration file
 *
 * Copyright (C) 2015 - 2016
 * IBEROXARXA SERVICIOS INTEGRALES, S.L. & CSS IBÉRICA, S.L.
 * 
 * Author: Jaume Olivé (jolive@iberoxarxa.com / jolive@whitecatboard.org)
 * 
 * All rights reserved.  
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */

#ifndef __MY_BOARD_DEF_H_
#define	__MY_BOARD_DEF_H_

#define LUA_OS_VER " 1.0 beta "
//#define BUILD_TIME __DATE__ //__DATE__为MDK自带的build time 输出%s 如 Apr 16 2018
#define BUILD_TIME 20180413
#define LUA_MYCOPYRIGHT	"Lua-Microthread " LUA_OS_VER " powered by " LUA_RELEASE 

//#define UNUSED_ARG(x) (void)(x)

/*********** lua modules ****************/
#define LUA_USE_THREAD 1 //add by lcj	
#define LUA_USE_PIO 1 //add by lcj	
#define LUA_USE_TMR 1
#define LUA_USE_MACHINE 1

#define LUA_USE_HISTORY 1
#define LUA_USE_XPRINTF 0
#define SHELL_USE_WINDOWS_LINE 1 // \r\n
#define LUA_USE_MYMALLOC 1 //是否使用自定义内存管理函数
#define LUA_USE_CMSISOS 1 //是否使用模拟cmsis_os优先级

#define DEBUG_FREE_MEM 0//开启Debug debug.h

/************** default define **************************/
#define HISTORY_DEFAULT_STATE 0 //history log
#define LUA_MAINSTACK_DEF 1024 //lua main stack

#if LUA_USE_CMSISOS
//模拟cmsis_os优先级
#define LUA_CONF_PriorityIdle 0 ///< priority: idle (lowest)
#define LUA_CONF_PriorityLow 1 ///< priority: below normal
#define LUA_CONF_PriorityBelowNormal 2  ///< priority: normal (default)
#define LUA_CONF_PriorityNormal 3 ///< priority: above normal
#define LUA_CONF_PriorityAboveNormal 4 ///< priority: high
#define LUA_CONF_PriorityHigh 5 ///< priority: realtime (highest)
#define LUA_CONF_PriorityRealtime 6
#endif

//这里仅仅针对linenoise的配置
#define LUA_OUTPUT_UART USART6 //定义UART6为lua print输出
#define LUA_INPUT_UART USART6 //定义UART6为lua 交互输入

#endif	/* CONFIG_H */
