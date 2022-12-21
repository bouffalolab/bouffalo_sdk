#include "bflb_uart.h"
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_rtc.h"
#include "bflb_flash.h"
#ifdef CONFIG_TLSF
#include "bflb_tlsf.h"
#else
#include "bflb_mmheap.h"
#endif
#include "board.h"
#include "bl702_glb.h"
#include "bl702_sflash.h"
#include "bl702_psram.h"

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

#ifndef CONFIG_TLSF
struct heap_info mmheap_root;

static struct heap_region system_mmheap[] = {
    { NULL, 0 },
    { NULL, 0 }, /* Terminates the array. */
};
#endif

static struct bflb_device_s *uart0;

#if defined(CONFIG_BFLOG)
static struct bflb_device_s *rtc;
#endif

static void system_clock_init(void)
{
    GLB_Set_System_CLK(GLB_DLL_XTAL_32M, GLB_SYS_CLK_DLL144M);
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 71);
}

static void peripheral_clock_init(void)
{
    PERIPHERAL_CLOCK_ADC_DAC_ENABLE();
    PERIPHERAL_CLOCK_SEC_ENABLE();
    PERIPHERAL_CLOCK_DMA0_ENABLE();
    PERIPHERAL_CLOCK_UART0_ENABLE();
    PERIPHERAL_CLOCK_UART1_ENABLE();
    PERIPHERAL_CLOCK_SPI0_ENABLE();
    PERIPHERAL_CLOCK_I2C0_ENABLE();
    PERIPHERAL_CLOCK_PWM0_ENABLE();
    PERIPHERAL_CLOCK_TIMER0_1_WDG_ENABLE();
    PERIPHERAL_CLOCK_IR_ENABLE();
    PERIPHERAL_CLOCK_I2S_ENABLE();
    PERIPHERAL_CLOCK_USB_ENABLE();
    GLB_AHB_Slave1_Clock_Gate(DISABLE,BL_AHB_SLAVE1_CAM);

    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_96M, 0);
    GLB_Set_SPI_CLK(ENABLE, 0);
    GLB_Set_I2C_CLK(ENABLE, 0);
    GLB_Set_IR_CLK(ENABLE, GLB_IR_CLK_SRC_XCLK, 15);

    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 1);
    GLB_Set_DAC_CLK(ENABLE, GLB_DAC_CLK_XCLK, 0x3E);

    GLB_Set_USB_CLK(ENABLE);
}

/* bsp sf psram private variables */

SPI_Psram_Cfg_Type apMemory1604 = {
    .readIdCmd = 0x9F,
    .readIdDmyClk = 0,
    .burstToggleCmd = 0xC0,
    .resetEnableCmd = 0x66,
    .resetCmd = 0x99,
    .enterQuadModeCmd = 0x35,
    .exitQuadModeCmd = 0xF5,
    .readRegCmd = 0xB5,
    .readRegDmyClk = 1,
    .writeRegCmd = 0xB1,
    .readCmd = 0x03,
    .readDmyClk = 0,
    .fReadCmd = 0x0B,
    .fReadDmyClk = 1,
    .fReadQuadCmd = 0xEB,
    .fReadQuadDmyClk = 3,
    .writeCmd = 0x02,
    .quadWriteCmd = 0x38,
    .pageSize = 512,
    .ctrlMode = PSRAM_SPI_CTRL_MODE,
    .driveStrength = PSRAM_DRIVE_STRENGTH_50_OHMS,
    .burstLength = PSRAM_BURST_LENGTH_512_BYTES,
};

SF_Ctrl_Cmds_Cfg cmdsCfg = {
    .cmdsEn = ENABLE,
    .burstToggleEn = ENABLE,
    .wrapModeEn = DISABLE,
    .wrapLen = SF_CTRL_WRAP_LEN_512,
};
SF_Ctrl_Psram_Cfg sfCtrlPsramCfg = {
    .owner = SF_CTRL_OWNER_SAHB,
    .padSel = SF_CTRL_PAD_SEL_DUAL_CS_SF2,
    .bankSel = SF_CTRL_SEL_PSRAM,
    .psramRxClkInvertSrc = ENABLE,
    .psramRxClkInvertSel = DISABLE,
    .psramDelaySrc = ENABLE,
    .psramClkDelay = 1,
};

#define BFLB_EXTFLASH_CS_GPIO    GLB_GPIO_PIN_25
#define BFLB_EXTPSRAM_CLK_GPIO   GLB_GPIO_PIN_27
#define BFLB_EXTPSRAM_CS_GPIO    GLB_GPIO_PIN_17
#define BFLB_EXTPSRAM_DATA0_GPIO GLB_GPIO_PIN_28
#define BFLB_EXTPSRAM_DATA1_GPIO GLB_GPIO_PIN_24
#define BFLB_EXTPSRAM_DATA2_GPIO GLB_GPIO_PIN_23
#define BFLB_EXTPSRAM_DATA3_GPIO GLB_GPIO_PIN_26

