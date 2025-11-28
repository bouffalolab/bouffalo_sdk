
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
#include "bl616d_tzc_sec.h"
#include "bl616d_psram.h"
#include "bl616d_glb.h"

#include "board_flash_psram.h"

#include "mm.h"

extern void log_start(void);

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;
extern uint32_t __psram_heap_base;
extern uint32_t __psram_limit;

extern uint32_t __psram_data_start__;
extern uint32_t __psram_data_end__;
extern uint32_t __psram_noinit_start__;
extern uint32_t __psram_noinit_end__;

#ifdef CONFIG_CONSOLE_WO
static struct bflb_device_s *wo;
#else
static struct bflb_device_s *uart;
#endif

#if (defined(CONFIG_LUA) || defined(CONFIG_BFLB_LOG) || defined(CONFIG_FATFS))
static struct bflb_device_s *rtc;
#endif

static void ATTR_CLOCK_SECTION __attribute__((noinline)) system_clock_init(void)
{
    if (GLB_CORE_ID_AP == GLB_Get_Core_Type()) {
        /* wifipll/cpupll */
        GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL | GLB_PLL_CPUPLL);
#if defined(CPU_MODEL_A0)
        GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M);
        HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_RC32M);
        GLB_Set_WL_MCU_System_CLK(GLB_WL_MCU_SYS_CLK_CPUPLL_DIV1, 0, 2);
#else
        GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_CPUPLL_DIV1);
        HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
        GLB_Set_WL_MCU_System_CLK(GLB_WL_MCU_SYS_CLK_WIFIPLL_DIV2, 0, 1);
#endif
        GLB_Set_WL_XCLK_Sel(GLB_WL_MCU_XCLK_XTAL);
#if 0
        /* RC32M=8M */
        //AON_Set_RC32M_Speed_As_8M(1);
        *(volatile uint32_t *)(0x2008F994) = 0x58000000;
#else
        /* RC32M=32M */
        //AON_Set_RC32M_Speed_As_8M(0);
        *(volatile uint32_t *)(0x2008F994) = 0x5A000000;
#endif

        CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);

        /* enable wifi clock */
        GLB_Set_WIFIPLL_Fine_Tune();
        GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    } else if (GLB_CORE_ID_NP == GLB_Get_Core_Type()) {
        CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_WL_XCLK) / 1000000 - 1);
    }
}

#ifndef LP_APP
static void peripheral_clock_init(void)
{
    if (GLB_CORE_ID_AP == GLB_Get_Core_Type()) {
        PERIPHERAL_CLOCK_ADC_DAC_ENABLE();
        PERIPHERAL_CLOCK_SEC_ENABLE();
        PERIPHERAL_CLOCK_DMA0_ENABLE();
        PERIPHERAL_CLOCK_UART0_ENABLE();
        PERIPHERAL_CLOCK_UART1_ENABLE();
#if defined(CPU_MODEL_A0)
        PERIPHERAL_CLOCK_SPI0_ENABLE();
        PERIPHERAL_CLOCK_SPI1_ENABLE();
#else
        PERIPHERAL_CLOCK_SPI0_2_ENABLE();
        PERIPHERAL_CLOCK_SPI3_ENABLE();
#endif
        PERIPHERAL_CLOCK_I2C0_ENABLE();
        PERIPHERAL_CLOCK_PWM0_ENABLE();
        PERIPHERAL_CLOCK_TIMER0_1_WDG_ENABLE();
        PERIPHERAL_CLOCK_IR_ENABLE();
        PERIPHERAL_CLOCK_I2S_ENABLE();
        PERIPHERAL_CLOCK_USB_ENABLE();
        PERIPHERAL_CLOCK_CAN_ENABLE();
        PERIPHERAL_CLOCK_CKS_ENABLE();
        PERIPHERAL_CLOCK_PEC_ENABLE();

        GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);

#if defined(CPU_MODEL_A0)
        GLB_Set_SPI1_CLK(ENABLE, GLB_SPI1_CLK_WIFIPLL_160M, 0);
        GLB_Set_SPI0_CLK(ENABLE, GLB_SPI_CLK_WIFIPLL_160M, 0);
#else
        GLB_Set_SPI0_2_CLK(ENABLE, GLB_SPI_CLK_WIFIPLL_160M, 0);
        GLB_Set_SPI3_CLK(ENABLE, GLB_SPI3_CLK_WIFIPLL_160M, 0);
#endif
        GLB_Set_DBI_CLK(ENABLE, GLB_DBI_CLK_MCU_MUXPLL_160M, 0);
        GLB_Set_I2C_CLK(ENABLE, GLB_I2C_CLK_XCLK, 0);
        GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 1);
        GLB_Set_DIG_CLK_Sel(GLB_DIG_CLK_XCLK);
        GLB_Set_DIG_512K_CLK(ENABLE, ENABLE, 0x4E);
        GLB_Set_PWM1_IO_Sel(GLB_PWM1_IO_SINGLE_END);
        GLB_Set_IR_CLK(ENABLE, GLB_IR_CLK_SRC_XCLK, bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_IR, 0) / 2000000 - 1);
        GLB_Set_CAM_CLK(ENABLE, GLB_CAM_CLK_WIFIPLL_96M, 3);
        GLB_Set_PEC_CLK(ENABLE, GLB_PEC_CLK_MUXPLL_160M, 0);
