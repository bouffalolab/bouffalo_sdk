#if __has_include("board_overlay.h")
/* Use board_overlay.c instead of this file */
#else

#ifdef CONFIG_CONSOLE_WO
#include "bflb_wo.h"
#else
#include "bflb_uart.h"
#endif
#include "bflb_gpio.h"
#include "bflb_clock.h"
#include "bflb_rtc.h"
#include "bflb_flash.h"
#include "bflb_xip_sflash.h"
#include "bflb_sf_ctrl.h"
#include "bflb_acomp.h"
#include "bflb_efuse.h"
#include "board.h"
#include "bl616cl_tzc_sec.h"
#include "bl616cl_pm.h"

#include "bl616cl_glb.h"

#include "board_flash_psram.h"

#include "mm.h"

extern void log_start(void);

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;
extern uint32_t __psram_heap_base;
extern uint32_t __psram_limit;

extern uint32_t __psram_data_start__;
extern uint32_t __psram_data_end__;
extern uint32_t __psram_noinit_data_start__;
extern uint32_t __psram_noinit_data_end__;

extern uint32_t _heap_wifi_start;
extern uint32_t _heap_wifi_size;

#ifdef CONFIG_CONSOLE_WO
static struct bflb_device_s *wo;
#else
static struct bflb_device_s *uart0;
#endif

#if (defined(CONFIG_LUA) || defined(CONFIG_BFLB_LOG) || defined(CONFIG_FATFS))
static struct bflb_device_s *rtc;
#endif

static void system_bod_init(void)
{
    HBN_BOD_CFG_Type bodCfg;
    bodCfg.enableBod = 1;
    /*0:BOD not trigger interrupt,1:trigger interrupt*/
    bodCfg.enableBodInt = 1;
    /* 0:2.2v, 1:2.3v, 2:2.4v .... 7:2.9v */
    bodCfg.bodThreshold = 2;
    /*1:BOD will cause reset, 0:BOD will not cause reset */
    bodCfg.enablePorInBod = 0;
    HBN_Set_BOD_Cfg(&bodCfg);
}

static void system_bod_isr(int irq, void *arg)
{
    printf("BOD interrupt occurred!\n");
    while (1) {
        ;
    }
}

static void ATTR_CLOCK_SECTION __attribute__((noinline)) system_clock_init(void)
{
#if 1
    /* wifipll */
    GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL);
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M);
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
#else
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_RC32M);
    GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL);
    GLB_Config_AUDIO_PLL_To_384M();
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_AUPLL_DIV1);
    GLB_Set_MCU_System_CLK_Div(0, 3);
#endif
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);

#ifdef CONFIG_WIFI6
    /* enable wifi clock */
    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);
#endif
}

#ifndef LP_APP
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
    PERIPHERAL_CLOCK_CAN_ENABLE();
    PERIPHERAL_CLOCK_AUDIO_ENABLE();
    PERIPHERAL_CLOCK_CKS_ENABLE();
    PERIPHERAL_CLOCK_PEC_ENABLE();

    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);
    GLB_Set_SPI0_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, 0);
    GLB_Set_SPI1_CLK(ENABLE, GLB_SPI_CLK_MCU_MUXPLL_160M, 0);
    GLB_Set_DBI_CLK(ENABLE, GLB_DBI_CLK_MCU_MUXPLL_160M, 0);
    GLB_Set_I2C_CLK(ENABLE, GLB_I2C_CLK_XCLK, 0);
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 19);
    GLB_Set_DIG_CLK_Sel(GLB_DIG_CLK_XCLK);
    GLB_Set_DIG_512K_CLK(ENABLE, ENABLE, 0x4E);
    GLB_Set_PWM1_IO_Sel(GLB_PWM1_IO_SINGLE_END);
    GLB_Set_IR_CLK(ENABLE, GLB_IR_CLK_SRC_XCLK, 19);
    GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_WIFIPLL_96M, 3);
    GLB_Set_PEC_CLK(ENABLE, GLB_PEC_CLK_MCU_MUXPLL_160M, 0);

    GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_WIFIPLL_320M);

    GLB_Swap_MCU_SPI_0_MOSI_With_MISO(0);
}

#else

static void peripheral_clock_init_lp(void)
{

}
#endif

#if defined(CONFIG_ANTI_ROLLBACK) && !defined(CONFIG_BOOT2)
extern const bflb_verinf_t app_ver;
uint8_t efuse_version = 0xFF;

