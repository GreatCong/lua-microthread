/**
 *************************************************************************************
 * @file 
 * @author liucongjun
 * @version
 * @date
 * @brief lua gpio
 *************************************************************************************
*/
#include "myBoardDef.h"

#if LUA_USE_PIO

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdio.h>
#include <stdlib.h>

#include "pio.h"
#include "makeparam.h"

/* gpio LIB type,used for create param */
#define GPIO_LIB_TYPE	'g'

#define LGPIO_INPUT        MAKE_P(GPIO_LIB_TYPE,1,GPIO_INPUT)
#define LGPIO_OUTPUT_PP 	 MAKE_P(GPIO_LIB_TYPE,2,GPIO_OUTPUT_PP)
#define LGPIO_ANALOG       MAKE_P(GPIO_LIB_TYPE,3,GPIO_ANALOG)
#define LGPIO_OUTPUT_OD    MAKE_P(GPIO_LIB_TYPE,4,GPIO_OUTPUT_OD)

#define LGPIO_PULLNO   MAKE_P(GPIO_LIB_TYPE,5,GPIO_NOPULL)
#define LGPIO_PULLUP   MAKE_P(GPIO_LIB_TYPE,6,GPIO_PULLUP)
#define LGPIO_PULLDOWN MAKE_P(GPIO_LIB_TYPE,7,GPIO_PULLDOWN)

#define LGPIO_STATE_HIGH   MAKE_P(GPIO_LIB_TYPE,5,GPIO_PIN_SET)
#define LGPIO_STATE_LOW    MAKE_P(GPIO_LIB_TYPE,6,GPIO_PIN_RESET)

#define is_lgpio_mode(mode) ( (P_TYPE(mode)==GPIO_LIB_TYPE) && \
                              (P_NUM(mode)>=1) && (P_NUM(mode)<=4) )
#define is_lgpio_pull(pull) ( (P_TYPE(pull)==GPIO_LIB_TYPE) && \
                              (P_NUM(pull)>=5) && (P_NUM(pull )<=7) ) 
															
struct pininfo
{
	GPIO_TypeDef *GPIO;
	uint16_t GPIO_Pin;
};

const struct pininfo pininfo[]=
{
	{NULL,0},//PA3(Rx)
	{NULL,0},//PA2(Tx)
	{GPIOA,GPIO_PIN_10},
	{GPIOB,GPIO_PIN_3},
	{GPIOB,GPIO_PIN_5},
	{GPIOB,GPIO_PIN_4},
	{GPIOB,GPIO_PIN_10},
	{GPIOA,GPIO_PIN_8},//led0 samlpe
/*------------------*/
	{GPIOA,GPIO_PIN_9},
	{GPIOC,GPIO_PIN_7},
	{GPIOB,GPIO_PIN_6},
	{NULL,0},//PA7(MOSI)
	{NULL,0},
	{NULL,0},
	{NULL,0},
	{NULL,0},
/*------------------*/
	{GPIOA,GPIO_PIN_0},
	{GPIOA,GPIO_PIN_1},
	{GPIOA,GPIO_PIN_4},
	{GPIOB,GPIO_PIN_0},
	{GPIOC,GPIO_PIN_1},
	{GPIOC,GPIO_PIN_0},
	
	{GPIOB,GPIO_PIN_12},//led3
	{GPIOB,GPIO_PIN_13},//led2
	{GPIOB,GPIO_PIN_14} //led1
};

#define pin2GPIO(pin) ( pininfo[pin].GPIO )
#define pin2pin(pin)	( pininfo[pin].GPIO_Pin)

/* enum in platform.h 
 *
 * enum {GPIO_INPUT,GPIO_OUTPUT,GPIO_ANALOG};
 * enum {GPIO_PULLNO,GPIO_PULLUP,GPIO_PULLDOWN};
 */
 
