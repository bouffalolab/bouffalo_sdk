#include "bl616l_glb.h"
#include "bl616l_pds.h"
#include "bl616l_hbn.h"
#include "bl616l_aon.h"
#include "bl616l_pm.h"
#include "bl616l_xip_recovery.h"

#include "bflb_core.h"
#include "bflb_l1c.h"
#include "bflb_irq.h"
#include "bflb_clock.h"
#include "bflb_rtc.h"
#include "bflb_uart.h"

#include "rv_hart.h"
#include "rv_pmp.h"

#include "board.h"


static uint32_t cpu_sleep_reg_save[32];

void ATTR_TCM_SECTION lp_fw_save_cpu_para(uint32_t save_addr)
{
    __asm__ volatile(
        "sw x0,  0(%[addr])\n\t"
        "sw x1,  4(%[addr])\n\t"
        "sw x2,  8(%[addr])\n\t"
        "sw x3,  12(%[addr])\n\t"
        "sw x4,  16(%[addr])\n\t"
        "sw x5,  20(%[addr])\n\t"
        "sw x6,  24(%[addr])\n\t"
        "sw x7,  28(%[addr])\n\t"
        "sw x8,  32(%[addr])\n\t"
        "sw x9,  36(%[addr])\n\t"
        "sw x10, 40(%[addr])\n\t"
        "sw x11, 44(%[addr])\n\t"
        "sw x12, 48(%[addr])\n\t"
        "sw x13, 52(%[addr])\n\t"
        "sw x14, 56(%[addr])\n\t"
        "sw x15, 60(%[addr])\n\t"
        "sw x16, 64(%[addr])\n\t"
        "sw x17, 68(%[addr])\n\t"
        "sw x18, 72(%[addr])\n\t"
        "sw x19, 76(%[addr])\n\t"
        "sw x20, 80(%[addr])\n\t"
        "sw x21, 84(%[addr])\n\t"
        "sw x22, 88(%[addr])\n\t"
        "sw x23, 92(%[addr])\n\t"
        "sw x24, 96(%[addr])\n\t"
        "sw x25, 100(%[addr])\n\t"
        "sw x26, 104(%[addr])\n\t"
        "sw x27, 108(%[addr])\n\t"
        "sw x28, 112(%[addr])\n\t"
        "sw x29, 116(%[addr])\n\t"
        "sw x30, 120(%[addr])\n\t"
        "sw x31, 124(%[addr])\n\t"
        :                       /* OutputOperands  */
        : [addr] "r"(save_addr) /* InputOperands */
        : "memory");            /* Clobbers */
}

void ATTR_TCM_SECTION lp_fw_restore_cpu_para(uint32_t save_addr)
{
    __asm__ volatile(
        "lw x0,  0(%[addr])\n\t"
        "lw x1,  4(%[addr])\n\t"
        "lw x2,  8(%[addr])\n\t"
        "lw x3,  12(%[addr])\n\t"
        "lw x4,  16(%[addr])\n\t"
        "lw x5,  20(%[addr])\n\t"
        "lw x6,  24(%[addr])\n\t"
        "lw x7,  28(%[addr])\n\t"
        "lw x8,  32(%[addr])\n\t"
        "lw x9,  36(%[addr])\n\t"
        "lw x10, 40(%[addr])\n\t"
        "lw x11, 44(%[addr])\n\t"
        "lw x12, 48(%[addr])\n\t"
        "lw x13, 52(%[addr])\n\t"
        "lw x14, 56(%[addr])\n\t"
        "lw x15, 60(%[addr])\n\t"
        "lw x16, 64(%[addr])\n\t"
        "lw x17, 68(%[addr])\n\t"
        "lw x18, 72(%[addr])\n\t"
        "lw x19, 76(%[addr])\n\t"
        "lw x20, 80(%[addr])\n\t"
        "lw x21, 84(%[addr])\n\t"
        "lw x22, 88(%[addr])\n\t"
        "lw x23, 92(%[addr])\n\t"
        "lw x24, 96(%[addr])\n\t"
        "lw x25, 100(%[addr])\n\t"
        "lw x26, 104(%[addr])\n\t"
        "lw x27, 108(%[addr])\n\t"
        "lw x28, 112(%[addr])\n\t"
        "lw x29, 116(%[addr])\n\t"
        "lw x30, 120(%[addr])\n\t"
        "lw x31, 124(%[addr])\n\t"

        :                       /* OutputOperands  */
        : [addr] "r"(save_addr) /* InputOperands */
        : "memory"              /* Clobbers */
    );
}

static void ATTR_TCM_SECTION lp_system_init(void)
{
    /* enable mstatus FS */
    uint32_t mstatus = __get_MSTATUS();
    mstatus |= (1 << 13);
    __set_MSTATUS(mstatus);

    /* enable mxstatus THEADISAEE */
    uint32_t mxstatus = __get_MXSTATUS();
    mxstatus |= (1 << 22);
    /* enable mxstatus MM */
    mxstatus |= (1 << 15);
    __set_MXSTATUS(mxstatus);

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    /* Every interrupt should be clear by software*/
    for (int i = 0; i < IRQn_LAST; i++) {
        CLIC->CLICINT[i].IE = 0;
        CLIC->CLICINT[i].IP = 0;
#ifdef CONFIG_IRQ_USE_VECTOR
        CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
#else
        CLIC->CLICINT[i].ATTR = 0; /* use no vector interrupt */
#endif
    }

    /* tspend interrupt will be clear auto*/
    /* tspend use positive interrupt */
#ifdef CONFIG_IRQ_USE_VECTOR
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x3;
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x3;
#else
    CLIC->CLICINT[MSOFT_IRQn].ATTR = 0x2;
    CLIC->CLICINT[SDU_SOFT_RST_IRQn].ATTR = 0x2;
#endif

    csi_dcache_enable();
    csi_icache_enable();

    /* disable mexstatus SPUSHEN and SPSWAPEN for ipush/ipop*/
    uint32_t mexstatus = __get_MEXSTATUS();
    mexstatus &= ~(0x3 << 16);
    __set_MEXSTATUS(mexstatus);

    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;

    uint32_t tmpVal = 0;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SRAM_CFG3);

    if (em_size == 0) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x00); // GLB_WRAM160KB_EM0KB
    } else if (em_size == 16 * 1024) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x01); // GLB_WRAM144KB_EM16KB
    } else if (em_size == 32 * 1024) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x03); // GLB_WRAM128KB_EM32KB
    } else {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, 0x00); // GLB_WRAM160KB_EM0KB
    }
    BL_WR_REG(GLB_BASE, GLB_SRAM_CFG3, tmpVal);
}