static void bflb_check_anti_rollback(void)
{
    if (0 != bflb_get_app_version_from_efuse(&efuse_version)) {
        printf("error! can't read app version in efuse\r\n");
        while (1) {}
    } else {
        printf("app version in efuse is: %d\r\n", efuse_version);
    }

    if (app_ver.anti_rollback < efuse_version) {
        printf("app version in application is: %d, less than app version in efuse, the application should not run up\r\n", app_ver.anti_rollback);
    } else {
        printf("app version in application is: %d, not less than app version in efuse, the application should run up\r\n", app_ver.anti_rollback);
    }

    /* change app version in efuse to app_ver.anti_rollback, default is 0 */
    if (app_ver.anti_rollback > efuse_version) {
        bflb_set_app_version_to_efuse(app_ver.anti_rollback); //be attention! app version in efuse is incremental(from 0 to 128), and cannot be reduced forever
        printf("update app version in efuse to %d\r\n", app_ver.anti_rollback);

        /* check app version in efuse */
        if (0 != bflb_get_app_version_from_efuse(&efuse_version)) {
            printf("error! can't read app version in efuse\r\n");
            while (1) {}
        } else {
            printf("app version in efuse is: %d\r\n", efuse_version);
        }
    }
}
#endif

void bl_show_log(void)
{
    puts("\r\n");
    puts("  ____               __  __      _       _       _     \r\n");
    puts(" |  _ \\             / _|/ _|    | |     | |     | |    \r\n");
    puts(" | |_) | ___  _   _| |_| |_ __ _| | ___ | | __ _| |__  \r\n");
    puts(" |  _ < / _ \\| | | |  _|  _/ _` | |/ _ \\| |/ _` | '_ \\ \r\n");
    puts(" | |_) | (_) | |_| | | | || (_| | | (_) | | (_| | |_) |\r\n");
    puts(" |____/ \\___/ \\__,_|_| |_| \\__,_|_|\\___/|_|\\__,_|_.__/ \r\n");
    puts("\r\n");
    puts("Build:" __TIME__ ", " __DATE__ "\r\n");
    puts("Copyright (c) 2022 Bouffalolab team\r\n");
}

const char *bl_sys_version(const char ***ctx)
{
    extern uint8_t _version_info_section_start;
    extern uint8_t _version_info_section_end;
    const char **const version_section_start = (const char **)&_version_info_section_start;
    const char **const version_section_end = (const char **)&_version_info_section_end;
    const char *version_str;

    //init
    if (NULL == (*ctx)) {
        (*ctx) = version_section_start;
    }
    //check the end
    if (version_section_end == (*ctx)) {
        return NULL;
    }
    version_str = (**ctx);
    *ctx = (*ctx) + 1;
    return version_str;
}

void bl_show_component_version(void)
{
    const char *version_str;
    const char **ctx = NULL;
    bflb_verinf_t version = { 0 };

    puts("Version of used components:\r\n");
    while ((version_str = bl_sys_version(&ctx))) {
        puts("\tVersion: ");
        puts(version_str);
        puts("\r\n");
    }

#ifdef CONFIG_PM
    char *bl_lpfw_bin_get_version_str(void);
    char *lpfw_ver = bl_lpfw_bin_get_version_str();

    puts("\tVersion: component_version_lpfw: ");
    if (lpfw_ver) {
        puts(lpfw_ver);
    } else {
        puts("Not found (NULL)");
    }
    puts("\r\n");
#endif

    if (bflb_sf_ctrl_get_flash_image_offset(0, 0) <= 0x2000) {
        puts("\tBoot2 maybe not exist!\r\n");
    } else {
        puts("\tVersion: ");
        bflb_get_boot2_info_from_flash(&version);
        printf("component_version_boot2: %d.%d.%d\r\n", version.x, version.y, version.z);
    }
}

#ifdef CONFIG_CONSOLE_WO
extern void bflb_wo_set_console(struct bflb_device_s *dev);
#else
extern void bflb_uart_set_console(struct bflb_device_s *dev);
#endif

static void console_init()
{
#ifdef CONFIG_CONSOLE_WO
    wo = bflb_device_get_by_name("wo");
    bflb_wo_uart_init(wo, 2000000, GPIO_PIN_34);
    bflb_wo_set_console(wo);
#else
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_34, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_35, GPIO_UART_FUNC_UART0_RX);

    struct bflb_uart_config_s cfg = { 0 };
    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    cfg.bit_order = UART_LSB_FIRST;

    uart0 = bflb_device_get_by_name("uart0");

    bflb_uart_init(uart0, &cfg);
    bflb_uart_set_console(uart0);
