/**
 * @file display_default_gram.c
 * @brief Default display, defined statically from Kconfig (default/Kconfig).
 *
 * The objects are chained bus -> endpoint (io) -> panel -> display, all allocated at build time.
 *
 * With the "Custom" module option, the project defines `const display_dcs_module_t display_default_custom_module`.
 * With CONFIG_DISPLAY_DEFAULT_BOARD_CUSTOM the project implements the panel power, reset and backlight
 * (`display_default_board_ops`), for example on an I/O expander or a PWM.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "default/display_default.h"
#include "default/display_default_board.h"
#include "default/display_default_gpio_ext.h"
#include "bflb_dma.h"
#include "default/display_default_module.h"
#if defined(CONFIG_DISPLAY_DEFAULT_IF_SPI)
#include "bus/spi/display_spi.h"
#else
#include "bus/dbi/display_dbi.h"
#endif

/* The panel module selected in Kconfig (DEFAULT_MODULE): display_default_module.h */

/* The panel uses a RAM copy of the (const) module, so that the application can change it before display_init():
 * display_default_get_module() */
static display_dcs_module_t module_ram;

/* Pixel format in memory */
#if defined(CONFIG_DISPLAY_DEFAULT_FORMAT_XRGB8888)
#define DEFAULT_FORMAT DISPLAY_FMT_XRGB8888
#elif defined(CONFIG_DISPLAY_DEFAULT_FORMAT_RGB888)
#define DEFAULT_FORMAT DISPLAY_FMT_RGB888
#else
#define DEFAULT_FORMAT DISPLAY_FMT_RGB565
#endif

/* Pixel format on the wire; unset options retain the endpoint's AUTO behavior. */
#if defined(CONFIG_DISPLAY_DEFAULT_WIRE_16)
#define DEFAULT_WIRE DISPLAY_WIRE_16
#elif defined(CONFIG_DISPLAY_DEFAULT_WIRE_24)
#define DEFAULT_WIRE DISPLAY_WIRE_24
#else
#define DEFAULT_WIRE DISPLAY_WIRE_AUTO
#endif

#if defined(CONFIG_DISPLAY_DEFAULT_FORMAT_RGB888) && defined(CONFIG_DISPLAY_DEFAULT_IF_SPI)
#error "packed RGB888 needs the DBI peripheral, the hardware SPI backend does not support it"
#endif

/* Interface and the board pin functions that go with it */
#if defined(CONFIG_DISPLAY_DEFAULT_IF_DBI_B8)
#define DEFAULT_IFACE       DISPLAY_IFACE_DBI_B8
#define DEFAULT_PINS_INIT   board_display_dbi_b8_gpio_init
#define DEFAULT_PINS_DEINIT board_display_dbi_b8_gpio_deinit
#elif defined(CONFIG_DISPLAY_DEFAULT_IF_DBI_C4)
#define DEFAULT_IFACE       DISPLAY_IFACE_DBI_C4
#define DEFAULT_PINS_INIT   board_display_dbi_c4_gpio_init
#define DEFAULT_PINS_DEINIT board_display_dbi_c4_gpio_deinit
#elif defined(CONFIG_DISPLAY_DEFAULT_IF_QSPI)
#define DEFAULT_IFACE       DISPLAY_IFACE_QSPI
#define DEFAULT_PINS_INIT   board_display_qspi_gpio_init
#define DEFAULT_PINS_DEINIT board_display_qspi_gpio_deinit
#else /* hardware SPI, also DBI Type C 4-wire */
#define DEFAULT_IFACE       DISPLAY_IFACE_DBI_C4
#define DEFAULT_PINS_INIT   board_display_spi_gpio_init
#define DEFAULT_PINS_DEINIT board_display_spi_gpio_deinit
#endif

/* Chip select: a software GPIO (the board pin functions never mux it), the DBI controller itself, or none */
#if defined(CONFIG_DISPLAY_DEFAULT_CS_GPIO)
#define DEFAULT_CS DISPLAY_PIN_LOW(CONFIG_DISPLAY_DEFAULT_CS_PIN)
#else
#define DEFAULT_CS DISPLAY_PIN_NONE
#endif

/* Hardware CS: the board additionally muxes the CS pin of the interface to the DBI controller, and the
 * bus gets wrapper functions that handle the interface pins and the CS pin together. */
