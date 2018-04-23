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

#include "NetDef.h"

Wifi_state Lua_wifi_state = {
  .net_on = WIFI_DEFAULT_STATE,
	.net_mode = NET_MODE_STA,
	.net_protocol = NET_TCP_CLIENT
};


//int Lua_wifi_on = WIFI_DEFAULT_STATE;

static int lwifi_init(lua_State *L)
{
	int state;
	  luaL_checktype(L, 1, LUA_TBOOLEAN);
    state = lua_toboolean( L, 1 );
	  if(Lua_wifi_state.net_on < 0)
			return luaL_error(L, "wifi is initialized!");
		else
			Lua_wifi_state.net_on = state;
	
    lua_pushboolean(L, Lua_wifi_state.net_on); 
	
    return 1;
}

static int lwifi_setParams(lua_State *L)
{
	int mode,protocol;
	mode=luaL_checkinteger(L,1);
	protocol=luaL_checkinteger(L,2);
	
	  if(Lua_wifi_state.net_on < 0)
			return luaL_error(L, "wifi is initialized!");
		else{
		  Lua_wifi_state.net_mode = mode;
			Lua_wifi_state.net_protocol = protocol;
		}
	
    return 0;
}

static int lwifi_getParams(lua_State *L)
{
		uint8_t table = 0;
	
		if (lua_gettop(L) == 1) {
		luaL_checktype(L, 1, LUA_TBOOLEAN);
		if (lua_toboolean(L, 1)) {
			table = 1;
		}
	}
	if(!table){	
		switch(Lua_wifi_state.net_mode){
			case NET_MODE_STA:
				printf("STA,");
				break;
			case NET_MODE_AP:
				printf("AP,");
				break;
			default:
				break;
		}
		
		switch(Lua_wifi_state.net_protocol){
			case NET_TCP_SERVER:
				printf("TCP_SERVER\r\n");
				break;
			case NET_TCP_CLIENT:
				printf("TCP_CLIENT\r\n");
				break;
			case NET_UDP_SERVER:
				printf("UDP_SERVER\r\n");
				break;
			case NET_UDP_CLIENT:
				printf("UDP_CLIENT\r\n");
				break;
			default:
				break;
		}
	}
	else{
			lua_createtable(L, 0, 2);

			lua_pushinteger(L, Lua_wifi_state.net_mode);
			lua_setfield(L, -2, "mode");

			lua_pushinteger(L, Lua_wifi_state.net_protocol);
			lua_setfield(L, -2, "protocol");
	}
			
    return table;
}

/**===============================================
 **/

#include "Lua_modules.h"

static const LUA_REG_TYPE wifiLib[] = {
    { LSTRKEY( "init"),			  LFUNCVAL( lwifi_init ) },
		{ LSTRKEY( "setparam"),			  LFUNCVAL( lwifi_setParams ) },
		{ LSTRKEY( "getparam"),			  LFUNCVAL( lwifi_getParams ) },
		
		/* placeholders */
    {"STA",NULL},
    {"AP",NULL},
		
    {"TCPS",NULL},
    {"TCPC",NULL},
		
		{"UDPS",NULL},
		{"UDPC",NULL},
		
    { LNILKEY, LNILVAL }
};

LUALIB_API int luaopen_wifi( lua_State *L ) {
#if !LUA_USE_ROTABLE
    luaL_newlib(L, wifiLib);
	
	  lua_pushinteger(L,NET_MODE_STA);
    lua_setfield(L,-2,"STA");
    lua_pushinteger(L,NET_MODE_AP);
    lua_setfield(L,-2,"AP");
	  lua_pushinteger(L,NET_TCP_SERVER);
    lua_setfield(L,-2,"TCPS");
    lua_pushinteger(L,NET_TCP_CLIENT);
    lua_setfield(L,-2,"TCPC");
    lua_pushinteger(L,NET_UDP_SERVER);
    lua_setfield(L,-2,"UDPS");
    lua_pushinteger(L,NET_UDP_CLIENT);
    lua_setfield(L,-2,"UDPC");

    return 1;
#else
	return 0;
#endif
}

//LUA_OS_MODULE(TMR, machine, machine_map);

#endif