#endif
}

#ifdef LP_APP
void board_recovery(void)
{
#ifdef CONF_PSRAM_RESTORE
    board_psram_x8_init();
#endif

    system_clock_init();
    peripheral_clock_init_lp();
    console_init();
}
#endif

#ifdef CONFIG_HIGH_ISR_STACK
void bflb_wfa_init(void)
{
    extern void interrupt1_handler(void);
    bflb_irq_attach(MAC_INT_PROT_TRIGGER_IRQn, (irq_callback)interrupt1_handler, NULL);
    bflb_irq_enable(MAC_INT_PROT_TRIGGER_IRQn);
    extern void csi_vic_set_prio(int32_t IRQn, uint32_t priority);
    csi_vic_set_prio(MAC_INT_PROT_TRIGGER_IRQn, 2);
}
#endif

void ram_heap_init(void)
{
    size_t heap_len;

    /* ram heap init */
    mem_manager_init();

    /* ocram heap init */
    heap_len = ((size_t)&__HeapLimit - (size_t)&__HeapBase);
    mm_register_heap(MM_HEAP_OCRAM_0, "OCRAM", MM_ALLOCATOR_TLSF, &__HeapBase, heap_len);
    mm_register_heap(MM_HEAP_WRAM_0, "WRAM", MM_ALLOCATOR_TLSF, &_heap_wifi_start, (uintptr_t)&_heap_wifi_size);

#ifdef CONFIG_PSRAM
    /* psram init */
    if (board_psram_x8_init() != SUCCESS) {
        puts("psram init fail !!!\r\n");
        while (1) {}
    }

    /* psram heap init */
    heap_len = ((size_t)&__psram_limit - (size_t)&__psram_heap_base);
    mm_register_heap(MM_HEAP_PSRAM_0, "PSRAM", MM_ALLOCATOR_TLSF, &__psram_heap_base, heap_len);

    /* ram info dump */
    printf("dynamic memory init success\r\n"
           "  ocram heap size: %d Kbyte, \r\n"
           "  psram heap size: %d Kbyte\r\n",
           ((size_t)&__HeapLimit - (size_t)&__HeapBase) / 1024,
           ((size_t)&__psram_limit - (size_t)&__psram_heap_base) / 1024);

#else
    /* check psram data */
    if (&__psram_data_end__ - &__psram_data_start__ > 0 || &__psram_noinit_data_end__ - &__psram_noinit_data_start__ > 0) {
        puts("psram data already exists, please enable CONFIG_PSRAM\r\n");
        while (1) {}
    }

    /* ram info dump */
    printf("dynamic memory init success\r\n"
           "  ocram heap size: %d Kbyte \r\n",
           ((size_t)&__HeapLimit - (size_t)&__HeapBase) / 1024);
#endif
}

#define D(field, t) printf(#field ":" #t "\r\n", field)
static void ebreak_cpu(void)
{
    printf("bus error/timeout occurs!\n");

    __ASM volatile("ebreak");
}
#undef D

void board_init(void)
{
    int ret = -1;
    uintptr_t flag;

    /* lock */
    flag = bflb_irq_save();

#ifndef CONFIG_BOARD_FLASH_INIT_SKIP
    /* flash init */
    ret = bflb_flash_init();
#ifndef CONFIG_BOARD_FLASH_LOW_SPEED
    /* flash clock frequency increased to 80MHz */
    board_set_flash_80m();
#endif
#endif

    /* system clock */
    system_clock_init();

#ifndef LP_APP
    peripheral_clock_init();
#else
    peripheral_clock_init_lp();
#endif

    /* irq init */
    bflb_irq_initialize();
#ifdef CONFIG_WIFI6
    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);
#endif
#ifdef CONFIG_HIGH_ISR_STACK
    bflb_wfa_init();
#endif

    /* console init (uart or wo) */
    console_init();

    /* config chip pod */
    bflb_irq_attach(BOD_IRQn, system_bod_isr, NULL);
    bflb_irq_enable(BOD_IRQn);
    system_bod_init();

    /* ram and heap init (including psram) */
    ram_heap_init();

    /* boot info dump */
    bl_show_log();
    /* version info dump */
    bl_show_component_version();

#if defined(CONFIG_ANTI_ROLLBACK) && !defined(CONFIG_BOOT2)
    bflb_check_anti_rollback();
