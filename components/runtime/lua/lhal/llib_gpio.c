/*
** $Id: llib_gpio.c $
** gpio
*/

#define llib_gpio_c
#define LUA_LIB

#ifdef DBG_TAG
#undef DBG_TAG
#endif
#define DBG_TAG "LGPIO"

#include <stdint.h>
#include "lprefix.h"

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"

#include "bflb_gpio.h"

static struct bflb_device_s *gpio;

#define GPIO_INPUT_N     "IN"
#define GPIO_OUTPUT_N    "OUT"
#define GPIO_ANALOG_N    "ANA"
#define GPIO_ALTERNATE_N "ALT"
#define GPIO_PULLUP_N    "PU"
#define GPIO_PULLDOWN_N  "PD"
#define GPIO_FLOAT_N     "PN"
#define GPIO_SMT_DIS_N   "SMT0"
#define GPIO_SMT_EN_N    "SMT1"
#define GPIO_DRV_0_N     "DRV0"
#define GPIO_DRV_1_N     "DRV1"
#define GPIO_DRV_2_N     "DRV2"
#define GPIO_DRV_3_N     "DRV3"

#if defined(BL702)
#error "LUA GPIO LIB not support BL702"
#elif defined(BL616)
#define GPIO_FUNC_SDH_N   "F_SDH"
#define GPIO_FUNC_SPI0_N  "F_SPI0"
#define GPIO_FUNC_I2S_N   "F_I2S"
#define GPIO_FUNC_PDM_N   "F_PDM"
#define GPIO_FUNC_I2C0_N  "F_I2C0"
#define GPIO_FUNC_I2C1_N  "F_I2C1"
#define GPIO_FUNC_EMAC_N  "F_EMAC"
#define GPIO_FUNC_CAM_N   "F_CAM"
#define GPIO_FUNC_SDU_N   "F_SDU"
#define GPIO_FUNC_PWM0_N  "F_PWM0"
#elif defined(BL628)
#define GPIO_FUNC_SDH_N  "F_SDH"
#define GPIO_FUNC_SPI0_N "F_SPI0"
#define GPIO_FUNC_I2S_N  "F_I2S"
#define GPIO_FUNC_PDM_N  "F_PDM"
#define GPIO_FUNC_I2C0_N "F_I2C0"
#define GPIO_FUNC_I2C1_N "F_I2C1"
#define GPIO_FUNC_UART_N "F_UART"
#define GPIO_FUNC_EMAC_N "F_EMAC"
#define GPIO_FUNC_CAM_N  "F_CAM"
#define GPIO_FUNC_SDU_N  "F_SDU"
#define GPIO_FUNC_PWM0_N "F_PWM0"
#define GPIO_FUNC_DBIB_N "F_DBIB"
#define GPIO_FUNC_DBIC_N "F_DBIC"
#define GPIO_FUNC_PEC_N  "F_PEC"
#endif

#define GPIO_UART_FUNC_UART0_RTS_N "UF_UART0_RTS"
#define GPIO_UART_FUNC_UART0_CTS_N "UF_UART0_CTS"
#define GPIO_UART_FUNC_UART0_TX_N  "UF_UART0_TX"
#define GPIO_UART_FUNC_UART0_RX_N  "UF_UART0_RX"
#define GPIO_UART_FUNC_UART1_RTS_N "UF_UART1_RTS"
#define GPIO_UART_FUNC_UART1_CTS_N "UF_UART1_CTS"
#define GPIO_UART_FUNC_UART1_TX_N  "UF_UART1_TX"
#define GPIO_UART_FUNC_UART1_RX_N  "UF_UART1_RX"
#if defined(BL808) || defined(BL606P)
#define GPIO_UART_FUNC_UART2_RTS_N "UF_UART2_RTS"
#define GPIO_UART_FUNC_UART2_CTS_N "UF_UART2_CTS"
#define GPIO_UART_FUNC_UART2_TX_N  "UF_UART2_TX"
#define GPIO_UART_FUNC_UART2_RX_N  "UF_UART2_RX"
#endif

static int gpio_init(lua_State *L)
{
    uint32_t pin = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t cfg = (uint32_t)luaL_checkinteger(L, 3);
    bflb_gpio_init(gpio, pin, cfg);
    return 1;
}

static int gpio_deinit(lua_State *L)
{
    uint32_t pin = (uint32_t)luaL_checkinteger(L, 2);
    bflb_gpio_deinit(gpio, pin);
    return 1;
}

static int gpio_set(lua_State *L)
{
    uint32_t pin = (uint32_t)luaL_checkinteger(L, 2);
    bflb_gpio_set(gpio, pin);
    return 1;
}

static int gpio_reset(lua_State *L)
{
    uint32_t pin = (uint32_t)luaL_checkinteger(L, 2);
    bflb_gpio_reset(gpio, pin);
    return 1;
}

