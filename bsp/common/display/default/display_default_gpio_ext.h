/**
 * @file display_default_gpio_ext.h
 * @brief Pin functions of the board that the default display needs (default/).
 *
 * The bus backends do not know pin numbers, and neither does the framework. The default display muxes the pins of its
 * interface through these functions, which boards declare in board_gpio.h and implement in board_gpio.c or
 * board_gpio_overlay.c (under CONFIG_BSP_DISPLAY). A project that defines its own displays hands its own functions to the static definition of
 * its buses (DBI: DISPLAY_DBI_IFACES(); SPI: the `func_init` / `func_deinit` fields) and does not need them.
 *
 * Rules every implementation follows:
 * - the `_gpio_init` functions mux only the clock, data and D/C pins of the interface;
 * - they never mux a CS pin to the controller: CS is a GPIO owned by the framework (software CS), so the CS pin of
 *   the interface stays free and can be any GPIO; the same holds for the D/C pin of the SPI interface;
 * - the `_cs_gpio_init` functions mux the CS pin of the interface to the controller, for a panel whose CS is driven
 *   by the hardware (an endpoint without software CS); call them together with the `_gpio_init` ones;
 * - the `_gpio_deinit` functions have nothing to do at the moment: when the display is released, the pins stay as
 *   they are.
 *
 * Every board of a chip that supports an interface implements the functions of that interface; a missing one is a
 * link error. A function that is not wired up yet is an empty one with a TODO comment.
 *
 * | Interface            | Functions                                               |
 * |----------------------|---------------------------------------------------------|
 * | DBI Type B (8080 x8) | board_display_dbi_b8_gpio_init / _deinit, _cs_ variants |
 * | DBI Type C 4-wire    | board_display_dbi_c4_gpio_init / _deinit, _cs_ variants |
 * | DBI Type C 3-wire    | board_display_dbi_c3_gpio_init / _deinit, _cs_ variants |
 * | DBI QSPI             | board_display_qspi_gpio_init / _deinit, _cs_ variants   |
 * | Hardware SPI         | board_display_spi_gpio_init / _deinit                   |
 * | BL618DG RGB (DPU)    | board_display_dpu_gpio_init / _deinit                   |
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef DISPLAY_DEFAULT_GPIO_EXT_H
#define DISPLAY_DEFAULT_GPIO_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup DISPLAY
 * @{
 */

/**
 * @defgroup DISPLAY_DEFAULT_GPIO_EXT Board pin functions of the default display
 * @{
 */

/** @brief Mux the pins of DBI Type B (8080 x8): WR, RD, D/C and D0..D7. */
void board_display_dbi_b8_gpio_init(void);
/** @brief Release the pins of DBI Type B. */
void board_display_dbi_b8_gpio_deinit(void);
/** @brief Mux the hardware CS pin of DBI Type B to the DBI controller. */
void board_display_dbi_b8_cs_gpio_init(void);
/** @brief Release the hardware CS pin of DBI Type B. */
void board_display_dbi_b8_cs_gpio_deinit(void);

/** @brief Mux the pins of DBI Type C 4-wire: SCL, SDA, D/C. */
void board_display_dbi_c4_gpio_init(void);
/** @brief Release the pins of DBI Type C 4-wire. */
void board_display_dbi_c4_gpio_deinit(void);
/** @brief Mux the hardware CS pin of DBI Type C 4-wire to the DBI controller. */
void board_display_dbi_c4_cs_gpio_init(void);
/** @brief Release the hardware CS pin of DBI Type C 4-wire. */
void board_display_dbi_c4_cs_gpio_deinit(void);

/** @brief Mux the pins of DBI Type C 3-wire: SCL, SDA. */
void board_display_dbi_c3_gpio_init(void);
/** @brief Release the pins of DBI Type C 3-wire. */
void board_display_dbi_c3_gpio_deinit(void);
/** @brief Mux the hardware CS pin of DBI Type C 3-wire to the DBI controller. */
void board_display_dbi_c3_cs_gpio_init(void);
/** @brief Release the hardware CS pin of DBI Type C 3-wire. */
void board_display_dbi_c3_cs_gpio_deinit(void);

/** @brief Mux the pins of DBI QSPI: SCL, SDA0..SDA3. */
void board_display_qspi_gpio_init(void);
/** @brief Release the pins of DBI QSPI. */
void board_display_qspi_gpio_deinit(void);
/** @brief Mux the hardware CS pin of DBI QSPI to the DBI controller. */
void board_display_qspi_cs_gpio_init(void);
/** @brief Release the hardware CS pin of DBI QSPI. */
void board_display_qspi_cs_gpio_deinit(void);

/** @brief Mux the pins of the hardware SPI of the display: SCK, MOSI. D/C and CS are software controlled. */
void board_display_spi_gpio_init(void);
/** @brief Release the pins of the hardware SPI of the display. */
void board_display_spi_gpio_deinit(void);

/** @brief Mux the RGB pins of the BL618DG display processing unit. */
void board_display_dpu_gpio_init(void);
/** @brief Release the RGB pins of the BL618DG display processing unit. */
void board_display_dpu_gpio_deinit(void);

/** @} */ /* DISPLAY_DEFAULT_GPIO_EXT */
/** @} */ /* DISPLAY */

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_DEFAULT_GPIO_EXT_H */