static const uint32_t gpio_mode_buff[12] =
 {
 GPIO_MODE_INPUT,
 GPIO_MODE_OUTPUT_PP,
 GPIO_MODE_ANALOG,
 GPIO_MODE_OUTPUT_OD,//暂时只是用到前4个
 GPIO_MODE_AF_PP,
 GPIO_MODE_AF_OD,
	 
// GPIO_MODE_ANALOG,
	 
 GPIO_MODE_IT_RISING,
 GPIO_MODE_IT_FALLING,
 GPIO_MODE_IT_RISING_FALLING,
 
 GPIO_MODE_EVT_RISING,
 GPIO_MODE_EVT_FALLING,
 GPIO_MODE_EVT_RISING_FALLING
 };
static const uint32_t gpio_pupd_buff[3] =
 {GPIO_NOPULL,GPIO_PULLUP,GPIO_PULLDOWN};

int gpio_mode(int pin,int mode,int pull)
{
	DECLARE_GPIO_INIT(ioIt,OUTPUT_PP,NOPULL,HIGH);
	
	if(!DECLARE_is_gpio_pin(pin) || !DECLARE_is_gpio_mode(mode) || !DECLARE_is_gpio_pull(pull) )
		return -1;
	ioIt.Pin=pin2pin(pin);
	ioIt.Mode=gpio_mode_buff[mode];
	ioIt.Pull=gpio_pupd_buff[pull];
	
	HAL_GPIO_Init(pin2GPIO(pin), &ioIt);
	
	return 0;
}

int gpio_write(int pin,int state)
{
	if(!DECLARE_is_gpio_pin(pin)) 
		return -1;
	HAL_GPIO_WritePin(pin2GPIO(pin),pin2pin(pin),(GPIO_PinState)state);
//	if(value) pin2GPIO(pin)->BSRR = pin2pin(pin);
//	else			pin2GPIO(pin)->BRR  = pin2pin(pin);
	return 0;
}

int gpio_read(int pin)
{
	if(!DECLARE_is_gpio_pin(pin)) 
		return -1;
//	return GPIO_ReadInputDataBit(pin2GPIO(pin),pin2pin(pin));
	return HAL_GPIO_ReadPin(pin2GPIO(pin),pin2pin(pin));
}

int gpio_toggle(int pin)
{
	if(!DECLARE_is_gpio_pin(pin))
		return -1;
//	pin2GPIO(pin)->ODR ^= pin2pin(pin);
	HAL_GPIO_TogglePin(pin2GPIO(pin),pin2pin(pin));
	return 0;
}


/**
 ** Lua function: gpio.mode(pin,mode,pull)
 ** mode can be : gpio.INPUT,gpio.OUTPUT,gpio.ANALOG
 ** pin,mode is must.pull is selected
 **/
static int lgpio_mode(lua_State *L)
{
	int pin,mode,paramcount;
	int pull = LGPIO_PULLNO;
	
	paramcount=lua_gettop(L);
	pin=luaL_checkinteger(L,1);
	mode=luaL_checkinteger(L,2);
	if(paramcount>2)
		pull=luaL_checkinteger(L,3);
		
	if(!is_lgpio_mode(mode) || !is_lgpio_pull(pull) ){
		return luaL_error(L, "param error! gpio.mode(pin,mode) ");
//		lua_pushstring(L,"param error! gpio.mode(pin,mode)\n");
//		lua_error(L);
	}

	if(gpio_mode(pin,P_DATA(mode),P_DATA(pull))){
		return luaL_error(L, "param error! gpio.mode(pin,mode) ");
//		lua_pushstring(L,"param error! gpio.mode(pin,mode)\n");
//		lua_error(L);
	}

	return 0; /* lua return value */
}

/**
 ** Lua function: gpio.write(pin,val)
 **/
static int lgpio_write(lua_State *L)
{
	int pin,val;
	pin=luaL_checkinteger(L,1);
	val=luaL_checkinteger(L,2);

	if(gpio_write(pin,val)){
		return luaL_error(L, "param error! GPIO.write(pin,value) ");
//		lua_pushstring(L,"param error! GPIO.write(pin,value) \n");
//		lua_error(L);
	}

	return 0;
}