#if defined(CPU_MODEL_A0)
        GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_WIFIPLL_160M);
#else
        GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_WIFIPLL_320M);
#endif
        GLB_Set_USB_CLK_From_WIFIPLL(1);
        GLB_Swap_MCU_SPI_0_MOSI_With_MISO(0);
    }
}

#else

static void peripheral_clock_init_lp(void)
{
    uint32_t tmpVal = 0;

    /* clk gate,except DMA&CPU&UART0&SF&EMI&WIFI&EFUSE */
    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_CPU, 1); // ungate cpu
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_DMA, 1); // ungate dma
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_SEC, 1); // ungate sec
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_M_SDU, 1); // ungate sdu
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0, tmpVal);

    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_RSVD0, 1);   // ungate dma and gpio set&clr register's bclk
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EF_CTRL, 1); // ungate ef_ctrl
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_SF_CTRL, 1); // ungate sf_ctrl
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_DMA, 1);     // ungate dma
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_UART0, 1);  // ungate uart0
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_UART1, 1);  // ungate uart1
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_SEC_ENG, 1); // ungate sev_eng

    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_UART2, 1); // ungate uart2
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_I2C1, 1); // ungate i2c1
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_DBI, 1); // ungate dbi
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_CKS, 1); // ungate cks
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_IR, 1); // ungate ir
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_TIMER, 1); // ungate timer
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_PWM, 1); // ungate pwm
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_I2C, 1); // ungate i2c
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_SPI, 1); // ungate spi
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_TZ, 1); // ungate tz
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_SEC_DBG, 1); // ungate sec_dbg
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_GPIP, 1); // ungate gpip
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_RF_TOP, 1); // ungate rf_top
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_RSVD12, 1); // ungate audio
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1A_RSVD11, 1); // ungate i2s
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_RSVD13, 1); // ungate sdu&usb

    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

    tmpVal = 0;
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S2_WIFI, 1);         // ungate wifi
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_EMI_MISC, 1); // ungate emi_misc
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_PIO, 1);      // ungate pec

    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_DMA2, 1); // ungate dma2
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_EMAC, 1); // ungate emac
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_SDH, 1); // ungate sdh
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_AUDIO, 1); // ungate audio
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_MIX2, 1); // ungate mix2
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_USB, 1); // ungate usb
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_PSRAM_CTRL, 1); // ungate psram_ctrl
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S1_EXT_PSRAM0_CTRL, 1); // ungate psram0_ctrl
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S3_M1542, 1); // ungate m1542
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S3_M154, 1); // ungate m154
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S3_BT_BLE2, 1); // ungate bt_ble2
    // tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_CGEN_S3_BT_BLE, 1); // ungate bt_ble

    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpVal);

    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_XCLK, 0);

#ifdef CONFIG_BSP_USB
    PERIPHERAL_CLOCK_USB_ENABLE();
    GLB_Set_USB_CLK_From_WIFIPLL(1);
