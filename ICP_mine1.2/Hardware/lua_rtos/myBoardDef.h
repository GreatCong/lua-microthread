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

#define DEBUG_FREE_MEM 0//开启Debug debug.h

/************** default define **************************/
#define HISTORY_DEFAULT_STATE 0 //history log



#endif	/* CONFIG_H */
