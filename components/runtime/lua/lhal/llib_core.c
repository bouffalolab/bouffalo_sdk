/*
** $Id: llib_core.c $
** core
*/

#define llib_dev_c
#define LUA_LIB

#ifdef DBG_TAG
#undef DBG_TAG
#endif
#define DBG_TAG "LCORE"

#include <stdint.h>
#include <string.h>
#include "lprefix.h"

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "bflb_core.h"

#define BFLB_DEVICE_TYPE_ADC_N      "ADC"
#define BFLB_DEVICE_TYPE_DAC_N      "DAC"
#define BFLB_DEVICE_TYPE_AUDIOADC_N "AUDIOADC"
#define BFLB_DEVICE_TYPE_AUDIODAC_N "AUDIODAC"
#define BFLB_DEVICE_TYPE_GPIO_N     "GPIO"
#define BFLB_DEVICE_TYPE_UART_N     "UART"
#define BFLB_DEVICE_TYPE_SPI_N      "SPI"
#define BFLB_DEVICE_TYPE_I2C_N      "I2C"
#define BFLB_DEVICE_TYPE_DMA_N      "DMA"
#define BFLB_DEVICE_TYPE_I2S_N      "I2S"
#define BFLB_DEVICE_TYPE_IR_N       "IR"
#define BFLB_DEVICE_TYPE_TIMER_N    "TIMER"
#define BFLB_DEVICE_TYPE_PWM_N      "PWM"
#define BFLB_DEVICE_TYPE_ISO11898_N "ISO11898"
#define BFLB_DEVICE_TYPE_CAMERA_N   "CAMERA"
#define BFLB_DEVICE_TYPE_FLASH_N    "FLASH"
#define BFLB_DEVICE_TYPE_QSPI_N     "QSPI"
#define BFLB_DEVICE_TYPE_SDH_N      "SDH"
#define BFLB_DEVICE_TYPE_SDU_N      "SDU"
#define BFLB_DEVICE_TYPE_ETH_N      "ETH"
#define BFLB_DEVICE_TYPE_RTC_N      "RTC"
#define BFLB_DEVICE_TYPE_CRC_N      "CRC"
#define BFLB_DEVICE_TYPE_RNG_N      "RNG"
#define BFLB_DEVICE_TYPE_MIPI_N     "MIPI"
#define BFLB_DEVICE_TYPE_DPI_N      "DPI"
#define BFLB_DEVICE_TYPE_DSI_N      "DSI"
#define BFLB_DEVICE_TYPE_CSI_N      "CSI"
#define BFLB_DEVICE_TYPE_USB_N      "USB"
#define BFLB_DEVICE_TYPE_AES_N      "AES"
#define BFLB_DEVICE_TYPE_SHA_N      "SHA"
#define BFLB_DEVICE_TYPE_MD5_N      "MD5"
#define BFLB_DEVICE_TYPE_TRNG_N     "TRNG"

static int __get_member(lua_State *L)
{
    struct bflb_device_s *dev = (struct bflb_device_s *)lua_touserdata(L, 1);
    const char *member = luaL_checkstring(L, 2);

    if (dev == NULL) {
        lua_pushnil(L);
        return 1;
    }

    if (strncmp(member, "name", sizeof("name")) == 0) {
        lua_pushstring(L, dev->name);
    } else if (strncmp(member, "reg_base", sizeof("reg_base")) == 0) {
        lua_pushinteger(L, dev->reg_base);
    } else if (strncmp(member, "irq_num", sizeof("irq_num")) == 0) {
        lua_pushinteger(L, dev->irq_num);
    } else if (strncmp(member, "idx", sizeof("idx")) == 0) {
        lua_pushinteger(L, dev->idx);
    } else if (strncmp(member, "sub_idx", sizeof("sub_idx")) == 0) {
        lua_pushinteger(L, dev->sub_idx);
    } else if (strncmp(member, "dev_type", sizeof("dev_type")) == 0) {
        lua_pushinteger(L, dev->dev_type);
    } else if (strncmp(member, "user_data", sizeof("user_data")) == 0) {
        lua_pushlightuserdata(L, dev->user_data);
    } else {
        luaL_argerror(L, 1, "undefined member");
    }

    return 1;
}

static int device_get(lua_State *L)
{
    void *dev = NULL;

    if (lua_type(L, 1) == LUA_TSTRING) {
        const char *s = luaL_checkstring(L, 1);
        dev = bflb_device_get_by_name(s);
    } else if (lua_type(L, 1) == LUA_TNUMBER) {
        uint32_t type = luaL_checkinteger(L, 1);
        uint32_t idx = luaL_checkinteger(L, 2);
        dev = bflb_device_get_by_id(type, idx);
    } else {
        luaL_argerror(L, 1, "need device name (string) or device index (integer)");
    }

    if (dev == NULL) {
        lua_pushnil(L);
    } else {
        lua_pushlightuserdata(L, dev);
        /*!< TODO 成员获取 */
    }

    return 1;
}


