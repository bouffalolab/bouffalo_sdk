/*
** $Id: llib_mtimer.c $
** mtimer
*/

#define llib_dev_c
#define LUA_LIB

#ifdef DBG_TAG
#undef DBG_TAG
#endif
#define DBG_TAG "LMTIMER"

#include <stdint.h>
#include <string.h>
#include "lprefix.h"

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "bflb_mtimer.h"

static int mtimer_config(lua_State *L)
{
    return 1;
}

static int mtimer_delay_ms(lua_State *L)
{
    uint32_t time = luaL_checkinteger(L, 1);
    bflb_mtimer_delay_ms(time);
    return 1;
}

static int mtimer_get_time_ms(lua_State *L)
{
    uint32_t time = bflb_mtimer_get_time_ms();
    lua_pushinteger(L, time);
    return 1;
}

static int mtimer_delay_us(lua_State *L)
{
    uint32_t time = luaL_checkinteger(L, 1);
    bflb_mtimer_delay_us(time);
    return 1;
}

static int mtimer_get_time_us(lua_State *L)
{
    uint32_t time = bflb_mtimer_get_time_us();
    lua_pushinteger(L, time);
    return 1;
}

static const luaL_Reg l_mtimer_lib[] = {
    { "config", NULL },
    { "delay_ms", mtimer_delay_ms },
    { "delay_us", mtimer_delay_us },
    { "get_time_ms", mtimer_get_time_ms },
    { "get_time_us", mtimer_get_time_us },
    /* config placeholders */
    { NULL, NULL }
};

/* }====================================================== */

LUAMOD_API int luaopen_llib_mtimer(lua_State *L)
{
    /*!< TODO Assert */
    luaL_newlib(L, l_mtimer_lib);
    return 1;
}