#if defined(CONFIG_DISPLAY_DEFAULT_CS_HARDWARE)
#if defined(CONFIG_DISPLAY_DEFAULT_IF_DBI_B8)
#define DEFAULT_CS_INIT   board_display_dbi_b8_cs_gpio_init
#define DEFAULT_CS_DEINIT board_display_dbi_b8_cs_gpio_deinit
#elif defined(CONFIG_DISPLAY_DEFAULT_IF_DBI_C4)
#define DEFAULT_CS_INIT   board_display_dbi_c4_cs_gpio_init
#define DEFAULT_CS_DEINIT board_display_dbi_c4_cs_gpio_deinit
#elif defined(CONFIG_DISPLAY_DEFAULT_IF_QSPI)
#define DEFAULT_CS_INIT   board_display_qspi_cs_gpio_init
#define DEFAULT_CS_DEINIT board_display_qspi_cs_gpio_deinit
#else
#error "hardware CS needs the DBI peripheral"
#endif

/** @brief Mux the interface pins, then the CS pin. */
static void default_pins_init(void)
{
    DEFAULT_PINS_INIT();
    DEFAULT_CS_INIT();
}

/** @brief Release the CS pin, then the interface pins (reverse order of default_pins_init()). */
static void default_pins_deinit(void)
{
    DEFAULT_CS_DEINIT();
    DEFAULT_PINS_DEINIT();
}
#undef DEFAULT_PINS_INIT
#undef DEFAULT_PINS_DEINIT
#define DEFAULT_PINS_INIT   default_pins_init
#define DEFAULT_PINS_DEINIT default_pins_deinit
#endif

/* Bus: the board functions mux and release the pins; the LLI pool holds the DMA descriptors of one draw */
static struct bflb_dma_channel_lli_pool_s lli[CONFIG_DISPLAY_DEFAULT_LLI_COUNT];

#if defined(CONFIG_DISPLAY_DEFAULT_IF_SPI)
static display_spi_bus_t bus = DISPLAY_SPI_BUS(.dev = CONFIG_DISPLAY_DEFAULT_DEV, .dma = CONFIG_DISPLAY_DEFAULT_DMA,
                                               .lli = lli, .lli_count = CONFIG_DISPLAY_DEFAULT_LLI_COUNT,
                                               .dc = DISPLAY_PIN_HIGH(CONFIG_DISPLAY_DEFAULT_DC_PIN),
                                               .func_init = DEFAULT_PINS_INIT, .func_deinit = DEFAULT_PINS_DEINIT);
#else
/* The DBI bus carries one interface, selected by the endpoint (io.iface) */
static display_dbi_bus_t bus =
    DISPLAY_DBI_BUS(.dev = CONFIG_DISPLAY_DEFAULT_DEV, .dma = CONFIG_DISPLAY_DEFAULT_DMA, .lli = lli,
                    .lli_count = CONFIG_DISPLAY_DEFAULT_LLI_COUNT,
                    .ifaces = DISPLAY_DBI_IFACES({ DEFAULT_IFACE, DEFAULT_PINS_INIT, DEFAULT_PINS_DEINIT }));
#endif

/* Endpoint */
static display_io_t io = DISPLAY_IO(&bus, .cs = DEFAULT_CS, .iface = DEFAULT_IFACE,
                                    .freq_hz = CONFIG_DISPLAY_DEFAULT_FREQ_HZ, .format = DEFAULT_FORMAT,
                                    .wire = DEFAULT_WIRE);

/* Panel: power, reset and backlight, see display_default_board.h */
static display_dcs_panel_t panel = DISPLAY_DCS_PANEL(&io, &module_ram, DEFAULT_PANEL_BOARD);

/* Display: GRAM panel, pixels are pushed with display_draw() */
static display_t display = DISPLAY_GRAM(&panel);

/** @brief Copy the module to RAM on first use. */
static void default_module_init(void)
{
    static bool copied;

    /* Not reentrant: call it from the application init, before display_init() */
    if (!copied) {
        module_ram = *DEFAULT_MODULE;
        copied = true;
    }
}

/** @brief See display_default.h. */
display_t *display_default_get(void)
{
    default_module_init();
    return &display;
}

/** @brief See display_default.h. */
display_dcs_module_t *display_default_get_module(void)
{
    default_module_init();
    return &module_ram;
}