static int gpio_read(lua_State *L)
{
    uint32_t pin = (uint32_t)luaL_checkinteger(L, 2);
    bool value = bflb_gpio_read(gpio, pin);
    lua_pushboolean(L, value);
    return 1;
}

static int gpio_int_init(lua_State *L)
{
    return 1;
}

static int gpio_int_mask(lua_State *L)
{
    return 1;
}

static int gpio_get_intstatus(lua_State *L)
{
    return 1;
}

static int gpio_int_clear(lua_State *L)
{
    return 1;
}

static int gpio_uart_init(lua_State *L)
{
    uint32_t pin = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t uart_func = (uint32_t)luaL_checkinteger(L, 3);
    bflb_gpio_uart_init(gpio, pin, uart_func);
    return 1;
}

static const luaL_Reg l_gpio_lib[] = {
    { "init", gpio_init },
    { "deinit", gpio_deinit },
    { "set", gpio_set },
    { "reset", gpio_reset },
    { "read", gpio_read },
    { "int_init", gpio_int_init },
    { "int_mask", gpio_int_mask },
    { "get_intstatus", gpio_get_intstatus },
    { "int_clear", gpio_int_clear },
    { "uart_init", gpio_uart_init },
    { "feature_control", NULL },
    /* config placeholders */
    { GPIO_INPUT_N, NULL },
    { GPIO_OUTPUT_N, NULL },
    { GPIO_ANALOG_N, NULL },
    { GPIO_ALTERNATE_N, NULL },
    { GPIO_PULLUP_N, NULL },
    { GPIO_PULLDOWN_N, NULL },
    { GPIO_FLOAT_N, NULL },
    { GPIO_SMT_DIS_N, NULL },
    { GPIO_SMT_EN_N, NULL },
    { GPIO_DRV_0_N, NULL },
    { GPIO_DRV_1_N, NULL },
    { GPIO_DRV_2_N, NULL },
    { GPIO_DRV_3_N, NULL },
#if defined(BL616)
    { GPIO_FUNC_SDH_N, NULL },
    { GPIO_FUNC_SPI0_N, NULL },
    { GPIO_FUNC_I2S_N, NULL },
    { GPIO_FUNC_PDM_N, NULL },
    { GPIO_FUNC_I2C0_N, NULL },
    { GPIO_FUNC_I2C1_N, NULL },
    { GPIO_FUNC_EMAC_N, NULL },
    { GPIO_FUNC_CAM_N, NULL },
    { GPIO_FUNC_SDU_N, NULL },
    { GPIO_FUNC_PWM0_N, NULL },
#elif defined(BL628)
    { GPIO_FUNC_SDH_N, NULL },
    { GPIO_FUNC_SPI0_N, NULL },
    { GPIO_FUNC_I2S_N, NULL },
    { GPIO_FUNC_PDM_N, NULL },
    { GPIO_FUNC_I2C0_N, NULL },
    { GPIO_FUNC_I2C1_N, NULL },
    { GPIO_FUNC_UART_N, NULL },
    { GPIO_FUNC_EMAC_N, NULL },
    { GPIO_FUNC_CAM_N, NULL },
    { GPIO_FUNC_SDU_N, NULL },
    { GPIO_FUNC_PWM0_N, NULL },
    { GPIO_FUNC_DBIB_N, NULL },
    { GPIO_FUNC_DBIC_N, NULL },
    { GPIO_FUNC_PEC_N, NULL },
#endif

    { NULL, NULL }
};

/* }====================================================== */

