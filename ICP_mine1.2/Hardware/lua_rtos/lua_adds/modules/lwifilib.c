/**
 *************************************************************************************
 * @file 
 * @author liucongjun
 * @version
 * @date
 * @brief lua wifi
 *************************************************************************************
*/
#include "myBoardDef.h"

#if LUA_USE_WIFI

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include <stdio.h>
#include <stdlib.h>

int Lua_wifi_on = WIFI_DEFAULT_STATE;

static int lwifi_init(lua_State *L)
{
	int state;
	  luaL_checktype(L, 1, LUA_TBOOLEAN);
    state = lua_toboolean( L, 1 );
	  if(Lua_wifi_on < 0)
			return luaL_error(L, "wifi is initialized!");
		else
			Lua_wifi_on = state;
	
    lua_pushboolean(L, Lua_wifi_on); 
	
    return 1;
}

/**===============================================
 **/

#include "Lua_modules.h"

static const LUA_REG_TYPE wifiLib[] = {
    { LSTRKEY( "init"),			  LFUNCVAL( lwifi_init ) },
		
    { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_wifi( lua_State *L ) {
#if !LUA_USE_ROTABLE
    luaL_newlib(L, wifiLib);

    return 1;
#else
	return 0;
#endif
}

//LUA_OS_MODULE(TMR, machine, machine_map);

#endif