/**
 ** Lua function: gpio.read(pin,...)
 ** This function is a multi-return function.
 ** You can pass all pin's number you want read.(at least one pin)
 ** Note: 
 **  If a pin not a really pin,this function will return -1 as it's value.
 ** Rev: the value of the pin
 **/
static int lgpio_read(lua_State *L)
{
	int i,pin,count;
//	int value;
	count = lua_gettop(L);/* all pin's count need read */

	for(i=0;i<count;i++)
	{
		pin=luaL_checkinteger(L,i+1);/* lua index from 1,NOT zero */
		lua_pushinteger(L,gpio_read(pin));
	}
	return i;
}

static int lgpio_toggle(lua_State *L)
{
	int pin=luaL_checkinteger(L,1);
	if(gpio_toggle(pin)){
//		lua_pushstring(L,"param not a really pin!\n");
//		lua_error(L);
		return luaL_error(L, "param not a really pin!");
	}
	return 0;
}

/**===============================================
 **/

#include "Lua_modules.h"

static const LUA_REG_TYPE gpioLib[] = {
    { LSTRKEY( "mode"),			  LFUNCVAL( lgpio_mode ) },
    { LSTRKEY( "write" ),			LFUNCVAL( lgpio_write ) },
    { LSTRKEY( "read" ),			LFUNCVAL( lgpio_read ) },
    { LSTRKEY( "toggle" ),		LFUNCVAL( lgpio_toggle ) },
    
		/* placeholders */
    {"IN",NULL},
    {"OUTPP",NULL},
		{"OUTOD",NULL},
    {"AN",NULL},
		
    {"UP",NULL},
    {"DOWN",NULL},
		
		{"HIGH",NULL},
		{"LOW",NULL},
    
		{"LED0",NULL},
		{"LED1",NULL},
		{"LED2",NULL},
		{"LED3",NULL},
		
    {"A0",NULL},
		
    { LNILKEY, LNILVAL }
};

/**
 **Open gpio lib
 */
LUAMOD_API int luaopen_pio (lua_State *L)
{
    luaL_newlib(L,gpioLib);

    lua_pushinteger(L,LGPIO_INPUT);
    lua_setfield(L,-2,"IN");
    lua_pushinteger(L,LGPIO_OUTPUT_PP);
    lua_setfield(L,-2,"OUTPP");//lua对下划线的支持不太好，命名不要用下划线
	  lua_pushinteger(L,LGPIO_OUTPUT_OD);
    lua_setfield(L,-2,"OUTOD");
    lua_pushinteger(L,LGPIO_ANALOG);
    lua_setfield(L,-2,"AN");
    lua_pushinteger(L,LGPIO_PULLUP);
    lua_setfield(L,-2,"PULLUP");
    lua_pushinteger(L,LGPIO_PULLDOWN);
    lua_setfield(L,-2,"PULLDOWN");
	  lua_pushinteger(L,LGPIO_PULLNO);
    lua_setfield(L,-2,"PULLNO");
	
	  lua_pushinteger(L,LGPIO_STATE_HIGH);
    lua_setfield(L,-2,"HIGH");
	  lua_pushinteger(L,LGPIO_STATE_LOW);
    lua_setfield(L,-2,"LOW");
    
    lua_pushinteger(L,PIO_LED0);
    lua_setfield(L,-2,"LED0");  
	  lua_pushinteger(L,PIO_LED1);
    lua_setfield(L,-2,"LED1");  
	  lua_pushinteger(L,PIO_LED2);
    lua_setfield(L,-2,"LED2");  
		lua_pushinteger(L,PIO_LED3);
    lua_setfield(L,-2,"LED3");  
	
    lua_pushinteger(L,PIO_A0);
    lua_setfield(L,-2,"A0");

    return 1;
}

#endif