#endif

    PERIPHERAL_CLOCK_ADC_DAC_ENABLE();
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, 1);
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

void bl_show_component_version(void)
{
    extern uint8_t _version_info_section_start;
    extern uint8_t _version_info_section_end;
    char **version_str_p = NULL;
    bflb_verinf_t version = { 0 };

    puts("Version of used components:\r\n");
    for (version_str_p = (char **)&_version_info_section_start; version_str_p < (char **)&_version_info_section_end; version_str_p++) {
        puts("\tVersion: ");
        puts(*version_str_p);
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
    bflb_wo_uart_init(wo, 2000000, GPIO_PIN_11);
    bflb_wo_set_console(wo);
#else

#if defined(CPU_AP)
    board_ap_console_gpio_init();
#elif defined(CPU_NP)
    board_np_console_gpio_init();
#elif defined(CPU_LP)
    board_lp_console_gpio_init();
#endif

    struct bflb_uart_config_s cfg = { 0 };
    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;

#if defined(CPU_AP)
    uart = bflb_device_get_by_name("uart0");
#elif defined(CPU_NP)
    uart = bflb_device_get_by_name("uart1");
#elif defined(CPU_LP)
    uart = bflb_device_get_by_name("uart3");
#endif

    bflb_uart_init(uart, &cfg);
    bflb_uart_set_console(uart);
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

void np_board_main(void)
{
    uint32_t cycle_delta = 0;
    uint32_t cycle_l = 0;
    uint32_t cycle_h = 0;

    /* set mtimer clock */
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_WL_XCLK) / 1000000 - 1);

    printf("NP boot up\r\n");
    __WFI();
#if 0
#eut.dev_write_str(AON_DTEN_RC32M, 1)
#eut.dev_write(GLB_DBG_CFG0, 0x00004000)
#eut.dev_write(GLB_DBG_CFG1, 0x00004000)
#eut.dev_write(GLB_DBG_CFG2, 0x00004000)
#eut.dev_write(GLB_DBG_CFG3, 0x00004000)
#eut.dev_write(GLB_DBG_CFG4, 0x00000001)
#eut.dev_write(GLB_GPIO_CFG12, 0x00400f02)
#endif
    *(volatile uint32_t *)(0x2008F844) |= (1 << 27);
    *(volatile uint32_t *)(0x200002E0) = 0x00004000;
    *(volatile uint32_t *)(0x200002E4) = 0x00004000;
    *(volatile uint32_t *)(0x200002E8) = 0x00004000;
    *(volatile uint32_t *)(0x200002EC) = 0x00004000;
    *(volatile uint32_t *)(0x200002F0) = 0x00000001;
    *(volatile uint32_t *)(0x200008F4) = 0x00400f02;

    while (1) {
        *(volatile uint32_t *)(0x20010088) = 0x41;
        *(volatile uint32_t *)(0x20010088) = 0x41;
        printf("0x24001014(mtimer clock cfg):%08x\r\n", *(volatile uint32_t *)(0x24001014));
        printf("0x24000000(wl cpu clock cfg):%08x\r\n", *(volatile uint32_t *)(0x24000000));
        printf("0x24000004(wl cpu divid cfg):%08x\r\n", *(volatile uint32_t *)(0x24000004));
        printf("mtimer current counter:%lld\r\n", bflb_mtimer_get_time_us());
#if defined(CPU_MODEL_A0) || defined(CPU_LP)
        cycle_l = __get_MCYCLE();
        cycle_h = __get_MCYCLEH();
#else
        cycle_l = __RV_CSR_READ(CSR_MCYCLE);
        cycle_h = __RV_CSR_READ(CSR_MCYCLEH);
#endif

        printf("MCYCLE:%08x\r\n", cycle_l);
        printf("MCYCLEH:%08x\r\n", cycle_h);
        printf("Delata:%08x\r\n", cycle_l - cycle_delta);
        cycle_delta = cycle_l;
        printf("Core ID:%08x\r\n", GLB_Get_Core_Type());

        bflb_mtimer_delay_ms(100);
    }

    __WFI();
}

extern uint32_t __start;

