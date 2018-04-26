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
#include "rw_app.h"

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

static int lwifi_setMode(lua_State *L)
{
	int mode;
	mode=luaL_checkinteger(L,1);
	
	  if(Lua_wifi_state.net_on < 0)
			return luaL_error(L, "wifi is initialized!");
		else{
		  Lua_wifi_state.net_mode = mode;
		}
	
    return 0;
}

static int lwifi_setProtocol(lua_State *L)
{
	int protocol;
	protocol=luaL_checkinteger(L,1);
	
	  if(Lua_wifi_state.net_on < 0)
			return luaL_error(L, "wifi is initialized!");
		else{
			Lua_wifi_state.net_protocol = protocol;
		}
	
    return 0;
}

static int lwifi_setParams(lua_State *L)
{
	const char *ssid = luaL_checkstring(L, 1);
	const char *pass = luaL_checkstring(L, 1);
	
	  if(Lua_wifi_state.net_on < 0)
			return luaL_error(L, "wifi is initialized!");
		else{
			if(Lua_wifi_state.net_mode == NET_MODE_AP){
			  Wifi_AP_SSID = (char*)ssid;
			  Wifi_AP_PASSWORD = (char*)pass;
			}
			else if(Lua_wifi_state.net_mode == NET_MODE_STA){
			  Wifi_STA_SSID = (char*)ssid;
			  Wifi_STA_PASSWORD = (char*)pass;
			}
		}
	
    return 0;
}

static int lwifi_setAddr(lua_State *L)
{
	int port,ip;
	bool ip_set = false;
	port=luaL_checkinteger(L,1);
	
	if (lua_gettop(L) == 2){
		ip=luaL_checkinteger(L,2);
		ip_set = true;
	}
	
	  if(Lua_wifi_state.net_on < 0)
			return luaL_error(L, "wifi is initialized!");
		else{
			if(Lua_wifi_state.net_protocol == NET_TCP_SERVER){
			 Wifi_TCPS_SERVER_PORT = (uint16_t)port;
				
			}
			else if(Lua_wifi_state.net_protocol == NET_TCP_CLIENT){
				Wifi_TCPC_SERVER_PORT = (uint16_t)port;
        if(ip_set){
				  Wifi_TCPC_SERVER_IP = ip;
				}
			}
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
		{ LSTRKEY( "setmode"),			  LFUNCVAL( lwifi_setMode ) },
		{ LSTRKEY( "setsocket"),			  LFUNCVAL( lwifi_setProtocol ) },
		{ LSTRKEY( "setaddr"),			  LFUNCVAL( lwifi_setAddr ) },
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
