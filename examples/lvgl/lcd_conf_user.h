#ifndef _LCD_CONF_USER_H_
#define _LCD_CONF_USER_H_

/* clang-format off */

/* Select screen Type, Optional:

  mipi dbi interface
    LCD_DBI_ILI9488
    LCD_DBI_ILI9486
    LCD_DBI_ILI9341
    LCD_DBI_NT35510
    LCD_DBI_ST7796

  mipi dpi (RGB) interface
    LCD_DPI_ILI9488
    LCD_DPI_GC9503V
    LCD_DPI_STANDARD

  mipi dsi vidio interface
    LCD_DSI_VIDIO_ILI9881C

  spi interface
    LCD_SPI_ILI9488
    LCD_SPI_ILI9341
    LCD_SPI_ST7796
    LCD_SPI_ST7789V
*/
#define LCD_SPI_ILI9341

/* dbi ili9488 config */
#if defined LCD_DBI_ILI9488

    /* Selecting interface type, more configuration of peripherals comes later
        1: DBI peripheral, supported functions: typeC-3wire, typeC-4wire, typeB-x8(8080); (support chips: bl616, bl606p, bl808),
    */
    #define LCD_DBI_INTERFACE_TYPE 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* Selecting pixel format
        1: rgb565 (16-bit, output rgb565)
        2: nrgb8888 (32-bit, output rgb888)
    */
    #define ILI9488_DBI_PIXEL_FORMAT 1

    /* ILI9488 LCD width and height */
    #define ILI9488_DBI_W 320
    #define ILI9488_DBI_H 480

    /* The offset of the area can be displayed */
    #define ILI9488_DBI_OFFSET_X 0
    #define ILI9488_DBI_OFFSET_Y 0

    /* Color reversal, Some screens are required
        0: disable
        1: enable
    */
   #define ILI9488_DBI_COLOR_REVERSAL 0

#elif defined LCD_DBI_ILI9486

    /* Selecting interface type, more configuration of peripherals comes later
        1: DBI peripheral, supported functions: typeC-3wire, typeC-4wire, typeB-x8(8080); (support chips: bl616, bl606p, bl808),
    */
    #define LCD_DBI_INTERFACE_TYPE 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* Selecting pixel format
        1: rgb565 (16-bit, output rgb565)
        2: nrgb8888 (32-bit, output rgb888)
    */
    #define ILI9486_DBI_PIXEL_FORMAT 2

    /* ILI9486 LCD width and height */
    #define ILI9486_DBI_W 320
    #define ILI9486_DBI_H 480

    /* The offset of the area can be displayed */
    #define ILI9486_DBI_OFFSET_X 0
    #define ILI9486_DBI_OFFSET_Y 0

    /* Color reversal, Some screens are required
        0: disable
        1: enable
    */
   #define ILI9486_DBI_COLOR_REVERSAL 0

/* dbi ili9341 config */
#elif defined LCD_DBI_ILI9341

    /* Selecting interface type, more configuration of peripherals comes later
        1: DBI peripheral, supported functions: typeC-3wire, typeC-4wire, typeB-x8(8080); (support chips: bl616, bl606p, bl808),
    */
    #define LCD_DBI_INTERFACE_TYPE 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* Selecting pixel format
        1: rgb565 (16-bit, output rgb565)
        2: nrgb8888 (32-bit, output rgb888)
    */
    #define ILI9341_DBI_PIXEL_FORMAT 2

    /* ILI9341 LCD width and height */
    #define ILI9341_DBI_W 240
    #define ILI9341_DBI_H 320

    /* The offset of the area can be displayed */
    #define ILI9341_DBI_OFFSET_X 0
    #define ILI9341_DBI_OFFSET_Y 0

    /* Color reversal, Some screens are required
        0: disable
        1: enable
    */
   #define ILI9341_DBI_COLOR_REVERSAL 0

   /* dbi ili9341 config */