void boot_up_np()
{
#ifndef CONFIG_BOOT_NP_ASYNC
    uint32_t ap_offset;
    /* AP & NP run in the same code */
    bflb_l1c_dcache_clean_all();
    ap_offset = bflb_sf_ctrl_get_flash_image_offset(0, 0);
    bflb_sf_ctrl_set_flash_image_offset(ap_offset, 1, 0);
#else
    /* AP & NP run in the different code */
    int ret;
    uint32_t boot_addr = 0, boot_len;
    ret = bflb_boot2_partition_addr_active("FW0", &boot_addr, &boot_len);
    if (ret != 0 || boot_addr == 0x0) {
        printf("ERROR: FW0 bootaddr not found ret:%d\r\n", ret);
        return;
    }
    extern uint32_t __dualcore_images__;
    uint32_t image_ap_1Kalign_size = *(uint32_t *)(&__dualcore_images__);
    boot_addr += image_ap_1Kalign_size + 0x1000; // FW0 header size is 0x1000

    printf("NP bootaddr get success 0x%08x\r\n", boot_addr);
    bflb_sf_ctrl_set_flash_image_offset(boot_addr, 1, 0);
#endif
    GLB_Set_CPU_Reset_Address(GLB_CORE_ID_NP, (uint32_t)&__start);
    GLB_Release_CPU(GLB_CORE_ID_NP);
}

__attribute__((weak)) const uint8_t gmini_sysData[4] = { 0 };
__attribute__((weak)) const uint32_t gmini_sysSize;

static void __attribute__((noinline)) boot_up_lp(uint32_t address)
{
    //Tzc_Sec_Set_CPU_Group(GLB_CORE_ID_LP, 0);
    if (gmini_sysData[0] != 0) {
        GLB_Release_Mini_Sys();
#if defined(CPU_MODEL_A0)
        GLB_Set_MINI_FCLK(ENABLE, GLB_MINI_FCLK_XCLK, 0);
#else
        GLB_Set_MINI_FCLK(ENABLE, GLB_MINI_FCLK_RC32M, 0);
#endif
        //GLB_Select_LPCPU_Jtag();
        arch_delay_us(10);

        memcpy((void *)address, gmini_sysData, gmini_sysSize);
        bflb_l1c_dcache_clean_all();
        printf("Copy gmini_sysData to 0x0,[1][31:0]=%08x,len=%d\r\n", *(uint32_t *)(address + 4), gmini_sysSize);

        GLB_Set_CPU_Reset_Address(GLB_CORE_ID_LP, (uint32_t)address);

        GLB_Release_CPU(GLB_CORE_ID_LP);
    }
}

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

#ifdef CONFIG_PSRAM
#if defined(CPU_AP)
    /* psram init */
    if (board_psram_x8_init() != SUCCESS) {
        puts("psram init fail !!!\r\n");
        while (1) {}
    }
#endif

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
    if (&__psram_data_end__ - &__psram_data_start__ > 0 || &__psram_noinit_end__ - &__psram_noinit_start__ > 0) {
        puts("psram data already exists, please enable CONFIG_PSRAM\r\n");
        while (1) {}
    }

    /* ram info dump */
    printf("dynamic memory init success\r\n"
           "  ocram heap size: %d Kbyte \r\n",
           ((size_t)&__HeapLimit - (size_t)&__HeapBase) / 1024);
#endif
}

#if defined(CPU_AP)
void board_init(void)
{
    int ret = -1;
    uintptr_t flag;
    uint32_t xtal_value = 0;

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
    /* Enable wifi irq */
    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_TO_CPU_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_TO_CPU_IRQn);
#endif
#ifdef CONFIG_HIGH_ISR_STACK
    bflb_wfa_init();
#endif

    /* console init (uart or wo) */
    console_init();

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

    putreg32(0xffffffff, GLB_BASE + GLB_CGEN_CFG1_OFFSET);
    putreg32(0xffffffff, GLB_BASE + GLB_CGEN_CFG2_OFFSET);

    printf("uart  sig1:%08x, sig2:%08x\r\n", getreg32(GLB_BASE + GLB_UART_CFG1_OFFSET), getreg32(GLB_BASE + GLB_UART_CFG2_OFFSET));
    printf("clock gen1:%08x, gen2:%08x\r\n", getreg32(GLB_BASE + GLB_CGEN_CFG1_OFFSET), getreg32(GLB_BASE + GLB_CGEN_CFG2_OFFSET));
    HBN_Get_Xtal_Value(&xtal_value);
    printf("xtal:%dHz(%s)\r\n", xtal_value, ((getreg32(AON_BASE + AON_XTAL_CFG_OFFSET) >> 3) & 0x01) ? "oscillator" : "crystal");

    log_start();

