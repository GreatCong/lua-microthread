/**
 *************************************************************************************
 * @file my_luaconf.h
 * @author lcj
 * @version 1.1
 * @date
 * @brief 在FreeRTOS中移植，更改地方
          task.h ---> #include
          task.c ---> 363行加入部分定义 以及 最后一行的#include
          port.c ---> 112行

          FreeRTOS.h ---> 设置configUSE_TRACE_FACILITY==1
          portmacro.h --> #include

          lua.h   ----> #include
          linit.c ----> 注册Lua
	--> 2018.4.10
					 注册lua函数不需要在linit中注册,增加的注册函数在lhardware.c中
					 note:
					     1.auxmods.h中设置模块的名称
							 2.my_luaconf.h中设置注册模块的表
							 3.myBoardDef.h中打开相应的宏，完成注册配置
  --> 2018.4.13
	         lua_adds.h --> 去除了doREPL的死循环,支持ctrl-D后reboot lua
					 bug：
					    1.线程执行前,FreeRTOS的Tasknum会莫名其妙地加1
							2.线程执行结束后,会执行删除线程函数,但是莫名其妙地多执行n(运行结束的Lua thread数)次
							  从而使得下一次创建线程的tag标号会加n
					 note:
					    在进行读文件的操作(如dofile)时,最好执行下ctrl-D,让lua main_thread进行一次循环
							释放其中的局部标量
 *************************************************************************************
*/
// Rename main in lua.c with lua_main
// Remove get_prompt in lua.c
// Rename pushline to luaos_pushline in lua.c

#ifndef WLUA_CONF
#define WLUA_CONF

#include "myBoardDef.h"


#if LUA_USE_LUA_LOCK
	void LuaLock(lua_State *L);
	void LuaUnlock(lua_State *L);
	
	#define lua_lock(L)          LuaLock(L)
	#define lua_unlock(L)        LuaUnlock(L)
	#define luai_threadyield(L) {lua_unlock(L); lua_lock(L);}
#else
	#define lua_lock(L)          
	#define lua_unlock(L)        
	#define luai_threadyield(L) 
#endif

#include "auxmods.h"

#include "unistd.h"

#undef  LUA_PROMPT
#define LUA_PROMPT		"> "

#undef  LUA_PROMPT2
#define LUA_PROMPT2		">> "

#undef  LUA_MAXINPUT
#define LUA_MAXINPUT 256
//
//#undef  LUA_ROOT
//#define LUA_ROOT	"/"
//
//#undef  LUA_LDIR
//#define LUA_LDIR	LUA_ROOT "lib/share/lua/"
//
//#undef  LUA_CDIR
//#define LUA_CDIR	LUA_ROOT "lib/lua/"
//
//#undef  LUA_COPYRIGHT
//#define LUA_COPYRIGHT	"Lua-Microthread " LUA_OS_VER " powered by " LUA_RELEASE 
//
//#undef  LUAI_THROW
//#define LUAI_THROW(L,c)	longjmp((c)->b, 1)
//
//#undef  LUAI_TRY
//#define LUAI_TRY(L,c,a)	if (setjmp((c)->b) == 0) { a }
//
//#undef  luai_jmpbuf
//#define luai_jmpbuf jmp_buf
//
//#undef LUA_TMPNAMBUFSIZE
//#define LUA_TMPNAMBUFSIZE	32
//
//#if !defined(LUA_TMPNAMTEMPLATE)
//#define LUA_TMPNAMTEMPLATE	"/tmp/lua_XXXXXX"
//#endif

//#define lua_tmpnam(b,e) { \
//        strcpy(b, LUA_TMPNAMTEMPLATE); \
//        e = mkstemp(b); \
//        if (e != -1) close(e); \
//        e = (e == -1); }


//#include <Lua/common/linenoise.h>
//#include "lstate.h"

//#undef  lua_readline
//#define lua_readline(L,b,p)     ((void)L, (linenoise(b, p)) != -1)//add by lcj

//#define lua_saveline(L,idx)     { (void)L; (void)idx; }
//#define lua_freeline(L,b)       { (void)L; (void)b; }

// Adds in standard Lua modukes

#define LBASELIB_REG_ADDS \
    {"dumpstack", stackDump}, \
    {"try", luaB_try}, 
        
#define LBASELIB_OPEN_ADDS 

#if LUA_USE_PIO
    #define LINIT_REG_PIO {AUXLIB_PIO, luaopen_pio},
#else
    #define LINIT_REG_PIO
#endif

#if LUA_USE_TMR
    #define LINIT_REG_TMR {AUXLIB_TMR, luaopen_tmr},
#else
    #define LINIT_REG_TMR
#endif

#if LUA_USE_CAN
    #define LINIT_REG_CAN {AUXLIB_CAN, luaopen_can},
#else
    #define LINIT_REG_CAN
#endif

#if LUA_USE_NET
    #define LINIT_REG_NET {AUXLIB_NET, luaopen_net},
#else
    #define LINIT_REG_NET
#endif

#if LUA_USE_ADC
    #define LINIT_REG_ADC {AUXLIB_ADC, luaopen_adc},
#else
    #define LINIT_REG_ADC
#endif

#if LUA_USE_SPI
    #define LINIT_REG_SPI {AUXLIB_SPI, luaopen_spi},
#else
    #define LINIT_REG_SPI
#endif

#if LUA_USE_MQTT
    #define LINIT_REG_MQTT {AUXLIB_MQTT, luaopen_mqtt},
#else
    #define LINIT_REG_MQTT
#endif