LUAMOD_API int luaopen_llib_gpio(lua_State *L)
{
    gpio = bflb_device_get_by_name("gpio");
    /*!< TODO Assert */

    luaL_newlib(L, l_gpio_lib);
    lua_pushinteger(L, GPIO_INPUT);
    lua_setfield(L, -2, GPIO_INPUT_N);
    lua_pushinteger(L, GPIO_OUTPUT);
    lua_setfield(L, -2, GPIO_OUTPUT_N);
    lua_pushinteger(L, GPIO_ANALOG);
    lua_setfield(L, -2, GPIO_ANALOG_N);
    lua_pushinteger(L, GPIO_ALTERNATE);
    lua_setfield(L, -2, GPIO_ALTERNATE_N);
    lua_pushinteger(L, GPIO_PULLUP);
    lua_setfield(L, -2, GPIO_PULLUP_N);
    lua_pushinteger(L, GPIO_PULLDOWN);
    lua_setfield(L, -2, GPIO_PULLDOWN_N);
    lua_pushinteger(L, GPIO_FLOAT);
    lua_setfield(L, -2, GPIO_FLOAT_N);
    lua_pushinteger(L, GPIO_SMT_DIS);
    lua_setfield(L, -2, GPIO_SMT_DIS_N);
    lua_pushinteger(L, GPIO_SMT_EN);
    lua_setfield(L, -2, GPIO_SMT_EN_N);
    lua_pushinteger(L, GPIO_DRV_0);
    lua_setfield(L, -2, GPIO_DRV_0_N);
    lua_pushinteger(L, GPIO_DRV_1);
    lua_setfield(L, -2, GPIO_DRV_1_N);
    lua_pushinteger(L, GPIO_DRV_2);
    lua_setfield(L, -2, GPIO_DRV_2_N);
    lua_pushinteger(L, GPIO_DRV_3);
    lua_setfield(L, -2, GPIO_DRV_3_N);

#if defined(BL616)
    lua_pushinteger(L, GPIO_FUNC_SDH);
    lua_setfield(L, -2, GPIO_FUNC_SDH_N);
    lua_pushinteger(L, GPIO_FUNC_SPI0);
    lua_setfield(L, -2, GPIO_FUNC_SPI0_N);
    lua_pushinteger(L, GPIO_FUNC_I2S);
    lua_setfield(L, -2, GPIO_FUNC_I2S_N);
    lua_pushinteger(L, GPIO_FUNC_PDM);
    lua_setfield(L, -2, GPIO_FUNC_PDM_N);
    lua_pushinteger(L, GPIO_FUNC_I2C0);
    lua_setfield(L, -2, GPIO_FUNC_I2C0_N);
    lua_pushinteger(L, GPIO_FUNC_I2C1);
    lua_setfield(L, -2, GPIO_FUNC_I2C1_N);
    lua_pushinteger(L, GPIO_FUNC_EMAC);
    lua_setfield(L, -2, GPIO_FUNC_EMAC_N);
    lua_pushinteger(L, GPIO_FUNC_CAM);
    lua_setfield(L, -2, GPIO_FUNC_CAM_N);
    lua_pushinteger(L, GPIO_FUNC_SDU);
    lua_setfield(L, -2, GPIO_FUNC_SDU_N);
    lua_pushinteger(L, GPIO_FUNC_PWM0);
    lua_setfield(L, -2, GPIO_FUNC_PWM0_N);
#elif defined(BL628)
    lua_pushinteger(L, GPIO_FUNC_SDH);
    lua_setfield(L, -2, GPIO_FUNC_SDH_N);
    lua_pushinteger(L, GPIO_FUNC_SPI0);
    lua_setfield(L, -2, GPIO_FUNC_SPI0_N);
    lua_pushinteger(L, GPIO_FUNC_I2S);
    lua_setfield(L, -2, GPIO_FUNC_I2S_N);
    lua_pushinteger(L, GPIO_FUNC_PDM);
    lua_setfield(L, -2, GPIO_FUNC_PDM_N);
    lua_pushinteger(L, GPIO_FUNC_I2C0);
    lua_setfield(L, -2, GPIO_FUNC_I2C0_N);
    lua_pushinteger(L, GPIO_FUNC_I2C1);
    lua_setfield(L, -2, GPIO_FUNC_I2C1_N);
    lua_pushinteger(L, GPIO_FUNC_UART);
    lua_setfield(L, -2, GPIO_FUNC_UART_N);
    lua_pushinteger(L, GPIO_FUNC_EMAC);
    lua_setfield(L, -2, GPIO_FUNC_EMAC_N);
    lua_pushinteger(L, GPIO_FUNC_CAM);
    lua_setfield(L, -2, GPIO_FUNC_CAM_N);
    lua_pushinteger(L, GPIO_FUNC_SDU);
    lua_setfield(L, -2, GPIO_FUNC_SDU_N);
    lua_pushinteger(L, GPIO_FUNC_PWM0);
    lua_setfield(L, -2, GPIO_FUNC_PWM0_N);
    lua_pushinteger(L, GPIO_FUNC_DBI_B);
    lua_setfield(L, -2, GPIO_FUNC_DBI_B_N);
    lua_pushinteger(L, GPIO_FUNC_DBI_C);
    lua_setfield(L, -2, GPIO_FUNC_DBI_C_N);
    lua_pushinteger(L, GPIO_FUNC_PEC);
    lua_setfield(L, -2, GPIO_FUNC_PEC_N);
#endif

    lua_pushinteger(L, GPIO_UART_FUNC_UART0_RTS);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART0_RTS_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART0_CTS);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART0_CTS_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART0_TX);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART0_TX_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART0_RX);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART0_RX_N);

    lua_pushinteger(L, GPIO_UART_FUNC_UART1_RTS);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART1_RTS_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART1_CTS);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART1_CTS_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART1_TX);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART1_TX_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART1_RX);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART1_RX_N);

#if defined(BL808) || defined(BL606P)
    lua_pushinteger(L, GPIO_UART_FUNC_UART2_RTS);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART2_RTS_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART2_CTS);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART2_CTS_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART2_TX);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART2_TX_N);
    lua_pushinteger(L, GPIO_UART_FUNC_UART2_RX);
    lua_setfield(L, -2, GPIO_UART_FUNC_UART2_RX_N);
#endif

    return 1;
}