void ATTR_TCM_SECTION psram_gpio_init(void)
{
    GLB_GPIO_Cfg_Type cfg;
    uint8_t gpiopins[7];
    uint8_t i = 0;

    cfg.gpioMode = GPIO_MODE_AF;
    cfg.pullType = GPIO_PULL_UP;
    cfg.drive = 3;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_FLASH_PSRAM;

    gpiopins[0] = BFLB_EXTPSRAM_CLK_GPIO;
    gpiopins[1] = BFLB_EXTPSRAM_CS_GPIO;
    gpiopins[2] = BFLB_EXTPSRAM_DATA0_GPIO;
    gpiopins[3] = BFLB_EXTPSRAM_DATA1_GPIO;
    gpiopins[4] = BFLB_EXTPSRAM_DATA2_GPIO;
    gpiopins[5] = BFLB_EXTPSRAM_DATA3_GPIO;
    gpiopins[6] = BFLB_EXTFLASH_CS_GPIO;

    for (i = 0; i < sizeof(gpiopins); i++) {
        cfg.gpioPin = gpiopins[i];

        if (i == 0 || i == 1 || i == 6) {
            /*flash clk and cs is output*/
            cfg.gpioMode = GPIO_MODE_OUTPUT;
        } else {
            /*data are bidir*/
            cfg.gpioMode = GPIO_MODE_AF;
        }

        GLB_GPIO_Init(&cfg);
    }
}

uint8_t psramId[8] = { 0 };

void ATTR_TCM_SECTION board_psram_init(void)
{
    psram_gpio_init();

    Psram_Init(&apMemory1604, &cmdsCfg, &sfCtrlPsramCfg);

    Psram_SoftwareReset(&apMemory1604, apMemory1604.ctrlMode);

    Psram_ReadId(&apMemory1604, psramId);
    Psram_Cache_Write_Set(&apMemory1604, SF_CTRL_QIO_MODE, ENABLE, DISABLE, DISABLE);
    L1C_Cache_Enable_Set(L1C_WAY_DISABLE_NONE);
}

void bl_show_log(void)
{
    printf("\r\n");
    printf("  ____               __  __      _       _       _     \r\n");
    printf(" |  _ \\             / _|/ _|    | |     | |     | |    \r\n");
    printf(" | |_) | ___  _   _| |_| |_ __ _| | ___ | | __ _| |__  \r\n");
    printf(" |  _ < / _ \\| | | |  _|  _/ _` | |/ _ \\| |/ _` | '_ \\ \r\n");
    printf(" | |_) | (_) | |_| | | | || (_| | | (_) | | (_| | |_) |\r\n");
    printf(" |____/ \\___/ \\__,_|_| |_| \\__,_|_|\\___/|_|\\__,_|_.__/ \r\n");
    printf("\r\n");
    printf("Build:%s,%s\r\n", __TIME__, __DATE__);
    printf("Copyright (c) 2022 Bouffalolab team\r\n");
}

void bl_show_flashinfo(void)
{
    SPI_Flash_Cfg_Type flashCfg;
    uint8_t *pFlashCfg = NULL;
    uint32_t flashCfgLen = 0;
    uint32_t flashJedecId = 0;

    flashJedecId = bflb_flash_get_jedec_id();
    bflb_flash_get_cfg(&pFlashCfg, &flashCfgLen);
    arch_memcpy((void *)&flashCfg, pFlashCfg, flashCfgLen);
    printf("=========== flash cfg ==============\r\n");
    printf("jedec id   0x%06X\r\n", flashJedecId);
    printf("mid            0x%02X\r\n", flashCfg.mid);
    printf("iomode         0x%02X\r\n", flashCfg.ioMode);
    printf("clk delay      0x%02X\r\n", flashCfg.clkDelay);
    printf("clk invert     0x%02X\r\n", flashCfg.clkInvert);
    printf("read reg cmd0  0x%02X\r\n", flashCfg.readRegCmd[0]);
    printf("read reg cmd1  0x%02X\r\n", flashCfg.readRegCmd[1]);
    printf("write reg cmd0 0x%02X\r\n", flashCfg.writeRegCmd[0]);
    printf("write reg cmd1 0x%02X\r\n", flashCfg.writeRegCmd[1]);
    printf("qe write len   0x%02X\r\n", flashCfg.qeWriteRegLen);
    printf("cread support  0x%02X\r\n", flashCfg.cReadSupport);
    printf("cread code     0x%02X\r\n", flashCfg.cReadMode);
    printf("burst wrap cmd 0x%02X\r\n", flashCfg.burstWrapCmd);
    printf("=====================================\r\n");
}

