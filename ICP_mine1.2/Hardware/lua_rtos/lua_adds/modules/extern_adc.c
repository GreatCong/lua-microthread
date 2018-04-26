/**
 *************************************************************************************
 * @file 
 * @author liucongjun
 * @version
 * @date
 * @brief lua AD7606
 *************************************************************************************
*/
#include "myBoardDef.h"

#if LUA_USE_EXTERN_ADC

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdio.h>
#include <stdlib.h>

#include "AD7606.h"

extern int AD_Conrtol_state;//my_task.c

static int lexternADC_CH_ctrl(lua_State *L)
{
	  int ch;
    ch = 	luaL_checkinteger(L,1);
	  
		AD_CH_ctrl = (uint8_t)ch;
	
    return 0;
}

static int lexternADC_Run(lua_State *L)
{
	  int state;
	  luaL_checktype(L, 1, LUA_TBOOLEAN);
    state = lua_toboolean( L, 1 );
	  
	  AD_Conrtol_state = state;
	
    lua_pushboolean(L, AD_Conrtol_state); 
	
    return 1;
}


/**===============================================
 **/

#include "Lua_modules.h"

static const LUA_REG_TYPE externADC[] = {
    { LSTRKEY( "channel"),		LFUNCVAL( lexternADC_CH_ctrl ) },
		{ LSTRKEY( "run"),			  LFUNCVAL( lexternADC_Run ) },
		
		/* placeholders */
    {"CH1",NULL},
    {"CH2",NULL},	
    {"CH3",NULL},
    {"CH4",NULL},		
		
    { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_externADC( lua_State *L ) {
#if !LUA_USE_ROTABLE
    luaL_newlib(L, externADC);
	
	  lua_pushinteger(L,AD_CH1);
    lua_setfield(L,-2,"CH1");
    lua_pushinteger(L,AD_CH2);
    lua_setfield(L,-2,"CH2");
	  lua_pushinteger(L,AD_CH3);
    lua_setfield(L,-2,"CH3");
    lua_pushinteger(L,AD_CH4);
    lua_setfield(L,-2,"CH4");

    return 1;
#else
	return 0;
#endif
}

//LUA_OS_MODULE(TMR, machine, machine_map);


#endif