#elif defined LCD_DBI_NT35510

    /* Selecting interface type, more configuration of peripherals comes later
        1: DBI peripheral, supported functions: typeB-x8(8080); (support chips: bl616, bl606p, bl808),
    */
    #define LCD_DBI_INTERFACE_TYPE 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* Selecting pixel format
        1: rgb565 (16-bit, output rgb565)
        2: nrgb8888 (32-bit, output rgb888)
    */
    #define NT35510_DBI_PIXEL_FORMAT 1

    /* NT35510 LCD width and height */
    #define NT35510_DBI_W 480
    #define NT35510_DBI_H 800

    /* The offset of the area can be displayed */
    #define NT35510_DBI_OFFSET_X 0
    #define NT35510_DBI_OFFSET_Y 0

    /* Color reversal, Some screens are required
        0: disable
        1: enable
    */
   #define NT35510_DBI_COLOR_REVERSAL 0

   /* dbi st7796 config */
#elif defined LCD_DBI_ST7796

    /* Selecting interface type, more configuration of peripherals comes later
        1: DBI peripheral, supported functions: typeB-x8(8080); (support chips: bl616, bl606p, bl808),
    */
    #define LCD_DBI_INTERFACE_TYPE 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* Selecting pixel format
        1: rgb565 (16-bit, output rgb565)
        2: nrgb8888 (32-bit, output rgb888)
    */
    #define ST7796_DBI_PIXEL_FORMAT 1

    /* ST7796 LCD width and height */
    #define ST7796_DBI_W 320
    #define ST7796_DBI_H 480

    /* The offset of the area can be displayed */
    #define ST7796_DBI_OFFSET_X 0
    #define ST7796_DBI_OFFSET_Y 0

    /* Color reversal, Some screens are required
        0: disable
        1: enable
    */
   #define ST7796_DBI_COLOR_REVERSAL 0

/* dpi gc9503v config */
#elif defined LCD_DPI_GC9503V

    /* Selecting DPI working mode
        1: DPI peripheral (support: bl808)
    */
    #define GC9503V_DPI_MODE 1

    /* Selecting initialization interface
        1: Software spi 9-bit mode
    */
    #define GC9503V_DPI_INIT_INTERFACE 1

    #if(GC9503V_DPI_INIT_INTERFACE == 1)
        #define GC9503V_DPI_SPI_CS_PIN   GPIO_PIN_12
        #define GC9503V_DPI_SPI_CLK_PIN  GPIO_PIN_24
        #define GC9503V_DPI_SPI_DAT_PIN  GPIO_PIN_25
    #endif

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* Selecting pixel format
        1: rgb565 (16-bits)
    */
    #define GC9503V_DPI_PIXEL_FORMAT 1

    /* ILI9488 LCD width and height */
    #define GC9503V_DPI_W 480
    #define GC9503V_DPI_H 480

/* dpi ili9488 config */
#elif defined LCD_DPI_ILI9488

    /* Selecting DPI working mode
        1: DPI peripheral (support: bl808)
    */
    #define ILI9488_DPI_MODE 1

    /* Selecting initialization interface
        1: Hardware spi 8-bit mode
        2: Hardware dbi peripheral typeC (support: bl616, bl606p, bl808)
    */
    #define ILI9488_DPI_INIT_INTERFACE 1

    #if(ILI9488_DPI_INIT_INTERFACE == 1)
        #define ILI9488_DPI_SPI_CS_PIN  GPIO_PIN_20
        #define ILI9488_DPI_SPI_DC_PIN  GPIO_PIN_3
    #endif

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* Selecting pixel format
        1: rgb565 (16-bits)
    */
    #define ILI9488_DPI_PIXEL_FORMAT 1

    /* ILI9488 LCD width and height */
    #define ILI9488_DPI_W 320
    #define ILI9488_DPI_H 480

/* dpi standard config */
#elif defined LCD_DPI_STANDARD

    /* Selecting DPI working mode
        1: DPI peripheral (support: bl808)
    */
    #define STANDARD_DPI_MODE 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 0

    /* Selecting pixel format
        1: rgb565 (16-bits)
    */
    #define STANDARD_DPI_PIXEL_FORMAT 1

    /* STANDARD LCD width and height */
    #define STANDARD_DPI_W 800
    #define STANDARD_DPI_H 480

    /* RGB timing parameter Settings
       Total Width = HSW + HBP + Active_Width + HFP
       Total Height = VSW + VBP + Active_Height + VFP */

    /* Hsync Pulse Width */
    #define STANDARD_DPI_HSW 4
    /* Hsync Back Porch */
    #define STANDARD_DPI_HBP 82
    /* Hsync Front Porch */
    #define STANDARD_DPI_HFP 14

    /* Vsync Pulse Width */
    #define STANDARD_DPI_VSW 5
    /* Vsync Back Porch */
    #define STANDARD_DPI_VBP 6
    /* Vsync Front Porch */
    #define STANDARD_DPI_VFP 39

    /* Maximum refresh frame rate per second, Used to automatically calculate the clock frequency */
    #define STANDARD_DPI_FRAME_RATE 70