#endif

    /* flash info dump */
    bl_show_flashinfo();
    if (ret != 0) {
        printf("flash init fail !!!\r\n");
    }
    //printf("cgen1:%08x\r\n", getreg32(BFLB_GLB_CGEN1_BASE));

    log_start();

#if (defined(CONFIG_LUA) || defined(CONFIG_BFLB_LOG) || defined(CONFIG_FATFS))
    rtc = bflb_device_get_by_name("rtc");
#endif

#ifdef CONFIG_MBEDTLS
    extern void bflb_sec_mutex_init(void);
    bflb_sec_mutex_init();
#endif

    /* unlock */
    bflb_irq_restore(flag);

    printf("board init done\r\n");
    printf("===========================\r\n");

    /* mcu_bus_cfg0 */
    *(volatile uint32_t *)MCU_MISC_BASE |= 0x1 << MCU_MISC_REG_MCU_INFRA_TIMEOUT_EN_POS;
    *(volatile uint32_t *)MCU_MISC_BASE &= ~(0x1 << MCU_MISC_DEC_ERR_RSP_POS);

    bflb_irq_attach(BMX_MCU_BUS_ERR_IRQn, (irq_callback)ebreak_cpu, NULL);
    bflb_irq_attach(BMX_MCU_TO_IRQn, (irq_callback)ebreak_cpu, NULL);

    bflb_irq_enable(BMX_MCU_BUS_ERR_IRQn);
    bflb_irq_enable(BMX_MCU_TO_IRQn);
}

void board_acomp_init(void)
{
    struct bflb_device_s *gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_ANALOG | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_ANALOG | GPIO_DRV_0);

    struct bflb_acomp_config_s acomp_cfg = {
        .mux_en = ENABLE,
        .pos_chan_sel = AON_ACOMP_CHAN_ADC0,
        .neg_chan_sel = AON_ACOMP_CHAN_VIO_X_SCALING_FACTOR_1,
        .vio_sel = DEFAULT_ACOMP_VREF_1V65,
        .scaling_factor = AON_ACOMP_SCALING_FACTOR_1,
        .bias_prog = AON_ACOMP_BIAS_POWER_MODE1,
        .hysteresis_pos_volt = AON_ACOMP_HYSTERESIS_VOLT_NONE,
        .hysteresis_neg_volt = AON_ACOMP_HYSTERESIS_VOLT_NONE,
    };

    acomp_cfg.pos_chan_sel = AON_ACOMP_CHAN_ADC3;
    bflb_acomp_init(AON_ACOMP0_ID, &acomp_cfg);
    acomp_cfg.pos_chan_sel = AON_ACOMP_CHAN_ADC0;
    bflb_acomp_init(AON_ACOMP1_ID, &acomp_cfg);
}

#ifdef CONFIG_BFLB_LOG
__attribute__((weak)) uint64_t bflb_log_clock(void)
{
    return bflb_mtimer_get_time_us();
}

__attribute__((weak)) uint32_t bflb_log_time(void)
{
    return BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc));
}

__attribute__((weak)) char *bflb_log_thread(void)
{
    return "";
}
#endif

#ifdef CONFIG_LUA
__attribute__((weak)) clock_t luaport_clock(void)
{
    return (clock_t)bflb_mtimer_get_time_us();
}

__attribute__((weak)) time_t luaport_time(time_t *seconds)
{
    time_t t = (time_t)BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc));
    if (seconds != NULL) {
        *seconds = t;
    }

    return t;
}
#endif

#ifdef CONFIG_FATFS
#include "bflb_timestamp.h"
__attribute__((weak)) uint32_t get_fattime(void)
{
    bflb_timestamp_t tm;

    bflb_timestamp_utc2time(BFLB_RTC_TIME2SEC(bflb_rtc_get_time(rtc)), &tm);

    return ((uint32_t)(tm.year - 1980) << 25) /* Year 2015 */
           | ((uint32_t)tm.mon << 21)         /* Month 1 */
           | ((uint32_t)tm.mday << 16)        /* Mday 1 */
           | ((uint32_t)tm.hour << 11)        /* Hour 0 */
           | ((uint32_t)tm.min << 5)          /* Min 0 */
           | ((uint32_t)tm.sec >> 1);         /* Sec 0 */
}
#endif

#ifdef CONFIG_SHELL
#include "shell.h"

static void show_sys_version_cmd(int argc, char **argv)
{
    bl_show_component_version();
}
SHELL_CMD_EXPORT_ALIAS(show_sys_version_cmd, sysver, show sys version);