#if LUA_USE_THREAD
    #define LINIT_REG_THREAD {AUXLIB_THREAD, luaopen_thread },
#else
    #define LINIT_REG_THREAD
#endif

#if LUA_USE_SCREEN
    #define LINIT_REG_SCREEN {AUXLIB_SCREEN, luaopen_screen},
#else
    #define LINIT_REG_SCREEN
#endif

#if LUA_USE_UART
    #define LINIT_REG_UART {AUXLIB_UART, luaopen_uart},
#else
    #define LINIT_REG_UART
#endif

#if LUA_USE_PWM
    #define LINIT_REG_PWM {AUXLIB_PWM, luaopen_pwm},
#else
    #define LINIT_REG_PWM
#endif

#if LUA_USE_GPS
    #define LINIT_REG_GPS {AUXLIB_GPS, luaopen_gps},
#else
    #define LINIT_REG_GPS
#endif

#if LUA_USE_HTTP
    #define LINIT_REG_HTTP {AUXLIB_HTTP, luaopen_http},
#else
    #define LINIT_REG_HTTP
#endif

#if LUA_USE_STEPPER
    #define LINIT_REG_STEPPER {AUXLIB_STEPPER, luaopen_stepper},
#else
    #define LINIT_REG_STEPPER
#endif

#if LUA_USE_I2C
    #define LINIT_REG_I2C {AUXLIB_I2C, luaopen_i2c},
#else
    #define LINIT_REG_I2C
#endif

#if LUA_USE_LORA
    #define LINIT_REG_LORA {AUXLIB_LORA, luaopen_lora},
#else
    #define LINIT_REG_LORA
#endif
//add 		
#if LUA_USE_MACHINE //包含shell等信息
    #define LINIT_REG_MACHINE {AUXLIB_MACHINE, luaopen_machine},
#else
    #define LINIT_REG_MACHINE
#endif
#if LUA_USE_WIFI //WIFI
    #define LINIT_REG_WIFI {AUXLIB_WIFI, luaopen_wifi},
#else
    #define LINIT_REG_WIFI
#endif
#if LUA_USE_EXTERN_ADC //AD7606
    #define LINIT_REG_EXTERN_ADC {AUXLIB_EXTERN_ADC, luaopen_externADC},
#else
    #define LINIT_REG_EXTERN_ADC
#endif
//add end

#define LINIT_REG_ADDS \
  LINIT_REG_PIO \
  LINIT_REG_TMR \
  LINIT_REG_CAN \
  LINIT_REG_NET \
  LINIT_REG_ADC \
  LINIT_REG_SPI \
  LINIT_REG_MQTT \
  LINIT_REG_THREAD \
  LINIT_REG_SCREEN \
  LINIT_REG_UART \
  LINIT_REG_PWM \
  LINIT_REG_GPS \
  LINIT_REG_HTTP \
  LINIT_REG_STEPPER \
  LINIT_REG_I2C \
  LINIT_REG_LORA \
  LINIT_REG_PACK
      
#define LINIT_OPEN_ADDS

#define LIOLIB_REG_ADDS {"receive", f_receive}, {"send", f_send},
#define LIOLIB_OPEN_ADDS

#if LUA_USE_SHELL
    #define LOSLIB_REG_SHELL {"shell",     os_shell},
#else
    #define LOSLIB_REG_SHELL
#endif

#if LUA_USE_EDITOR
    #define LOSLIB_REG_EDITOR {"edit",      os_edit},
#else
    #define LOSLIB_REG_EDITOR
#endif

#if 0
#define LOSLIB_REG_ADDS \
  LOSLIB_REG_SHELL \
  {"history",   os_history}, \
  {"cp",        os_cp}, \
  {"ls",        os_ls}, \
  {"cd",        os_cd}, \
  {"pwd",       os_pwd}, \
  {"mkdir",     os_mkdir}, \
  LOSLIB_REG_EDITOR \
  {"clear",     os_clear}, \
  {"cat",       os_cat}, \
  {"more",      os_more}, \
  {"dmesg",     os_dmesg}, \
  {"format",    os_format}, \
  {"idle",      os_idle}, \
  {"loglevel",  os_loglevel}, \
  {"logcons",   os_logcons}, \
  {"stats",     os_stats}, \
  {"version",   os_version}, \
  {"cpu",       os_cpu}, \
  {"run",       os_run}, 
#else
#define LOSLIB_REG_ADDS
#endif

#if 0	  	 
#define LOSLIB_OPEN_ADDS \
  os_constants(L);
#else
  #define LOSLIB_OPEN_ADDS
#endif


//#ifdef lbaselib_c
//#undef lbaselib_c
//#include <Lua/modules/lbaselib_adds.inc>
//#endif
//
//#ifdef liolib_c
//#undef liolib_c
//#include <Lua/modules/liolib_adds.inc>
//#include <sys/syscalls/mount.h>
//#endif
//
//#ifdef loslib_c
//#undef loslib_c
//#include <Lua/modules/loslib_adds.inc>
//#endif

#ifdef lua_c
#undef lua_c

static int  report (lua_State *L, int status);
static void l_message (const char *pname, const char *msg);
static int  runargs (lua_State *L, char **argv, int n);
static void print_version (void);
static void doREPL (lua_State *L);
static void print_usage (const char *badoption);
static int  collectargs (char **argv, int *first);
static void createargtable (lua_State *L, char **argv, int argc, int script);

#include "lauxlib.h"
#include "lualib.h"
#include "lua_modules.h"

#include "lua_adds.h"

#endif
			
#endif