#if (defined(CONFIG_LUA) || defined(CONFIG_BFLB_LOG) || defined(CONFIG_FATFS))
    rtc = bflb_device_get_by_name("rtc");
#endif

#ifdef CONFIG_MBEDTLS
    extern void bflb_sec_mutex_init(void);
    bflb_sec_mutex_init();
#endif

#ifdef CONFIG_BFLB_MTD
    bflb_mtd_init();
#endif
#ifdef CONFIG_DUALCORE_DISABLE
#else
    boot_up_np();
#endif

    if (GLB_Get_Mini_System_Status() == SET) { //first read the value of the register address,it release
        GLB_Halt_Mini_Sys();
        GLB_Halt_CPU(GLB_CORE_ID_LP);
        boot_up_lp(0x0);
    } else { //After release, the value reads 0 and is no longer reset
        printf("already release lp\r\n");
    }
    bflb_irq_restore(flag);

    printf("board init done\r\n");
    printf("===========================\r\n");
}
#elif defined(CPU_NP)
void board_init(void)
{
    uintptr_t flag;

    flag = bflb_irq_save();

    /* system clock */
    system_clock_init();
    /* peripherals clock */
    peripheral_clock_init();

    bflb_irq_initialize();

#ifdef CONFIG_WIFI6
    /* Enable wifi irq */
    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_TO_CPU_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_TO_CPU_IRQn);
#endif

    console_init();

    /* ram and heap init (including psram) */
    ram_heap_init();

    bl_show_log();

    printf("uart  sig1:%08x, sig2:%08x\r\n", getreg32(GLB_BASE + GLB_UART_CFG1_OFFSET), getreg32(GLB_BASE + GLB_UART_CFG2_OFFSET));
    printf("clock gen1:%08x, gen2:%08x\r\n", getreg32(GLB_BASE + GLB_CGEN_CFG1_OFFSET), getreg32(GLB_BASE + GLB_CGEN_CFG2_OFFSET));
    log_start();

    bflb_irq_restore(flag);

    printf("board init done\r\n");
    printf("===========================\r\n");
}
#elif defined(CPU_LP)
void board_init(void)
{
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);

    bflb_irq_initialize();

    /* ram heap init */

    console_init();

    /* heap init */
    heap_init();

    bl_show_log();

    //printf("lp does not use memheap due to little ram \r\n");

    printf("uart  sig1:%08x, sig2:%08x\r\n", getreg32(GLB_BASE + GLB_UART_CFG1_OFFSET), getreg32(GLB_BASE + GLB_UART_CFG2_OFFSET));
    printf("clock gen1:%08x, gen2:%08x\r\n", getreg32(GLB_BASE + GLB_CGEN_CFG1_OFFSET), getreg32(GLB_BASE + GLB_CGEN_CFG2_OFFSET));

    log_start();
}
#endif

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
            printf("unkown error: %d\r\n", wakeup_mode);
        }
    }
}

static void test_wakeup_io_callback(uint64_t wake_up_io_bits)
{
    printf("io wakeup bits 0x%llX\r\n", wake_up_io_bits);

    /* wakeup io status dump */
    for (uint8_t i = 0; i < BL_LP_WAKEUP_IO_MAX_NUM; i++) {
        test_io_wakeup_status(i);
    }
}

