/*
** $Id: luaport.c $
** Tag methods
** See Copyright Notice in lua.h
*/

#define luaport_c
#define LUA_PORT

#include <stdint.h>
#include <stdio.h>

extern void lua_end(void);

void luaport_abort(void)
{
    lua_end();
}

void luaport_exit(int status)
{
    lua_end();
}

char *luaport_getenv(const char *name)
{
    return NULL;
}