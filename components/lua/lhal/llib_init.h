/*
** $Id: llib_init.h $
** Lua standard libraries
*/

#ifndef llib_init_h
#define llib_init_h

#include "lua.h"

LUALIB_API void luaL_preload_llibs(lua_State *L);

#define LUA_LLIB_CORENAME "core"
LUAMOD_API int(luaopen_llib_core)(lua_State *L);

#define LUA_LLIB_MTIMERNAME "mtimer"
LUAMOD_API int(luaopen_llib_mtimer)(lua_State *L);

#define LUA_LLIB_GPIONAME "gpio"
LUAMOD_API int(luaopen_llib_gpio)(lua_State *L);

#endif
