
#include <stddef.h>

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"

#include "Lua_modules.h"
#include "lgc.h"

/*
** these libs are loaded by lua.c and are readily available to any Lua
** program
*/
static const luaL_Reg loaded_hardware_libs[] = {	
	//{LUA_THREADLIBNAME, luaopen_thread },//add by lcj
  LINIT_REG_THREAD //add by lcj
	LINIT_REG_PIO//add by lcj
	LINIT_REG_TMR//add by lcj
	LINIT_REG_MACHINE //add by lcj
	
  {NULL, NULL}
};


LUALIB_API void luaL_openHardwarelibs (lua_State *L) {
  const luaL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = loaded_hardware_libs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
		
		luaC_fullgc(L, 1);//add force mem free
  }
}