/* spi ili9488 config */
#elif defined LCD_SPI_ILI9488

    /* Selecting interface type, more configuration of peripherals comes later
        1: SPI peripheral, supported functions: spi-4wire,
    */
    #define LCD_SPI_INTERFACE_TYPE 1

    /* Selecting pixel format
        1: rgb565
    */
    #define ILI9488_SPI_PIXEL_FORMAT 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* ILI9488 LCD width and height */
    #define ILI9488_SPI_W 320
    #define ILI9488_SPI_H 480

    /* The offset of the area can be displayed */
    #define ILI9488_SPI_OFFSET_X 0
    #define ILI9488_SPI_OFFSET_Y 0

    /* Color reversal, Some screens are required
        0: disable
        1: enable
    */
   #define ILI9488_SPI_COLOR_REVERSAL 0

/* spi ili9341 config */
#elif defined LCD_SPI_ILI9341

    /* Selecting interface type, more configuration of peripherals comes later
        1: SPI peripheral, supported functions: spi-4wire,
    */
    #define LCD_SPI_INTERFACE_TYPE 1

    /* Selecting pixel format
        1: rgb565, 16-bit/pixel
    */
    #define ILI9341_SPI_PIXEL_FORMAT 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* LCD width and height */
    #define ILI9341_SPI_W 240
    #define ILI9341_SPI_H 320

    /* The offset of the area can be displayed */
    #define ILI9341_SPI_OFFSET_X 0
    #define ILI9341_SPI_OFFSET_Y 0

    /* Color reversal, Some screens are required
        0: disable
        1: enable
    */
   #define ILI341_SPI_COLOR_REVERSAL 0

/* spi st7789v config */
#elif defined LCD_SPI_ST7789V

    /* Selecting interface type, more configuration of peripherals comes later
        1: SPI peripheral, supported functions: spi-4wire,
    */
    #define LCD_SPI_INTERFACE_TYPE 1

    /* Selecting pixel format
        1: rgb565
    */
    #define ST7789V_SPI_PIXEL_FORMAT 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* LCD width and height */
    #define ST7789V_SPI_W 320
    #define ST7789V_SPI_H 480

    /* The offset of the area can be displayed */
    #define ST7789V_SPI_OFFSET_X 0
    #define ST7789V_SPI_OFFSET_Y 0

/* spi st7796 config */
#elif defined LCD_SPI_ST7796

    /* Selecting interface type, more configuration of peripherals comes later
        1: SPI peripheral, supported functions: spi-4wire,
    */
    #define LCD_SPI_INTERFACE_TYPE 1

    /* Selecting pixel format
        1: rgb565
    */
    #define ST7796_SPI_PIXEL_FORMAT 1

    /* enable the lcd reset function
        0: Does not care about lcd hard reset
        1: use gpio to reset the lcd
    */
    #define LCD_RESET_EN 1

    /* LCD width and height */
    #define ST7796_SPI_W 320
    #define ST7796_SPI_H 480

    /* The offset of the area can be displayed */
    #define ST7796_SPI_OFFSET_X 0
    #define ST7796_SPI_OFFSET_Y 0

    /* Color reversal, Some screens are required
        0: disable
        1: enable
    */
   #define ST7796_SPI_COLOR_REVERSAL 0

#endif