extern void bflb_uart_set_console(struct bflb_device_s *dev);

static void console_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_14, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_15, GPIO_UART_FUNC_UART0_RX);

    struct bflb_uart_config_s cfg;
    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 15;
    cfg.rx_fifo_threshold = 15;

    uart0 = bflb_device_get_by_name("uart0");

    bflb_uart_init(uart0, &cfg);
    bflb_uart_set_console(uart0);
}

void board_init(void)
{
    uintptr_t flag;

    flag = bflb_irq_save();

    bflb_flash_init();

    system_clock_init();
    peripheral_clock_init();
    bflb_irq_initialize();

    bflb_irq_restore(flag);

#ifdef CONFIG_TLSF
    bflb_mmheap_init((void *)&__HeapBase, ((size_t)&__HeapLimit - (size_t)&__HeapBase));
#else
    system_mmheap[0].addr = (uint8_t *)&__HeapBase;
    system_mmheap[0].mem_size = ((size_t)&__HeapLimit - (size_t)&__HeapBase);

    if (system_mmheap[0].mem_size > 0) {
        bflb_mmheap_init(&mmheap_root, system_mmheap);
    }
#endif

    console_init();

    bl_show_log();
    bl_show_flashinfo();

    printf("dynamic memory init success,heap size = %d Kbyte \r\n", ((size_t)&__HeapLimit - (size_t)&__HeapBase) / 1024);

    printf("cgen1:%08x\r\n", getreg32(BFLB_GLB_CGEN1_BASE));
#if defined(CONFIG_BFLOG)
    rtc = bflb_device_get_by_name("rtc");
#endif
#ifdef CONFIG_PSRAM
    board_psram_init();
#endif
}

void board_uartx_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_18, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_19, GPIO_UART_FUNC_UART1_RX);
}

void board_i2c0_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* I2C0_SDA */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* I2C0_SCL */
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_spi0_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* spi clk */
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* spi miso */
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    /* spi mosi */
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUNC_SPI0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_pwm_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_PWM0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
}

void board_ir_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* IR TX support GPIO 22 or GPIO 23 */
    bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    GLB_IR_LED_Driver_Output_Disable(GLB_GPIO_PIN_23);
    GLB_IR_LED_Driver_Output_Enable(GLB_GPIO_PIN_22);

    /* IR RX support GPIO 17 ~ GPIO 31 */
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_0);
    GLB_IR_RX_GPIO_Sel(GLB_GPIO_PIN_18);
}

void board_adc_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}

void board_dac_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* DAC_CHA */
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* DAC_CHB */
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}

void board_emac_gpio_init()
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_7, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_8, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_21, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_22, GPIO_FUNC_EMAC | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* enable audio clock */
    PDS_Enable_PLL_Clk(PDS_PLL_CLK_48M);
    PDS_Set_Audio_PLL_Freq(AUDIO_PLL_50000000_HZ);
}

void board_keyscan_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUN_KEY_SCAN_COL | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUN_KEY_SCAN_COL | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUN_KEY_SCAN_COL | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUN_KEY_SCAN_COL | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_OUTPUT | GPIO_SMT_EN | GPIO_DRV_1);

    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUN_KEY_SCAN_ROW | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_17, GPIO_FUN_KEY_SCAN_ROW | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_18, GPIO_FUN_KEY_SCAN_ROW | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_FUN_KEY_SCAN_ROW | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_INPUT | GPIO_SMT_EN | GPIO_DRV_1);

    GLB_Set_QDEC_CLK(GLB_QDEC_CLK_F32K, 0);
}

void board_dvp_gpio_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* I2C GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_16, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_11, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    /* MCLK, use GPIO 10 */
    //bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_FUNC_CAM_MCLK | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    //GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_DLL96M, 3);

    /* MCLK, use GPIO 9 */
    bflb_gpio_init(gpio, GPIO_PIN_9, GPIO_FUNC_CLK_OUT | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    PDS_Set_Audio_PLL_Freq(AUDIO_PLL_24576000_HZ);
    GLB_Set_Chip_Out_1_CLK_Sel(GLB_CHIP_CLK_OUT_I2S_REF_CLK);

    /* DVP GPIO */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_4, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_5, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_6, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_30, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_31, GPIO_FUNC_CAM | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

    GLB_SWAP_EMAC_CAM_Pin(GLB_EMAC_CAM_PIN_CAM);
}

#ifdef CONFIG_BFLOG
__attribute__((weak)) uint64_t bflog_clock(void)
{
    return bflb_mtimer_get_time_us();
}

__attribute__((weak)) uint32_t bflog_time(void)
{
    return BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc));
}

__attribute__((weak)) char *bflog_thread(void)
{
    return "";
}
#endif