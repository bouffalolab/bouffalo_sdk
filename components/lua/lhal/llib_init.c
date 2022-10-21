/*
** $Id: llib_init.c $
** Initialization of libraries for lua.c and other clients
*/

#define llib_init_c
#define LUA_LIB

/*
** If you embed Lua in your program and need to open the standard
** libraries, call luaL_openlibs in your program. If you need a
** different set of libraries, copy this file to your project and edit
** it to suit your needs.
**
** You can also *preload* libraries, so that a later 'require' can
** open the library, which is already linked to the application.
** For that, do the following code:
**
**  luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
**  lua_pushcfunction(L, luaopen_modname);
**  lua_setfield(L, -2, modname);
**  lua_pop(L, 1);  // remove PRELOAD table
*/

#include "lprefix.h"

#include <stddef.h>

#include "lua.h"

#include "llib_init.h"
#include "lauxlib.h"

static const luaL_Reg loadedlibs[] = {
    { LUA_LLIB_CORENAME, luaopen_llib_core },
    { LUA_LLIB_MTIMERNAME, luaopen_llib_mtimer },
    { LUA_LLIB_GPIONAME, luaopen_llib_gpio },
    { NULL, NULL }
};

LUALIB_API void luaL_preload_llibs(lua_State *L)
{
    const luaL_Reg *lib;

    for (lib = loadedlibs; lib->func; lib++) {
        luaL_getsubtable(L, LUA_REGISTRYINDEX, LUA_PRELOAD_TABLE);
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
        lua_pop(L, 1);
    }
}