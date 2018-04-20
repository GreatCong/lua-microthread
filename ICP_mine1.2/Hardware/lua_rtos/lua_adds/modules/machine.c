
#include "myBoardDef.h"

#if LUA_USE_MACHINE

#include "lua.h"
#include "lauxlib.h"
#include "Lua_modules.h"

#include <unistd.h>

#if LUA_USE_RUNRTOS
#include "FreeRTOS.h"
#include "Task.h"
extern int luaTaskStack;
extern int luaTaskStack_init;
#endif

int lua_history_on = HISTORY_DEFAULT_STATE;

//static int os_lua_running(lua_State *L) { 
//    lua_pushboolean(L, status_get(STATUS_LUA_RUNNING));

//    return 1;
//}

//static int os_lua_interpreter(lua_State *L) { 
//    lua_pushboolean(L, status_get(STATUS_LUA_INTERPRETER));

//    return 1;
//}

static int os_history(lua_State *L) { 
    luaL_checktype(L, 1, LUA_TBOOLEAN);
    lua_history_on = lua_toboolean( L, 1 );
	
    lua_pushboolean(L, lua_history_on); 
	
    return 1;
}

static int os_clear_history(lua_State *L) { 
  FILE *fp;
  const char *fname;
	
	luaL_checktype(L, 1, LUA_TBOOLEAN);
	int clear_on = lua_toboolean( L, 1 );
	
	if(lua_history_on)//避免了同时打开同一个文本的情况
		return luaL_error(L, "history_on=true!");
	
  if(clear_on){
		fname = "/history";		
		fp = fopen(fname,"w");	 
			if (!fp) {
					return luaL_error(L, "can not open file %s!",fname);
			}
		fclose(fp);
			
	}	
    
	lua_pushboolean(L, clear_on); 
	
    return 1;
}

static int os_version(lua_State *L) {
   lua_pushstring(L, "Lua-Microthread");   
   lua_pushstring(L, LUA_OS_VER);   
   lua_pushinteger(L, BUILD_TIME); 
   
   return 3;
}

#if LUA_USE_RUNRTOS
static int os_get_mainstack(lua_State *L) { 
    lua_pushinteger(L, luaTaskStack); 
	
    return 1;
}

static int os_set_mainstack(lua_State *L) {
    int stack;
    stack = luaL_checkinteger( L, 1 );
    if(luaTaskStack_init){
		   return luaL_error(L, "luaTaskStack_init=true,luaTaskStack=%d",luaTaskStack);
		} 	
    if(luaTaskStack < configMINIMAL_STACK_SIZE){
		   return luaL_error(L, "param error!");
		} 	

		luaTaskStack = stack;
	
    return 0;
}

static int os_get_allTask(lua_State *L) {
	#if ( configUSE_TRACE_FACILITY == 1 ) //开启FreeRTOS的debug
	TaskStatus_t *statusArray;
	UBaseType_t task_num;
		
	uint32_t TotalRunTime;
	int arraySize = 0;

	task_num = uxTaskGetNumberOfTasks();      //获取任务数量
	printf("find %ld tasks in total!\r\n", task_num);
	printf("memory free/total:%d / %d Byte\r\n\r\n",xPortGetFreeHeapSize(),configTOTAL_HEAP_SIZE);

	statusArray = pvPortMalloc(task_num*sizeof(TaskStatus_t));//申请内存
	if(statusArray!=NULL)                   //内存申请成功
	{
			arraySize=uxTaskGetSystemState((TaskStatus_t*   )statusArray,   //任务信息存储数组
																		 (UBaseType_t     )task_num,  //任务信息存储数组大小
																		 (uint32_t*       )&TotalRunTime);//保存系统总的运行时间
			assert_param(arraySize);
		  printf("********************Task Message Table*********************\r\n\r\n");
			printf("    TaskName\tPriority\tTaskNumber\tTaskFree\t\r\n");
			for( int x=0;x < task_num;x++)
			{
					printf("%12s\t%4d\t\t%4d\t\t%4d\t\r\n",                
									statusArray[x].pcTaskName, //任务名称
									(int)statusArray[x].uxCurrentPriority, //任务优先级
									(int)statusArray[x].xTaskNumber, //任务编号
									statusArray[x].usStackHighWaterMark);

			}
	}
	else
	{
     return luaL_error(L, "not enough memory!");
	}
   vPortFree(statusArray); //释放内存
	#else
	printf("configUSE_TRACE_FACILITY=0,debug off!\r\n");
	#endif
   return 0;
}
#endif


static const LUA_REG_TYPE machine_map[] = {
    { LSTRKEY( "history" ),			LFUNCVAL( os_history ) },
    { LSTRKEY( "version" ),			LFUNCVAL( os_version ) },
		{ LSTRKEY( "clclog" ),			LFUNCVAL( os_clear_history ) },
		#if LUA_USE_RUNRTOS
		{ LSTRKEY( "setmainstack" ),			LFUNCVAL( os_set_mainstack ) },
		{ LSTRKEY( "getmainstack" ),			LFUNCVAL( os_get_mainstack ) },
		{ LSTRKEY( "tasklist" ),			    LFUNCVAL( os_get_allTask ) },
		#endif
    { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_machine( lua_State *L ) {
#if !LUA_USE_ROTABLE
    luaL_newlib(L, machine_map);

    return 1;
#else
	return 0;
#endif
}

LUA_OS_MODULE(TMR, machine, machine_map);

#endif