void cmd_io_test(char *buf, int len, int argc, char **argv)
{
    static bl_lp_io_cfg_t lp_wake_io_cfg = {
        /* input enable, use @ref BL_LP_IO_INPUT_EN */
        .io_0_15_ie = BL_LP_IO_INPUT_ENABLE,
        .io_16_ie = BL_LP_IO_INPUT_ENABLE,
        .io_17_ie = BL_LP_IO_INPUT_ENABLE,
        .io_18_ie = BL_LP_IO_INPUT_ENABLE,
        .io_19_ie = BL_LP_IO_INPUT_ENABLE,
        .io_20_34_ie = BL_LP_IO_INPUT_ENABLE,
        /* trigger mode */
        .io_0_7_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE,          /* use @ref BL_LP_PDS_IO_TRIG */
        .io_8_15_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_HIGH_LEVEL,           /* use @ref BL_LP_PDS_IO_TRIG */
        .io_16_19_aon_trig_mode = BL_LP_AON_IO_TRIG_SYNC_RISING_FALLING_EDGE, /* aon io, use @ref BL_LP_AON_IO_TRIG, full mode support */
        .io_20_27_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE,        /* use @ref BL_LP_PDS_IO_TRIG */
        .io_28_34_pds_trig_mode = BL_LP_PDS_IO_TRIG_SYNC_FALLING_EDGE,        /* use @ref BL_LP_PDS_IO_TRIG */
        /* resistors */
        .io_0_15_res = BL_LP_IO_RES_PULL_UP,
        .io_16_res = BL_LP_IO_RES_NONE,
        .io_17_res = BL_LP_IO_RES_NONE,
        .io_18_res = BL_LP_IO_RES_PULL_UP,
        .io_19_res = BL_LP_IO_RES_PULL_UP,
        .io_20_34_res = BL_LP_IO_RES_PULL_DOWN,
        /* wake up unmask */
        .io_wakeup_unmask = 0,
    };

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

    bl_lp_io_wakeup_cfg(&lp_wake_io_cfg);

    /* register io wakeup callback */
    bl_lp_wakeup_io_int_register(test_wakeup_io_callback);
}

static void test_acomp_wakeup_status(uint8_t acomp_num)
{
    int wakeup_mode;

    wakeup_mode = bl_lp_wakeup_acomp_get_mode(acomp_num);

    if (wakeup_mode) {
        printf("ACOMP %d wakeup: ", acomp_num);
        if (wakeup_mode == BL_LP_ACOMP_WAKEUP_MODE_FALLING) {
            printf("edge falling\r\n");
        } else if (wakeup_mode == BL_LP_ACOMP_WAKEUP_MODE_RISING) {
            printf("edge rising\r\n");
        } else {
            printf("unkown error: %d\r\n", wakeup_mode);
        }
    }
}

static void test_wakeup_acomp_callback(uint32_t wake_up_acomp_bits)
{
    printf("acomp wakeup bits 0x%02X\r\n", wake_up_acomp_bits);

    for (uint8_t i = 0; i < BL_LP_WAKEUP_ACOMP_MAX_NUM; i++) {
        test_acomp_wakeup_status(i);
    }
}

void cmd_acomp_test(char *buf, int len, int argc, char **argv)
{
    static bl_lp_acomp_cfg_t lp_wake_acomp_cfg = {
        /* input enable, use @ref BL_LP_ACOMP_EN */
        .acomp0_en = BL_LP_ACOMP_ENABLE,
        .acomp1_en = BL_LP_ACOMP_ENABLE,

        /* Map to pins num, range: 2, 3, 10, 12, 13, 14, 19 */
        .acomp0_io_num = 13,
        .acomp1_io_num = 14,

        /* trigger mode, use @ref BL_LP_ACOMP_TRIG  */
        .acomp0_trig_mode = BL_LP_ACOMP_TRIG_EDGE_FALLING,
        .acomp1_trig_mode = BL_LP_ACOMP_TRIG_EDGE_FALLING_RISING,
    };

    bl_lp_acomp_wakeup_cfg(&lp_wake_acomp_cfg);

    /* register acomp wakeup callback */
    bl_lp_wakeup_acomp_int_register(test_wakeup_acomp_callback);
}
SHELL_CMD_EXPORT_ALIAS(cmd_io_test, io_test, cmd io_test);
SHELL_CMD_EXPORT_ALIAS(cmd_acomp_test, acomp_test, cmd acomp_test);
#endif /* LP_APP */

#endif /* CONFIG_SHELL */

#endif /* __has_include("board_overlay.h") */