/********** DBI peripheral configuration ***********/
#if (defined(LCD_DBI_INTERFACE_TYPE) && (LCD_DBI_INTERFACE_TYPE == 1))

    /* Select the working mode of DBI
        1: DBI TypeC 4-wire mode
        2: DBI TypeC 3-wire mode
        3: DBI TypeB x8 mode (8080)
        4: DBI QSPI mode (Not the mipi standard. Extra support for QSPI mode)
    */
    #define LCD_DBI_WORK_MODE 1

    /* dma used by DBI */
    #define LCD_DBI_DMA_NAME "dma0_ch3"

    /* The maximum amount of data to be transferred affects the number of LLI memory pools */
    #define DBI_DBI_DATA_SIZE_MAX (800 * 640 * 4)

    /* pin cfg */
    #if(LCD_DBI_WORK_MODE == 1)
        #define LCD_DBI_TYPEC_PIN_CLK   GPIO_PIN_12
        #define LCD_DBI_TYPEC_PIN_CS    GPIO_PIN_13
        #define LCD_DBI_TYPEC_PIN_DAT   GPIO_PIN_14
        #define LCD_DBI_TYPEC_PIN_DC    GPIO_PIN_15

    #elif(LCD_DBI_WORK_MODE == 2)
        #define LCD_DBI_TYPEC_PIN_CLK   GPIO_PIN_12
        #define LCD_DBI_TYPEC_PIN_CS    GPIO_PIN_13
        #define LCD_DBI_TYPEC_PIN_DAT   GPIO_PIN_14

    #elif(LCD_DBI_WORK_MODE == 3)
        #define LCD_DBI_TYPEB_PIN_WR    GPIO_PIN_4
        #define LCD_DBI_TYPEB_PIN_CS    GPIO_PIN_5
        #define LCD_DBI_TYPEB_PIN_RD    GPIO_PIN_6
        #define LCD_DBI_TYPEB_PIN_DC    GPIO_PIN_7

        #define LCD_DBI_TYPEB_PIN_DAT0  GPIO_PIN_8
        #define LCD_DBI_TYPEB_PIN_DAT1  GPIO_PIN_9
        #define LCD_DBI_TYPEB_PIN_DAT2  GPIO_PIN_10
        #define LCD_DBI_TYPEB_PIN_DAT3  GPIO_PIN_11
        #define LCD_DBI_TYPEB_PIN_DAT4  GPIO_PIN_12
        #define LCD_DBI_TYPEB_PIN_DAT5  GPIO_PIN_13
        #define LCD_DBI_TYPEB_PIN_DAT6  GPIO_PIN_14
        #define LCD_DBI_TYPEB_PIN_DAT7  GPIO_PIN_15

    #elif(LCD_DBI_WORK_MODE == 4)
        #define LCD_DBI_QSPI_PIN_CLK   GPIO_PIN_10
        #define LCD_DBI_QSPI_PIN_CS    GPIO_PIN_11

        #define LCD_DBI_QSPI_PIN_DAT0  GPIO_PIN_12
        #define LCD_DBI_QSPI_PIN_DAT1  GPIO_PIN_13
        #define LCD_DBI_QSPI_PIN_DAT2  GPIO_PIN_14
        #define LCD_DBI_QSPI_PIN_DAT3  GPIO_PIN_15

    #endif

#endif

/********** SPI hard 4-wire configuration ***********/
#if (defined(LCD_SPI_INTERFACE_TYPE) && (LCD_SPI_INTERFACE_TYPE == 1))

    /* spi idx */
    #define LCD_SPI_HARD_4_NAME "spi0"

    /* dma used by spi */
    #define LCD_SPI_HARD_4_DMA_NAME "dma0_ch3"

    /* The maximum pixel cnt to be transferred affects the number of LLI memory pools */
    #define LCD_SPI_HARD_4_PIXEL_CNT_MAX (800 * 640)

    /* spi pin, hardware controlled */
    #define LCD_SPI_HARD_4_PIN_CLK   GPIO_PIN_13
    #define LCD_SPI_HARD_4_PIN_DAT   GPIO_PIN_15
    /* cs/dc pin, software controlled */
    #define LCD_SPI_HARD_4_PIN_CS   GPIO_PIN_14
    #define LCD_SPI_HARD_4_PIN_DC   GPIO_PIN_16

#endif

/********** lcd reset configuration ***********/
#if (defined(LCD_RESET_EN) && LCD_RESET_EN)

/* lcd reset signal pin, please leave blank if not needed */
#define LCD_RESET_PIN GPIO_PIN_12

/* lcd reset signal active level
    0: lcd reset at low level
    1: lcd reset at high level
 */
#define LCD_RESET_ACTIVE_LEVEL  0

/* lcd reset active hold time (ms) */
#define LCD_RESET_HOLD_MS   10

/* lcd recovery time after reset end (ms) */
#define LCD_RESET_DELAY   100

#endif

/* clang-format on */

#endif