static const luaL_Reg l_core_lib[] = {
    { "get", device_get },
    /* config placeholders */
    { BFLB_DEVICE_TYPE_ADC_N, NULL },
    { BFLB_DEVICE_TYPE_DAC_N, NULL },
    { BFLB_DEVICE_TYPE_AUDIOADC_N, NULL },
    { BFLB_DEVICE_TYPE_AUDIODAC_N, NULL },
    { BFLB_DEVICE_TYPE_GPIO_N, NULL },
    { BFLB_DEVICE_TYPE_UART_N, NULL },
    { BFLB_DEVICE_TYPE_SPI_N, NULL },
    { BFLB_DEVICE_TYPE_I2C_N, NULL },
    { BFLB_DEVICE_TYPE_DMA_N, NULL },
    { BFLB_DEVICE_TYPE_I2S_N, NULL },
    { BFLB_DEVICE_TYPE_IR_N, NULL },
    { BFLB_DEVICE_TYPE_TIMER_N, NULL },
    { BFLB_DEVICE_TYPE_PWM_N, NULL },
    { BFLB_DEVICE_TYPE_ISO11898_N, NULL },
    { BFLB_DEVICE_TYPE_CAMERA_N, NULL },
    { BFLB_DEVICE_TYPE_FLASH_N, NULL },
    { BFLB_DEVICE_TYPE_QSPI_N, NULL },
    { BFLB_DEVICE_TYPE_SDH_N, NULL },
    { BFLB_DEVICE_TYPE_SDU_N, NULL },
    { BFLB_DEVICE_TYPE_ETH_N, NULL },
    { BFLB_DEVICE_TYPE_RTC_N, NULL },
    { BFLB_DEVICE_TYPE_CRC_N, NULL },
    { BFLB_DEVICE_TYPE_RNG_N, NULL },
    { BFLB_DEVICE_TYPE_MIPI_N, NULL },
    { BFLB_DEVICE_TYPE_DPI_N, NULL },
    { BFLB_DEVICE_TYPE_DSI_N, NULL },
    { BFLB_DEVICE_TYPE_CSI_N, NULL },
    { BFLB_DEVICE_TYPE_USB_N, NULL },
    { BFLB_DEVICE_TYPE_AES_N, NULL },
    { BFLB_DEVICE_TYPE_SHA_N, NULL },
    { BFLB_DEVICE_TYPE_MD5_N, NULL },
    { BFLB_DEVICE_TYPE_TRNG_N, NULL },
    { NULL, NULL }
};

static const luaL_Reg metameth[] = {
    { "__index", __get_member },
    { "__tostring", NULL },
    { NULL, NULL }
};

static void createmeta(lua_State *L)
{
    luaL_newmetatable(L, "device*");
    luaL_setfuncs(L, metameth, 0);
    lua_pop(L, 1);
}

/* }====================================================== */

LUAMOD_API int luaopen_llib_core(lua_State *L)
{
    /*!< TODO Assert */
    luaL_newlib(L, l_core_lib);

    createmeta(L);

    lua_pushinteger(L, BFLB_DEVICE_TYPE_ADC);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_ADC_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_DAC);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_DAC_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_AUDIOADC);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_AUDIOADC_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_AUDIODAC);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_AUDIODAC_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_GPIO);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_GPIO_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_UART);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_UART_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_SPI);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_SPI_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_I2C);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_I2C_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_DMA);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_DMA_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_I2S);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_I2S_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_IR);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_IR_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_TIMER);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_TIMER_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_PWM);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_PWM_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_ISO11898);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_ISO11898_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_CAMERA);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_CAMERA_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_FLASH);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_FLASH_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_QSPI);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_QSPI_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_SDH);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_SDH_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_SDU);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_SDU_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_ETH);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_ETH_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_RTC);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_RTC_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_CRC);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_CRC_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_RNG);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_RNG_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_MIPI);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_MIPI_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_DPI);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_DPI_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_DSI);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_DSI_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_CSI);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_CSI_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_USB);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_USB_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_AES);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_AES_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_SHA);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_SHA_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_MD5);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_MD5_N);
    lua_pushinteger(L, BFLB_DEVICE_TYPE_TRNG);
    lua_setfield(L, -2, BFLB_DEVICE_TYPE_TRNG_N);

    return 1;
}
