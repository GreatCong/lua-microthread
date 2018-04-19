/*
* Whitecat, main program
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
#include "myBoardDef.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portable.h"
#include "timers.h"
#include "event_groups.h"
//#include "lua.h"

//#include <machine/pic32mz.h>
//#include <machine/machConst.h>
//#include <sys/drivers/gpio.h>
//#include <sys/drivers/uart.h>
//#include <sys/syslog.h>
#include "sys/panic_sys.h"
//#include <drivers/network/network.h>
//#include <drivers/lora/lora.h>
//#include <utils/delay.h>

#include <stdio.h>
//#include <fcntl.h>

//#include <stdio.h>
//#include <sys/param.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <errno.h>
//#include <stdlib.h>
//#include <string.h>
//#include <syslog.h>
//#include <stdarg.h>
//#include <dirent.h>
//#include <time.h>
//#include <setjmp.h>

#include "pthread.h"
#include <stdlib.h>
//#include <sys/delay.h>
#include <sys/debug.h>

extern const char *__progname;

// #include "math.h"

//add by lcj
//#ifndef luaTaskStack
//#define luaTaskStack 192 * 5
//#endif 
extern int Lua_dofile_script(const char *filename);
int luaTaskStack = LUA_MAINSTACK_DEF;
int luaTaskStack_init=0;
EventGroupHandle_t Lua_groupGets_handle;
//add by lcj END

int luaos_main(void);

void *lua_start(void *arg) {
	for (;;) {
		luaos_main();
		//printf("ok ");
		//vTaskDelay(100);
	}

	return NULL;
}

//extern char *normalize_path(const char *path);
//extern double nmea_geoloc_to_decimal(char *token);

//add by lcj
//extern void mach_dev();
//extern void _clock_init();
//extern void _syscalls_init();
extern void _pthread_init(void);
//extern void _console_init();
//extern void _lora_init();
//extern void _signal_init();
extern void _mtx_init(void);

void mach_init() {
	Lua_groupGets_handle = xEventGroupCreate();//设置串口接收时间标志
	//resource_init();
	_mtx_init();
	_pthread_init();
	/*_clock_init();
	_syscalls_init();
	_console_init();
	_signal_init();*/

//#if LUA_USE_LORA
//	_lora_init();
//#endif
//
//	mach_dev();
}

static void lua_boot(void){
	int  system;
  FILE *fp;
  int i;
  const char *system_order[2];
  printf("\r\nluaos_boot_scripts...\r\n");
	
   system_order[0] = "/system.lua";
   system_order[1] = NULL;

    printf("\r\n");
    
    // Ecexute system script
    system = -1;
    for(i = 0; i < sizeof(system_order) / sizeof(*system_order); i++) {
        if (system_order[i]) {
					fp = fopen(system_order[i], "r" );
            if (fp) {
                fclose(fp);
                system = i;
                break;
            }            
        }
    }
    
    if (system >=0) {
        printf("Executing %s ...\n", system_order[system]);
        Lua_dofile_script(system_order[system]);
    }    
    
    printf("\r\n");
		
		#if LUA_USE_MYMALLOC
		my_mem_init(LUA_MYMEM_ADDR);//重新初始化LUA_MYMEM_ADDR地址的内存(默认为CCM)
		#endif
}
//add by lcj end

void lua_app_main(void) {

	pthread_attr_t attr;
	pthread_t thread;
	int res;

	mach_init();
	luaTaskStack_init = 0;
	lua_boot();//启动需要2048的堆栈 测试为4400byte

	debug_free_mem_begin(lua_main_thread);//lua_main_thread是表示返回值，是对堆的检查

	pthread_attr_init(&attr);
	
	#if LUA_USE_CMSISOS
	struct sched_param param; //PC机不做设置
	param.sched_priority = LUA_CONF_PriorityBelowNormal;	
	pthread_attr_setschedparam(&attr,&param);
	#endif

	pthread_attr_setstacksize(&attr, luaTaskStack);
	luaTaskStack_init = 1;

	res = pthread_create(&thread, &attr, lua_start, NULL);
	if (res) {
		panic("Cannot start lua");
	}

	debug_free_mem_end(lua_main_thread, NULL);
}