static void ATTR_TCM_SECTION lp_system_clock_init(void)
{
    /* wifipll */
    GLB_Power_On_XTAL_And_PLL_CLK(GLB_XTAL_40M, GLB_PLL_WIFIPLL);
    GLB_Set_MCU_System_CLK(GLB_MCU_SYS_CLK_TOP_WIFIPLL_320M);
    HBN_Set_MCU_XCLK_Sel(HBN_MCU_XCLK_XTAL);
    CPU_Set_MTimer_CLK(ENABLE, BL_MTIMER_SOURCE_CLOCK_MCU_XCLK, Clock_System_Clock_Get(BL_SYSTEM_CLOCK_XCLK) / 1000000 - 1);
}

static void ATTR_TCM_SECTION lp_peripheral_clock_init(void)
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

    GLB_Set_PKA_CLK_Sel(GLB_PKA_CLK_WIFIPLL_320M);
#ifdef CONFIG_BSP_SDH_SDCARD
    PERIPHERAL_CLOCK_SDH_ENABLE();
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_SDH);
#endif

    GLB_Swap_MCU_SPI_0_MOSI_With_MISO(0);
}

static void lp_console_init()
{
#ifdef CONFIG_CONSOLE_WO
    wo = bflb_device_get_by_name("wo");
    bflb_wo_uart_init(wo, 2000000, GPIO_PIN_21);
    bflb_wo_set_console(wo);
#else

    struct bflb_device_s *gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_uart_init(gpio, GPIO_PIN_34, GPIO_UART_FUNC_UART0_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_35, GPIO_UART_FUNC_UART0_RX);

    struct bflb_uart_config_s cfg;
    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;

    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");

    bflb_uart_init(uart0, &cfg);

    extern void bflb_uart_set_console(struct bflb_device_s * dev);
    bflb_uart_set_console(uart0);
#endif
}

/* 注意: 这个函数从 bootrom 跳转来, SP 无效的, 所以不可使用 Stack */
void ATTR_TCM_SECTION pds_cpu_wakeup_cb(void)
{
    /* cpu return */
    lp_fw_restore_cpu_para((uint32_t)cpu_sleep_reg_save);
}

void ATTR_TCM_SECTION pds_wakeup_recover(void)
{
    __asm__ volatile(
    /* mtvec: for all exceptions and non-vector mode IRQs */
#if defined(CONFIG_FREERTOS)
        "la a0, freertos_risc_v_trap_handler\n\t"
#else
        "la a0, default_trap_handler\n\t"
#endif
        "ori    a0, a0, 3\n\t"
        "csrw   mtvec, a0\n\t"

#ifdef CONFIG_IRQ_USE_VECTOR
        /* mtvt: for all vector mode IRQs */
        "la      a0, __Vectors\n\t"
        "csrw    mtvt, a0\n\t"
#endif

#if defined(__riscv_flen)
        /* FP: initial state */
        "csrr    t0, mstatus\n\t"
        "li      t1, ~0x6000\n\t"
        "and     t0, t0, t1\n\t"
        "li      t1, 0x2000\n\t"
        "or      t0, t0, t1\n\t"
        "csrw    mstatus, t0\n\t"
    /* csrwi   fcsr, 0 */
#endif
    );

#if PM_PDS_LDO18IO_POWER_DOWN
    GLB_Power_Up_Ldo18ioVout();
#endif

    /* system init (SystemInit) */
    lp_system_init();

    /* system_clock_init (system_clock_init) */
    lp_system_clock_init();

    /* flash recovery */
    bl_lp_xip_recovery();

    /* peripheral_clock_init (peripheral_clock_init) */
    lp_peripheral_clock_init();

    /* irq init */
    bflb_irq_initialize();

    /* uart init (console_init) */
    lp_console_init();
}

void ATTR_TCM_SECTION pds_enter(uint32_t pds_level, uint32_t sleep_time)
{
    volatile uintptr_t irq_flag;
    volatile bool enert_flag;

    bl_lp_xip_para_save();

    /* disable irq */
    irq_flag = bflb_irq_save();

    /* cpu save */
    enert_flag = true;
    lp_fw_save_cpu_para((uint32_t)cpu_sleep_reg_save);

    if (enert_flag) {
        enert_flag = false;

        pm_set_wakeup_callback(pds_cpu_wakeup_cb);

        /* clean all D-Cache */
        bflb_l1c_dcache_clean_all();

        // printf("pds_enter\r\n");

        pm_pds_mode_enter(pds_level, sleep_time);

    } else {
        /* wakeup, recover */
        pds_wakeup_recover();
        

        // printf("pds wakeup, return...\r\n");
    
    }

    /* enable irq */
    bflb_irq_restore(irq_flag);
}