extern int bl_sys_reset_por(void);

static void reboot_cmd(int argc, char **argv)
{
    if(argc > 1){
        if(strcmp(argv[1],"uart")==0){
            HBN_Set_User_Boot_Config(1);
        }
    }
    bl_sys_reset_por();
}
SHELL_CMD_EXPORT_ALIAS(reboot_cmd, reboot, reboot);

#define MFG_CONFIG_REG (0x2000F100)
#define MFG_CONFIG_VAL ("0mfg")

void mfg_config(void)
{
    union _reg_t {
        uint8_t byte[4];
        uint32_t word;
    } mfg = {
        .byte = MFG_CONFIG_VAL,
    };

    *(volatile uint32_t *)(MFG_CONFIG_REG) = mfg.word;
}

static void mfg_cmd(int argc, char **argv)
{
    mfg_config();
    bl_sys_reset_por();
}
SHELL_CMD_EXPORT_ALIAS(mfg_cmd, mfg, mfg);

#ifdef LP_APP
#include "bl_lp.h"

static void test_io_wakeup_status(uint8_t io_num)
{
    int wakeup_mode;

    wakeup_mode = bl_lp_wakeup_io_get_mode(io_num);

    if (wakeup_mode) {
        /* user can add app process logic */
#if 0
        if(io_num == WAKEUP_BUTTON){
            /* deal a button wakeup event */
        }else if(io_num == WAKEUP_FINGER_INPUT){
            /* deal fingerprint input wake up event */
        }else if(io_num == WAKEUP_USB_INPUT){
            /* deal USB plug in wake up event */
        }
#endif
        printf("GPIO %d wakeup: ", io_num);
        if (wakeup_mode == BL_LP_IO_WAKEUP_MODE_LOW) {
            printf("level low\r\n");
        } else if (wakeup_mode == BL_LP_IO_WAKEUP_MODE_HIGH) {
            printf("level high\r\n");
        } else if (wakeup_mode == BL_LP_IO_WAKEUP_MODE_FALLING) {
            printf("edge falling\r\n");
        } else if (wakeup_mode == BL_LP_IO_WAKEUP_MODE_RISING) {
            printf("edge rising\r\n");
        } else {
            printf("unknown error: %d\r\n", wakeup_mode);
        }
    }
}

static void test_wakeup_io_callback(uint64_t wake_up_io_bits)
{
    printf("io wakeup bits 0x%llX\r\n", (unsigned long long)wake_up_io_bits);

    /* wakeup io status dump: only report pins that actually triggered */
    for (uint8_t i = 0; i < BL_LP_WAKEUP_IO_MAX_NUM; i++) {
        if (wake_up_io_bits & ((uint64_t)1 << i)) {
            test_io_wakeup_status(i);
        }
    }
}

void cmd_io_test(char *buf, int len, int argc, char **argv)
{
    static lp_gpio_cfg_type lp_wake_io_cfg = { 0};

    /* trigger mode */
    lp_wake_io_cfg.io_0_5_trig_mode = PDS_GPIO_INT_SYNC_FALLING_EDGE; /* gpio 0-5 */
    lp_wake_io_cfg.io_6_36_trig_mode[0] = PDS_GPIO_INT_SYNC_FALLING_EDGE; /* gpio 6 */
    lp_wake_io_cfg.io_6_36_trig_mode[17] = PDS_GPIO_INT_SYNC_RISING_FALLING_EDGE; /* gpio 23 */


    /* wake up unmask */
    lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 0); /* gpio 0 */
    // lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 10); /* gpio 10 */
    lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 18); /* gpio 18 */
    // lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 19); /* gpio 19 */
    // lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 20); /* gpio 20 */

    lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 31); /* gpio 31 */
    // lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 32); /* gpio 32 */
    // lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 33);     /* gpio 33 */
    // lp_wake_io_cfg.io_wakeup_unmask |= ((uint64_t)1 << 34);     /* gpio 34 */

    lp_wake_io_cfg.io_ie = lp_wake_io_cfg.io_wakeup_unmask;

    bl_lp_io_wakeup_cfg(&lp_wake_io_cfg);

    /* register io wakeup callback */
    bl_lp_wakeup_io_int_register(test_wakeup_io_callback);
}

SHELL_CMD_EXPORT_ALIAS(cmd_io_test, io_test, cmd io_test);
#endif /* LP_APP */

#endif /* CONFIG_SHELL */

#endif /* __has_include("board_overlay.h") */
