
#include "myBoardDef.h"

#if LUA_USE_MACHINE

#include "lua.h"
#include "lauxlib.h"
#include "Lua_modules.h"

#include <unistd.h>

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

static const LUA_REG_TYPE machine_map[] = {
    { LSTRKEY( "history" ),			LFUNCVAL( os_history ) },
    { LSTRKEY( "version" ),			LFUNCVAL( os_version ) },
		{ LSTRKEY( "clclog" ),			LFUNCVAL( os_clear_history ) },
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
