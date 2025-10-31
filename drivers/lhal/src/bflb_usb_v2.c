#include "bflb_core.h"
#include "usbd_core.h"
#include "usbh_core.h"
#include "hardware/usb_v2_reg.h"

#ifndef CONFIG_USB_EHCI_HCOR_RESERVED_DISABLE
#error "usb host must enable CONFIG_USB_EHCI_HCOR_RESERVED_DISABLE"
#endif

/* select only one mode */
// #define CONFIG_USB_PINGPONG_ENABLE
// #define CONFIG_USB_TRIPLE_ENABLE

#if defined(BL616) || defined(BL606P) || defined(BL808)
#define BFLB_USB_BASE           ((uint32_t)0x20072000)
#define BFLB_PDS_BASE           ((uint32_t)0x2000E000)
#define BFLB_USB_IRQ_NUM        37
#define PDS_USB_CTL_OFFSET      (0x500) /* usb_ctl */
#define PDS_USB_PHY_CTRL_OFFSET (0x504) /* usb_phy_ctrl */
#elif defined(BL616L)
#define BFLB_USB_BASE            ((uint32_t)0x20072000)
#define BFLB_PDS_BASE            ((uint32_t)0x2000E000)
#define BFLB_USB_IRQ_NUM         37
#define PDS_USB_CTL_OFFSET       (0x500) /* usb_ctl */
#define PDS_USB_PHY0_CTRL_OFFSET (0x504) /* usb_phy0_ctrl */
#define PDS_USB_PHY1_CTRL_OFFSET (0x508) /* usb_phy1_ctrl */
#define PDS_USB_PHY2_CTRL_OFFSET (0x50C) /* usb_phy2_ctrl */
#define PDS_USB_PHY3_CTRL_OFFSET (0x510) /* usb_phy3_ctrl */
#elif defined(BL616D)
#define BFLB_USB_BASE            ((uint32_t)0x20087000)
#define BFLB_PDS_BASE            ((uint32_t)0x2008E000)
#define BFLB_USB_IRQ_NUM         19
#define PDS_USB_CTL_OFFSET       (0x500) /* usb_ctl */
#define PDS_USB_PHY0_CTRL_OFFSET (0x504) /* usb_phy0_ctrl */
#define PDS_USB_PHY1_CTRL_OFFSET (0x508) /* usb_phy1_ctrl */
#define PDS_USB_PHY2_CTRL_OFFSET (0x50C) /* usb_phy2_ctrl */
#define PDS_USB_PHY3_CTRL_OFFSET (0x510) /* usb_phy3_ctrl */
#endif

#if defined(BL616) || defined(BL606P) || defined(BL808)
/* 0x500 : usb_ctl */
#define PDS_REG_USB_SW_RST_N         (1 << 0U)
#define PDS_REG_USB_EXT_SUSP_N       (1 << 1U)
#define PDS_REG_USB_WAKEUP           (1 << 2U)
#define PDS_REG_USB_L1_WAKEUP        (1 << 3U)
#define PDS_REG_USB_DRVBUS_POL       (1 << 4U)
#define PDS_REG_USB_IDDIG            (1 << 5U)

/* 0x504 : usb_phy_ctrl */
#define PDS_REG_USB_PHY_PONRST       (1 << 0U)
#define PDS_REG_USB_PHY_OSCOUTEN     (1 << 1U)
#define PDS_REG_USB_PHY_XTLSEL_SHIFT (2U)
#define PDS_REG_USB_PHY_XTLSEL_MASK  (0x3 << PDS_REG_USB_PHY_XTLSEL_SHIFT)
#define PDS_REG_USB_PHY_OUTCLKSEL    (1 << 4U)
#define PDS_REG_USB_PHY_PLLALIV      (1 << 5U)
#define PDS_REG_PU_USB20_PSW         (1 << 6U)
#endif

#if defined(BL616D) || defined(BL616L)
/* 0x500 : usb_ctl */
#define PDS_REG_USB_SW_RST_N (1 << 0U)
#if defined(BL616D)
#define PDS_REG_USB_IDDIG (1 << 1U)
#endif
#define PDS_REG_USB_L1_WAKEUP            (1 << 2U)
#define PDS_REG_USB_WAKEUP               (1 << 3U)
#define PDS_REG_USB_VBUS_VALID_FORCE     (1 << 4U)
#define PDS_REG_USB_VBUS_VALID_FROM_PHY  (1 << 5U)
#define PDS_REG_USB_VBUS_VALID_FROM_GPIO (1 << 6U)
#define PDS_REG_USB_LP_MODE_UCLK_DISABLE (1 << 7U)
#define PDS_REG_USB_EXIT_SUSPEND_N       (1 << 8U)
#if defined(BL616L)
#define PDS_REG_USB_IDDIG (1 << 16U)
#endif
#define PDS_REG_PU_USB20_PSW              (1 << 31U)
/* 0x508 : usb_phy1_ctrl */
#define PDS_REG_USB_PHY_SUSPENDM0_USE_REG (1 << 0U)
#define PDS_REG_USB_PHY_SIDDQ             (1 << 28U)
/* 0x50C : usb_phy2_ctrl */
#define PDS_REG_USB_PHY_VBUS_VLD_EXT_SEL0 (1 << 18U)
/* 0x510 : usb_phy3_ctrl */
#define PDS_REG_USB_PHY_POR               (1 << 31U)
#endif

#define USB_SOF_TIMER_MASK_AFTER_RESET_HS (0x44C)
#define USB_SOF_TIMER_MASK_AFTER_RESET_FS (0x2710)

extern void USBD_IRQHandler(uint8_t busid);

void USBD_IRQ(int irq, void *arg)
{
    USBD_IRQHandler(0);
}

static void bflb_usb_phy_init(void)
{
    uint32_t regval;

#if defined(BL616) || defined(BL606P) || defined(BL808)
    /* USB_PHY_CTRL[3:2] reg_usb_phy_xtlsel=0                             */
    /* 2000e504 = 0x40; #100; USB_PHY_CTRL[6] reg_pu_usb20_psw=1 (VCC33A) */
    /* 2000e504 = 0x41; #500; USB_PHY_CTRL[0] reg_usb_phy_ponrst=1        */
    /* 2000e500 = 0x20; #100; USB_CTL[0] reg_usb_sw_rst_n=0               */
    /* 2000e500 = 0x22; #500; USB_CTL[1] reg_usb_ext_susp_n=1             */
    /* 2000e500 = 0x23; #100; USB_CTL[0] reg_usb_sw_rst_n=1               */
    /* #1.2ms; wait UCLK                                                  */
    /* wait(soc616_b0.usb_uclk);                                          */

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    regval &= ~PDS_REG_USB_PHY_XTLSEL_MASK;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    regval |= PDS_REG_PU_USB20_PSW;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    regval |= PDS_REG_USB_PHY_PONRST;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    /* greater than 5T */
    bflb_mtimer_delay_us(1);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval &= ~PDS_REG_USB_SW_RST_N;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    /* greater than 5T */
    bflb_mtimer_delay_us(1);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval |= PDS_REG_USB_EXT_SUSP_N;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    /* wait UCLK 1.2ms */
    bflb_mtimer_delay_ms(3);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval |= PDS_REG_USB_SW_RST_N;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    bflb_mtimer_delay_ms(2);

#elif defined(BL616D) || defined(BL616L)

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval |= PDS_REG_PU_USB20_PSW;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    bflb_mtimer_delay_us(50);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval &= ~PDS_REG_USB_SW_RST_N;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    /* wait XXX */
    bflb_mtimer_delay_us(50);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval |= PDS_REG_USB_SW_RST_N;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    bflb_mtimer_delay_us(50);

    /* power on usb phy analog blocks*/
    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY1_CTRL_OFFSET);
    regval &= ~PDS_REG_USB_PHY_SIDDQ;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY1_CTRL_OFFSET);

    bflb_mtimer_delay_us(50);

    /* reset usb macro */
    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY3_CTRL_OFFSET);
    regval |= PDS_REG_USB_PHY_POR;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY3_CTRL_OFFSET);

    bflb_mtimer_delay_us(50);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY3_CTRL_OFFSET);
    regval &= ~PDS_REG_USB_PHY_POR;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY3_CTRL_OFFSET);

    bflb_mtimer_delay_us(50);

    /* External VBUS Valid Select */
    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY2_CTRL_OFFSET);
    regval |= PDS_REG_USB_PHY_VBUS_VLD_EXT_SEL0;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY2_CTRL_OFFSET);

    /* SW force mode to assert VBUS valid signal */
    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval &= ~PDS_REG_USB_VBUS_VALID_FROM_PHY;
    regval &= ~PDS_REG_USB_VBUS_VALID_FROM_GPIO;
    regval |= PDS_REG_USB_VBUS_VALID_FORCE;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

#endif
}

extern void USBH_IRQHandler(uint8_t busid);

void USBH_IRQ(int irq, void *arg)
{
    USBH_IRQHandler(0);
}

void usb_hc_low_level_init(uint8_t busid)
{
    uint32_t regval;

    bflb_usb_phy_init();

    bflb_irq_attach(BFLB_USB_IRQ_NUM, USBH_IRQ, NULL);
    bflb_irq_enable(BFLB_USB_IRQ_NUM);

    /* enable device-A for host */
    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval &= ~PDS_REG_USB_IDDIG;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_OTG_CSR_OFFSET);
    regval |= USB_A_BUS_DROP_HOV;
    regval &= ~USB_A_BUS_REQ_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_OTG_CSR_OFFSET);

    bflb_mtimer_delay_ms(10);

    /* enable vbus and bus control */
    regval = getreg32(BFLB_USB_BASE + USB_OTG_CSR_OFFSET);
    regval &= ~USB_A_BUS_DROP_HOV;
    regval |= USB_A_BUS_REQ_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_OTG_CSR_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_GLB_INT_OFFSET);
    regval |= USB_MDEV_INT;
    regval |= USB_MOTG_INT;
    regval &= ~USB_MHC_INT;
    putreg32(regval, BFLB_USB_BASE + USB_GLB_INT_OFFSET);
}

void usb_hc_low_level_deinit(uint8_t busid)
{
}

uint8_t usbh_get_port_speed(const uint8_t port)
{
    uint8_t speed = 3;

    speed = (getreg32(BFLB_USB_BASE + USB_OTG_CSR_OFFSET) & USB_SPD_TYP_HOV_POV_MASK) >> USB_SPD_TYP_HOV_POV_SHIFT;

    if (speed == 0) {
        return USB_SPEED_FULL;
    } else if (speed == 1) {
        return USB_SPEED_LOW;
    } else if (speed == 2) {
        return USB_SPEED_HIGH;
    }
    return USB_SPEED_HIGH;
}

#define USB_FIFO_F0 0
#define USB_FIFO_F1 1
#define USB_FIFO_F2 2
#define USB_FIFO_F3 3
#if defined(BL616D) || defined(BL616L)
#define USB_FIFO_F4 4
#define USB_FIFO_F5 5
#define USB_FIFO_F6 6
#define USB_FIFO_F7 7
#endif
#define USB_FIFO_CXF          0xff

#define USB_FIFO_DIR_OUT      0
#define USB_FIFO_DIR_IN       1
#define USB_FIFO_DIR_BID      2

#define USB_VDMA_DIR_FIFO2MEM 0
#define USB_VDMA_DIR_MEM2FIFO 1

#if defined(BL616) || defined(BL606P) || defined(BL808)
#define USB_MAX_EP_WITH_EP0     5
#define USB_MAX_EP_EXCLUDE_EP0  4
#define USB_NUM_BIDIR_ENDPOINTS 5
#endif
#if defined(BL616D) || defined(BL616L)
#define USB_MAX_EP_WITH_EP0     9
#define USB_MAX_EP_EXCLUDE_EP0  8
#define USB_NUM_BIDIR_ENDPOINTS 9
#endif

/* Endpoint state */
struct bl_ep_state {
    uint16_t ep_mps;    /* Endpoint max packet size */
    uint8_t ep_type;    /* Endpoint type */
    uint8_t ep_stalled; /* Endpoint stall flag */
    uint8_t ep_enable;  /* Endpoint enable */
    bool ep_active;
    uint8_t *xfer_buf;
    uint32_t xfer_len;
    uint32_t actual_xfer_len;
};

struct bl_udc {
    struct bl_ep_state in_ep[USB_NUM_BIDIR_ENDPOINTS];  /*!< IN endpoint parameters             */
    struct bl_ep_state out_ep[USB_NUM_BIDIR_ENDPOINTS]; /*!< OUT endpoint parameters            */
} g_bl_udc;

static USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t g_setup_buffer[8];

static void bflb_usb_reset_fifo(uint8_t fifo)
{
    uint32_t regval;

    if (fifo == USB_FIFO_CXF) {
        regval = getreg32(BFLB_USB_BASE + USB_DEV_CXCFE_OFFSET);
        regval |= USB_CX_CLR;
        putreg32(regval, BFLB_USB_BASE + USB_DEV_CXCFE_OFFSET);
    } else {
        if (fifo < 4) {
            regval = getreg32(BFLB_USB_BASE + USB_DEV_FIBC0_OFFSET + 4 * fifo);
            regval |= USB_FFRST0_HOV;
            putreg32(regval, BFLB_USB_BASE + USB_DEV_FIBC0_OFFSET + 4 * fifo);
        } else {
            regval = getreg32(BFLB_USB_BASE + USB_DEV_FCFG4_OFFSET + 4 * (fifo - 4));
            regval |= USB_FFRST0_HOV;
            putreg32(regval, BFLB_USB_BASE + USB_DEV_FCFG4_OFFSET + 4 * (fifo - 4));
        }
    }
}

void bflb_usb_get_setup_packet(uint32_t setup[2])
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DMA_TFN_OFFSET);
    regval |= USB_ACC_CXF_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_DMA_TFN_OFFSET);

    setup[0] = getreg32(BFLB_USB_BASE + USB_DMA_CPS3_OFFSET);
    setup[1] = getreg32(BFLB_USB_BASE + USB_DMA_CPS3_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_DMA_TFN_OFFSET);
    regval &= ~USB_ACC_CXF_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_DMA_TFN_OFFSET);
}

static void bflb_usb_set_ep_fifomap(uint8_t ep_idx, uint8_t fifo)
{
    uint32_t regval;
    uint32_t regaddr;

    if (ep_idx < 5) {
        regaddr = BFLB_USB_BASE + USB_DEV_EPMAP0_OFFSET;
    } else {
        ep_idx = ep_idx - 4;
        regaddr = BFLB_USB_BASE + USB_DEV_EPMAP1_OFFSET;
    }

    regval = getreg32(regaddr);
    regval &= ~(0xff << ((ep_idx - 1) * 8));
    regval |= (fifo << ((ep_idx - 1) * 8));
    regval |= (fifo << ((ep_idx - 1) * 8 + 4));

    putreg32(regval, regaddr);
}

static void bflb_usb_set_fifo_epmap(uint8_t fifo, uint8_t ep_idx, uint8_t dir)
{
    uint32_t regval;
    uint32_t regaddr;

    if (fifo < 4) {
        regaddr = BFLB_USB_BASE + USB_DEV_FMAP_OFFSET;
    } else {
        fifo = fifo - 4;
        regaddr = BFLB_USB_BASE + USB_DEV_FMAP2_OFFSET;
    }

    regval = getreg32(regaddr);
    regval &= ~(0x3f << (fifo * 8));
    regval |= (ep_idx << (fifo * 8));
    regval |= (dir << (fifo * 8 + 4));

    putreg32(regval, regaddr);
}

static void bflb_usb_set_outep_mps(uint8_t ep_idx, uint16_t ep_mps)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_OUTMPS1_OFFSET + (ep_idx - 1) * 4);
    regval &= ~USB_MAXPS_OEP1_MASK;
    regval |= ep_mps;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_OUTMPS1_OFFSET + (ep_idx - 1) * 4);
}

int bflb_usb_get_inep_mps(uint8_t ep_idx)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);

    return regval &= USB_MAXPS_IEP1_MASK;
}

int bflb_usb_get_outep_mps(uint8_t ep_idx)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_OUTMPS1_OFFSET + (ep_idx - 1) * 4);

    return regval &= USB_MAXPS_OEP1_MASK;
}

static void bflb_usb_set_inep_mps(uint8_t ep_idx, uint16_t ep_mps)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
    regval &= ~USB_MAXPS_IEP1_MASK;
    regval |= ep_mps;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
}

static uint8_t bflb_usb_get_fifo_ep(uint8_t fifo)
{
    uint32_t regval;

    if (fifo < 4) {
        regval = (getreg32(BFLB_USB_BASE + USB_DEV_FMAP_OFFSET) & (0xf << (fifo * 8)));
        regval >>= (fifo * 8);
    } else {
        fifo = fifo - 4;
        regval = (getreg32(BFLB_USB_BASE + USB_DEV_FMAP2_OFFSET) & (0xf << (fifo * 8)));
        regval >>= (fifo * 8);
    }
    return regval;
}

static void bflb_usb_fifo_config(uint8_t fifo, uint8_t ep_type, uint16_t block_size, uint8_t block_num, bool fifo_en)
{
    uint32_t regval;
    uint32_t regaddr;

    if (fifo < 4) {
        regaddr = BFLB_USB_BASE + USB_DEV_FCFG_OFFSET;
    } else {
        fifo = fifo - 4;
        regaddr = BFLB_USB_BASE + USB_DEV_FCFG2_OFFSET;
    }

    regval = getreg32(regaddr);
    regval &= ~(0x3f << (fifo * 8));
    regval |= (ep_type << (fifo * 8 + 0));
    regval |= ((block_num - 1) << (fifo * 8 + 2));
    if (block_size == 1024) {
        regval |= (1 << (fifo * 8 + 4));
    }

    if (fifo_en) {
        regval |= (1 << (fifo * 8 + 5));
    }

    putreg32(regval, regaddr);
}

static void bflb_usb_vdma_start_write(uint8_t fifo, const uint8_t *data, uint32_t len)
{
    uint32_t regval;
    uint32_t regaddr;
    uint32_t mem_regaddr;

    if (fifo == USB_FIFO_CXF) {
        regval = getreg32(BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET);
        regval &= ~USB_VDMA_LEN_CXF_MASK;
        regval &= ~USB_VDMA_IO_CXF;
        regval |= USB_VDMA_TYPE_CXF;
        regval |= (len << USB_VDMA_LEN_CXF_SHIFT);
        putreg32(regval, BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET);

        putreg32((uint32_t)data, BFLB_USB_BASE + USB_VDMA_CXFPS2_OFFSET);

        regval = getreg32(BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET);
        regval |= USB_VDMA_START_CXF;
        putreg32(regval, BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET);
    } else {
        if (fifo < 4) {
            regaddr = BFLB_USB_BASE + USB_VDMA_F0PS1_OFFSET + fifo * 8;
            mem_regaddr = BFLB_USB_BASE + USB_VDMA_F0PS2_OFFSET + fifo * 8;
        } else {
            fifo = fifo - 4;
            regaddr = BFLB_USB_BASE + USB_VDMA_FNPS1_OFFSET + fifo * 8;
            mem_regaddr = BFLB_USB_BASE + USB_VDMA_FNPS2_OFFSET + fifo * 8;
        }
        regval = getreg32(regaddr);
        regval &= ~USB_VDMA_LEN_CXF_MASK;
        regval &= ~USB_VDMA_IO_CXF;
        regval |= USB_VDMA_TYPE_CXF;
        regval |= (len << USB_VDMA_LEN_CXF_SHIFT);
        putreg32(regval, regaddr);

        putreg32((uint32_t)data, mem_regaddr);

        regval = getreg32(regaddr);
        regval |= USB_VDMA_START_CXF;
        putreg32(regval, regaddr);
    }
}

static void bflb_usb_vdma_start_read(uint8_t fifo, uint8_t *data, uint32_t len)
{
    uint32_t regval;
    uint32_t regaddr;
    uint32_t mem_regaddr;

    if (fifo == USB_FIFO_CXF) {
        regval = getreg32(BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET);
        regval &= ~USB_VDMA_LEN_CXF_MASK;
        regval &= ~USB_VDMA_IO_CXF;
        regval &= ~USB_VDMA_TYPE_CXF;
        regval |= (len << USB_VDMA_LEN_CXF_SHIFT);
        putreg32(regval, BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET);

        putreg32((uint32_t)data, BFLB_USB_BASE + USB_VDMA_CXFPS2_OFFSET);

        regval = getreg32(BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET);
        regval |= USB_VDMA_START_CXF;
        putreg32(regval, BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET);
    } else {
        if (fifo < 4) {
            regaddr = BFLB_USB_BASE + USB_VDMA_F0PS1_OFFSET + fifo * 8;
            mem_regaddr = BFLB_USB_BASE + USB_VDMA_F0PS2_OFFSET + fifo * 8;
        } else {
            fifo = fifo - 4;
            regaddr = BFLB_USB_BASE + USB_VDMA_FNPS1_OFFSET + fifo * 8;
            mem_regaddr = BFLB_USB_BASE + USB_VDMA_FNPS2_OFFSET + fifo * 8;
        }
        regval = getreg32(regaddr);
        regval &= ~USB_VDMA_LEN_CXF_MASK;
        regval &= ~USB_VDMA_IO_CXF;
        regval &= ~USB_VDMA_TYPE_CXF;
        regval |= (len << USB_VDMA_LEN_CXF_SHIFT);
        putreg32(regval, regaddr);

        putreg32((uint32_t)data, mem_regaddr);

        regval = getreg32(regaddr);
        regval |= USB_VDMA_START_CXF;
        putreg32(regval, regaddr);
    }
}

static uint32_t bflb_usb_vdma_get_remain_size(uint8_t fifo)
{
    uint32_t regval;
    uint32_t regaddr;

    if (fifo == USB_FIFO_CXF) {
        regval = (getreg32(BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET) & USB_VDMA_LEN_CXF_MASK);
        regval >>= USB_VDMA_LEN_CXF_SHIFT;

    } else {
        if (fifo < 4) {
            regaddr = BFLB_USB_BASE + USB_VDMA_F0PS1_OFFSET + fifo * 8;
        } else {
            fifo = fifo - 4;
            regaddr = BFLB_USB_BASE + USB_VDMA_FNPS1_OFFSET + fifo * 8;
        }
        regval = (getreg32(regaddr) & USB_VDMA_LEN_CXF_MASK);
        regval >>= USB_VDMA_LEN_CXF_SHIFT;
    }

    return regval;
}

static inline void bflb_usb_control_transfer_done(void)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_CXCFE_OFFSET);
    regval |= USB_CX_DONE;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CXCFE_OFFSET);
}

static inline void bflb_usb_set_mult(uint8_t ep_idx, uint8_t mult)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
    regval &= ~USB_TX_NUM_HBW_IEP1_MASK;
    regval |= (mult << USB_TX_NUM_HBW_IEP1_SHIFT);
    putreg32(regval, BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
}

static inline void bflb_usb_send_zlp(uint8_t ep_idx)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
    regval |= USB_TX0BYTE_IEP1;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
}

static inline uint32_t bflb_usb_get_tx_zlp_intstatus(void)
{
    return getreg32(BFLB_USB_BASE + USB_DEV_TXZ_OFFSET);
}

static inline uint32_t bflb_usb_get_rx_zlp_intstatus(void)
{
    return getreg32(BFLB_USB_BASE + USB_DEV_RXZ_OFFSET);
}

static inline void bflb_usb_clear_tx_zlp_intstatus(uint8_t ep_idx)
{
    putreg32((1 << (ep_idx - 1)), BFLB_USB_BASE + USB_DEV_TXZ_OFFSET);
}

static inline void bflb_usb_clear_rx_zlp_intstatus(uint8_t ep_idx)
{
    putreg32((1 << (ep_idx - 1)), BFLB_USB_BASE + USB_DEV_RXZ_OFFSET);
}

static uint32_t bflb_usb_get_source_group_intstatus(uint8_t group)
{
    switch (group) {
        case 0:
            return (getreg32(BFLB_USB_BASE + USB_DEV_ISG0_OFFSET) & ~getreg32(BFLB_USB_BASE + USB_DEV_MISG0_OFFSET));
        case 1:
            return (getreg32(BFLB_USB_BASE + USB_DEV_ISG1_OFFSET) & ~getreg32(BFLB_USB_BASE + USB_DEV_MISG1_OFFSET));
        case 2:
            return (getreg32(BFLB_USB_BASE + USB_DEV_ISG2_OFFSET) & ~getreg32(BFLB_USB_BASE + USB_DEV_MISG2_OFFSET));
        case 3:
            return (getreg32(BFLB_USB_BASE + USB_DEV_ISG3_OFFSET) & ~getreg32(BFLB_USB_BASE + USB_DEV_MISG3_OFFSET));
        case 4:
            return (getreg32(BFLB_USB_BASE + USB_DEV_ISG4_OFFSET) & ~getreg32(BFLB_USB_BASE + USB_DEV_MISG4_OFFSET));
        default:
            break;
    }
    return 0;
}

static void bflb_usb_source_group_int_clear(uint8_t group, uint32_t int_clear)
{
    switch (group) {
        case 0:
            putreg32(int_clear, BFLB_USB_BASE + USB_DEV_ISG0_OFFSET);
            break;
        case 1:
            putreg32(int_clear, BFLB_USB_BASE + USB_DEV_ISG1_OFFSET);
            break;
        case 2:
            putreg32(int_clear, BFLB_USB_BASE + USB_DEV_ISG2_OFFSET);
            break;
        case 3:
            putreg32(int_clear, BFLB_USB_BASE + USB_DEV_ISG3_OFFSET);
            break;
        case 4:
            putreg32(int_clear, BFLB_USB_BASE + USB_DEV_ISG4_OFFSET);
            break;

        default:
            break;
    }
}

static uint8_t usb_get_transfer_fifo(uint8_t ep_idx)
{
    uint8_t target_fifo_id;

    if ((g_bl_udc.out_ep[ep_idx].ep_mps > 512) || (g_bl_udc.in_ep[ep_idx].ep_mps > 512)) {
#if defined(CONFIG_USB_PINGPONG_ENABLE)
        target_fifo_id = (ep_idx - 1) * 4;
#elif defined(CONFIG_USB_TRIPLE_ENABLE)
        target_fifo_id = USB_FIFO_F0;
#else
        target_fifo_id = (ep_idx - 1) * 2;
#endif
    } else {
#if defined(CONFIG_USB_PINGPONG_ENABLE)
        target_fifo_id = (ep_idx - 1) * 2;
#elif defined(CONFIG_USB_TRIPLE_ENABLE)
        target_fifo_id = (ep_idx - 1) * 3;
#else
        target_fifo_id = (ep_idx - 1);
#endif
    }

    return target_fifo_id;
}

int usb_dc_init(uint8_t busid)
{
    uint32_t regval;

    bflb_usb_phy_init();

#ifndef NOT_USE_BFLB_LHAL_IRQ_ATTACH
    bflb_irq_attach(BFLB_USB_IRQ_NUM, USBD_IRQ, NULL);
    bflb_irq_enable(BFLB_USB_IRQ_NUM);
#endif

    /* disable device-A for host */
    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval |= PDS_REG_USB_IDDIG;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    /* disable global irq */
    regval = getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET);
    regval &= ~USB_GLINT_EN_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CTL_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_PHY_TST_OFFSET);
    regval |= USB_UNPLUG;
    putreg32(regval, BFLB_USB_BASE + USB_PHY_TST_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET);
    regval &= ~USB_CAP_RMWAKUP;
    regval |= USB_CHIP_EN_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CTL_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET);
#ifdef CONFIG_USB_HS
    regval &= ~USB_FORCE_FS;
#else
    regval |= USB_FORCE_FS;
#endif
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CTL_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET);
    regval |= USB_SFRST_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CTL_OFFSET);

    while (getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET) & USB_SFRST_HOV) {
    }

    regval = getreg32(BFLB_USB_BASE + USB_DEV_ADR_OFFSET);
    regval &= ~USB_AFT_CONF;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_ADR_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_DEV_SMT_OFFSET);
    regval &= ~USB_SOFMT_MASK;
#ifdef CONFIG_USB_HS
    regval |= USB_SOF_TIMER_MASK_AFTER_RESET_HS;
#else
    regval |= USB_SOF_TIMER_MASK_AFTER_RESET_FS;
#endif
    putreg32(regval, BFLB_USB_BASE + USB_DEV_SMT_OFFSET);

    /* enable setup irq in source group0 */
    regval = getreg32(BFLB_USB_BASE + USB_DEV_MISG0_OFFSET);
    regval &= ~USB_MCX_SETUP_INT;
    regval |= USB_MCX_IN_INT;
    regval |= (1 << 3);
    regval |= USB_MCX_OUT_INT;
    regval |= USB_MCX_IN_INT;
    regval |= USB_MCX_COMFAIL_INT;
    regval |= USB_MCX_COMABORT_INT;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_MISG0_OFFSET);

    /* disable all fifo irq in source group1 */
    putreg32(0xffffffff, BFLB_USB_BASE + USB_DEV_MISG1_OFFSET);

    /* enable rst/tx0/rx0 irq in source group2 */
    regval = 0xffffffff;
    regval &= ~USB_MUSBRST_INT;
    regval &= ~USB_MSUSP_INT;
    regval &= ~USB_MRESM_INT;
    regval &= ~USB_MTX0BYTE_INT;
    regval &= ~USB_MRX0BYTE_INT;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_MISG2_OFFSET);

    /* enable vdma cmplt and error irq in source group3 */
    regval = 0xffffffff;
    regval &= ~(USB_MVDMA_CMPLT_CXF |
#if (defined(BL616D) || defined(BL616L))
                USB_MVDMA_CMPLT_F4 |
                USB_MVDMA_CMPLT_F5 |
                USB_MVDMA_CMPLT_F6 |
                USB_MVDMA_CMPLT_F7 |
#endif
                USB_MVDMA_CMPLT_F0 |
                USB_MVDMA_CMPLT_F1 |
                USB_MVDMA_CMPLT_F2 |
                USB_MVDMA_CMPLT_F3);
    // regval &= ~(USB_MVDMA_ERROR_CXF |
    //             USB_MVDMA_ERROR_F0 |
    //             USB_MVDMA_ERROR_F1 |
    //             USB_MVDMA_ERROR_F2 |
    //             USB_MVDMA_ERROR_F3);
    putreg32(regval, BFLB_USB_BASE + USB_DEV_MISG3_OFFSET);

    /* enable group irq */
    regval = getreg32(BFLB_USB_BASE + USB_DEV_MIGR_OFFSET);
    regval &= ~USB_MINT_G0;
    regval &= ~USB_MINT_G1;
    regval &= ~USB_MINT_G2;
    regval &= ~USB_MINT_G3;
    regval &= ~USB_MINT_G4;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_MIGR_OFFSET);

    /* enable device irq */
    regval = getreg32(BFLB_USB_BASE + USB_GLB_INT_OFFSET);
    regval |= USB_MHC_INT;
    regval |= USB_MOTG_INT;
    regval &= ~USB_MDEV_INT;
    putreg32(regval, BFLB_USB_BASE + USB_GLB_INT_OFFSET);

    bflb_usb_source_group_int_clear(2, 0x3ff);
    bflb_usb_source_group_int_clear(3, 0xffffffff);

    for (uint8_t i = 1; i < 9; i++) {
        bflb_usb_set_ep_fifomap(i, 15);
        bflb_usb_set_fifo_epmap(i, 15, 0);
    }

    /* enable vdma */
    regval = getreg32(BFLB_USB_BASE + USB_VDMA_CTRL_OFFSET);
    regval |= USB_VDMA_EN;
    putreg32(regval, BFLB_USB_BASE + USB_VDMA_CTRL_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_PHY_TST_OFFSET);
    regval &= ~USB_UNPLUG;
    putreg32(regval, BFLB_USB_BASE + USB_PHY_TST_OFFSET);

    /* enable USB_HC_CONN_DET_EN and USB_HC_WKP_DET_EN */
    regval = getreg32(BFLB_USB_BASE + USB_HCMISC_OFFSET);
    regval |= USB_HC_WKP_DET_EN;
    regval |= USB_HC_CONN_DET_EN;
    putreg32(regval, BFLB_USB_BASE + USB_HCMISC_OFFSET);

    /* enable global irq */
    regval = getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET);
    regval |= USB_GLINT_EN_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CTL_OFFSET);

#if 0
    /* enable test mode for DV */
    regval = getreg32(BFLB_USB_BASE + USB_DEV_TST_OFFSET);
    regval |= USB_TST_MOD_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_TST_OFFSET);
#endif

    return 0;
}

int usb_dc_deinit(uint8_t busid)
{
    uint32_t regval;

    /* disable global irq */
    regval = getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET);
    regval &= ~USB_GLINT_EN_HOV;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CTL_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_PHY_TST_OFFSET);
    regval |= USB_UNPLUG;
    putreg32(regval, BFLB_USB_BASE + USB_PHY_TST_OFFSET);

#if defined(BL616) || defined(BL606P) || defined(BL808)
    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    regval &= ~PDS_REG_USB_PHY_XTLSEL_MASK;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    regval &= ~PDS_REG_PU_USB20_PSW;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);
    regval &= ~PDS_REG_USB_PHY_PONRST;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_PHY_CTRL_OFFSET);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval &= ~PDS_REG_USB_EXT_SUSP_N;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
#endif

    return 0;
}

int usbd_set_address(uint8_t busid, const uint8_t addr)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_ADR_OFFSET);
    regval &= ~USB_DEVADR_MASK;
    regval |= addr;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_ADR_OFFSET);

    return 0;
}

int usbd_set_remote_wakeup(uint8_t busid)
{
    uint32_t regval;

    regval = getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET);
    regval |= USB_CAP_RMWAKUP;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CTL_OFFSET);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval |= PDS_REG_USB_WAKEUP;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    bflb_mtimer_delay_ms(10);

    regval = getreg32(BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);
    regval &= ~PDS_REG_USB_WAKEUP;
    putreg32(regval, BFLB_PDS_BASE + PDS_USB_CTL_OFFSET);

    regval = getreg32(BFLB_USB_BASE + USB_DEV_CTL_OFFSET);
    regval &= ~USB_CAP_RMWAKUP;
    putreg32(regval, BFLB_USB_BASE + USB_DEV_CTL_OFFSET);

    return 0;
}

uint8_t usbd_get_port_speed(uint8_t busid)
{
    uint8_t speed = 3;

    speed = (getreg32(BFLB_USB_BASE + USB_OTG_CSR_OFFSET) & USB_SPD_TYP_HOV_POV_MASK) >> USB_SPD_TYP_HOV_POV_SHIFT;

    if (speed == 0) {
        return USB_SPEED_FULL;
    } else if (speed == 1) {
        return USB_SPEED_LOW;
    } else if (speed == 2) {
        return USB_SPEED_HIGH;
    }
    return USB_SPEED_HIGH;
}

int usbd_ep_open(uint8_t busid, const struct usb_endpoint_descriptor *ep)
{
    uint8_t ep_addr;
    uint32_t regval;

    ep_addr = ep->bEndpointAddress;

    uint8_t ep_idx = USB_EP_GET_IDX(ep_addr);

#if defined(BL616) || defined(BL606P) || defined(BL808)
    if ((ep_idx > 4) && (ep_idx < 9)) {
        return 0;
    }
#elif defined(BL616D) || defined(BL616L)
    if ((ep_idx > 8) && (ep_idx < 16)) {
        return 0;
    }
#endif

    if (USB_EP_DIR_IS_OUT(ep_addr)) {
        g_bl_udc.out_ep[ep_idx].ep_mps = USB_GET_MAXPACKETSIZE(ep->wMaxPacketSize);
        g_bl_udc.out_ep[ep_idx].ep_type = USB_GET_ENDPOINT_TYPE(ep->bmAttributes);
        g_bl_udc.out_ep[ep_idx].ep_enable = 1U;
        bflb_usb_set_outep_mps(ep_idx, USB_GET_MAXPACKETSIZE(ep->wMaxPacketSize));
    } else {
        g_bl_udc.in_ep[ep_idx].ep_mps = USB_GET_MAXPACKETSIZE(ep->wMaxPacketSize);
        g_bl_udc.in_ep[ep_idx].ep_type = USB_GET_ENDPOINT_TYPE(ep->bmAttributes);
        g_bl_udc.in_ep[ep_idx].ep_enable = 1U;
        bflb_usb_set_inep_mps(ep_idx, USB_GET_MAXPACKETSIZE(ep->wMaxPacketSize));
        bflb_usb_set_mult(ep_idx, 0);
    }

    if (ep_idx != 0) {
#if !defined(CONFIG_USB_PINGPONG_ENABLE) && !defined(CONFIG_USB_TRIPLE_ENABLE)
        if (USB_GET_MAXPACKETSIZE(ep->wMaxPacketSize) > 512) {
            bflb_usb_set_ep_fifomap(1, USB_FIFO_F0);
            bflb_usb_set_ep_fifomap(2, USB_FIFO_F2);
#if defined(BL616D) || defined(BL616L)
            bflb_usb_set_ep_fifomap(3, USB_FIFO_F4);
            bflb_usb_set_ep_fifomap(4, USB_FIFO_F6);
#endif

            bflb_usb_set_fifo_epmap(USB_FIFO_F0, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F1, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F2, 2, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F3, 2, USB_FIFO_DIR_BID);
#if defined(BL616D) || defined(BL616L)
            bflb_usb_set_fifo_epmap(USB_FIFO_F4, 3, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F5, 3, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F6, 4, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F7, 4, USB_FIFO_DIR_BID);
#endif
            if (ep_idx == 1) {
                bflb_usb_fifo_config(USB_FIFO_F0, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 1, true);
                bflb_usb_fifo_config(USB_FIFO_F1, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 1, false);
            } else if (ep_idx == 2) {
                bflb_usb_fifo_config(USB_FIFO_F2, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 1, true);
                bflb_usb_fifo_config(USB_FIFO_F3, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 1, false);
#if defined(BL616D) || defined(BL616L)
            } else if (ep_idx == 3) {
                bflb_usb_fifo_config(USB_FIFO_F4, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 1, true);
                bflb_usb_fifo_config(USB_FIFO_F5, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 1, false);
            } else if (ep_idx == 4) {
                bflb_usb_fifo_config(USB_FIFO_F6, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 1, true);
                bflb_usb_fifo_config(USB_FIFO_F7, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 1, false);
#endif
            } else {
                return -1;
            }
        } else {
            bflb_usb_set_ep_fifomap(1, USB_FIFO_F0);
            bflb_usb_set_ep_fifomap(2, USB_FIFO_F1);
            bflb_usb_set_ep_fifomap(3, USB_FIFO_F2);
            bflb_usb_set_ep_fifomap(4, USB_FIFO_F3);
#if defined(BL616D) || defined(BL616L)
            bflb_usb_set_ep_fifomap(5, USB_FIFO_F4);
            bflb_usb_set_ep_fifomap(6, USB_FIFO_F5);
            bflb_usb_set_ep_fifomap(7, USB_FIFO_F6);
            bflb_usb_set_ep_fifomap(8, USB_FIFO_F7);
#endif

            bflb_usb_set_fifo_epmap(USB_FIFO_F0, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F1, 2, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F2, 3, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F3, 4, USB_FIFO_DIR_BID);
#if defined(BL616D) || defined(BL616L)
            bflb_usb_set_fifo_epmap(USB_FIFO_F4, 5, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F5, 6, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F6, 7, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F7, 8, USB_FIFO_DIR_BID);
#endif
            if (ep_idx == 1) {
                bflb_usb_fifo_config(USB_FIFO_F0, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 1, true);
            } else if (ep_idx == 2) {
                bflb_usb_fifo_config(USB_FIFO_F1, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 1, true);
            } else if (ep_idx == 3) {
                bflb_usb_fifo_config(USB_FIFO_F2, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 1, true);
            } else if (ep_idx == 4) {
                bflb_usb_fifo_config(USB_FIFO_F3, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 1, true);
#if defined(BL616D) || defined(BL616L)
            } else if (ep_idx == 5) {
                bflb_usb_fifo_config(USB_FIFO_F4, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 1, true);
            } else if (ep_idx == 6) {
                bflb_usb_fifo_config(USB_FIFO_F5, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 1, true);
            } else if (ep_idx == 7) {
                bflb_usb_fifo_config(USB_FIFO_F6, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 1, true);
            } else if (ep_idx == 8) {
                bflb_usb_fifo_config(USB_FIFO_F7, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 1, true);
#endif
            } else {
                return -1;
            }
        }
#elif defined(CONFIG_USB_PINGPONG_ENABLE)
        if (USB_GET_MAXPACKETSIZE(ep->wMaxPacketSize) > 512) {
            bflb_usb_set_ep_fifomap(1, USB_FIFO_F0);

            bflb_usb_set_fifo_epmap(USB_FIFO_F0, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F1, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F2, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F3, 1, USB_FIFO_DIR_BID);
#if defined(BL616D) || defined(BL616L)
            bflb_usb_set_fifo_epmap(USB_FIFO_F4, 2, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F5, 2, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F6, 2, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F7, 2, USB_FIFO_DIR_BID);
#endif
            if (ep_idx == 1) {
                bflb_usb_fifo_config(USB_FIFO_F0, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 2, true);
                bflb_usb_fifo_config(USB_FIFO_F1, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 2, false);
                bflb_usb_fifo_config(USB_FIFO_F2, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 2, false);
                bflb_usb_fifo_config(USB_FIFO_F3, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 2, false);
#if defined(BL616D) || defined(BL616L)
            } else if (ep_idx == 2) {
                bflb_usb_fifo_config(USB_FIFO_F4, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 2, true);
                bflb_usb_fifo_config(USB_FIFO_F5, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 2, false);
                bflb_usb_fifo_config(USB_FIFO_F6, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 2, false);
                bflb_usb_fifo_config(USB_FIFO_F7, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 1024, 2, false);
#endif
            } else {
                return -1;
            }
        } else {
            bflb_usb_set_ep_fifomap(1, USB_FIFO_F0);
            bflb_usb_set_ep_fifomap(2, USB_FIFO_F2);
#if defined(BL616D) || defined(BL616L)
            bflb_usb_set_ep_fifomap(3, USB_FIFO_F4);
            bflb_usb_set_ep_fifomap(4, USB_FIFO_F6);
#endif
            bflb_usb_set_fifo_epmap(USB_FIFO_F0, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F1, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F2, 2, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F3, 2, USB_FIFO_DIR_BID);
#if defined(BL616D) || defined(BL616L)
            bflb_usb_set_fifo_epmap(USB_FIFO_F4, 3, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F5, 3, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F6, 4, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F7, 4, USB_FIFO_DIR_BID);
#endif
            if (ep_idx == 1) {
                bflb_usb_fifo_config(USB_FIFO_F0, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 2, true);
                bflb_usb_fifo_config(USB_FIFO_F1, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 2, false);
            } else if (ep_idx == 2) {
                bflb_usb_fifo_config(USB_FIFO_F2, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 2, true);
                bflb_usb_fifo_config(USB_FIFO_F3, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 2, false);
#if defined(BL616D) || defined(BL616L)
            } else if (ep_idx == 3) {
                bflb_usb_fifo_config(USB_FIFO_F4, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 2, true);
                bflb_usb_fifo_config(USB_FIFO_F5, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 2, false);
            } else if (ep_idx == 4) {
                bflb_usb_fifo_config(USB_FIFO_F6, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 2, true);
                bflb_usb_fifo_config(USB_FIFO_F7, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 2, false);
#endif
            } else {
                return -1;
            }
        }
#elif defined(CONFIG_USB_TRIPLE_ENABLE)
        if (USB_GET_MAXPACKETSIZE(ep->wMaxPacketSize) > 512) {
            return -1;
        } else {
            bflb_usb_set_ep_fifomap(1, USB_FIFO_F0);
            bflb_usb_set_ep_fifomap(2, USB_FIFO_F3);

            bflb_usb_set_fifo_epmap(USB_FIFO_F0, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F1, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F2, 1, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F3, 2, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F4, 2, USB_FIFO_DIR_BID);
            bflb_usb_set_fifo_epmap(USB_FIFO_F5, 2, USB_FIFO_DIR_BID);

            if (ep_idx == 1) {
                bflb_usb_fifo_config(USB_FIFO_F0, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 3, true);
                bflb_usb_fifo_config(USB_FIFO_F1, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 3, false);
                bflb_usb_fifo_config(USB_FIFO_F2, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 3, false);
            } else if (ep_idx == 2) {
                bflb_usb_fifo_config(USB_FIFO_F3, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 3, true);
                bflb_usb_fifo_config(USB_FIFO_F4, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 3, true);
                bflb_usb_fifo_config(USB_FIFO_F5, USB_GET_ENDPOINT_TYPE(ep->bmAttributes), 512, 3, true);
            } else {
                return -1;
            }
        }
#endif
        regval = getreg32(BFLB_USB_BASE + USB_DEV_ADR_OFFSET);
        regval |= USB_AFT_CONF;
        putreg32(regval, BFLB_USB_BASE + USB_DEV_ADR_OFFSET);
    }
    return 0;
}

int usbd_ep_close(uint8_t busid, const uint8_t ep)
{
    return 0;
}

int usbd_ep_set_stall(uint8_t busid, const uint8_t ep)
{
    uint32_t regval;

    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (ep_idx == 0) {
        regval = getreg32(BFLB_USB_BASE + USB_DEV_CXCFE_OFFSET);
        regval |= USB_CX_STL;
        putreg32(regval, BFLB_USB_BASE + USB_DEV_CXCFE_OFFSET);
    } else {
        if (USB_EP_DIR_IS_OUT(ep)) {
            regval = getreg32(BFLB_USB_BASE + USB_DEV_OUTMPS1_OFFSET + (ep_idx - 1) * 4);
            regval |= USB_STL_OEP1;
            //regval |= USB_RSTG_OEP1;
            putreg32(regval, BFLB_USB_BASE + USB_DEV_OUTMPS1_OFFSET + (ep_idx - 1) * 4);
        } else {
            regval = getreg32(BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
            regval |= USB_STL_IEP1;
            //regval |= USB_RSTG_IEP1;
            putreg32(regval, BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
        }
    }

    return 0;
}

int usbd_ep_clear_stall(uint8_t busid, const uint8_t ep)
{
    uint32_t regval;

    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (ep_idx == 0) {
    } else {
        if (USB_EP_DIR_IS_OUT(ep)) {
            regval = getreg32(BFLB_USB_BASE + USB_DEV_OUTMPS1_OFFSET + (ep_idx - 1) * 4);
            //regval &= ~USB_RSTG_OEP1;
            regval &= ~USB_STL_OEP1;
            putreg32(regval, BFLB_USB_BASE + USB_DEV_OUTMPS1_OFFSET + (ep_idx - 1) * 4);
        } else {
            regval = getreg32(BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
            //regval &= ~USB_RSTG_IEP1;
            regval &= ~USB_STL_IEP1;
            putreg32(regval, BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
        }
    }

    return 0;
}

int usbd_ep_is_stalled(uint8_t busid, const uint8_t ep, uint8_t *stalled)
{
    uint32_t regval;

    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (ep_idx == 0) {
    } else {
        if (USB_EP_DIR_IS_OUT(ep)) {
            regval = getreg32(BFLB_USB_BASE + USB_DEV_OUTMPS1_OFFSET + (ep_idx - 1) * 4);
            if (regval & USB_STL_OEP1) {
                *stalled = 1;
            } else {
                *stalled = 0;
            }
        } else {
            regval = getreg32(BFLB_USB_BASE + USB_DEV_INMPS1_OFFSET + (ep_idx - 1) * 4);
            if (regval & USB_STL_IEP1) {
                *stalled = 1;
            } else {
                *stalled = 0;
            }
        }
    }

    return 0;
}

int usbd_ep_start_write(uint8_t busid, const uint8_t ep, const uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!data && data_len) {
        return -1;
    }

    if (!g_bl_udc.in_ep[ep_idx].ep_enable) {
        return -2;
    }

    g_bl_udc.in_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_bl_udc.in_ep[ep_idx].xfer_len = data_len;
    g_bl_udc.in_ep[ep_idx].actual_xfer_len = 0;
    g_bl_udc.in_ep[ep_idx].ep_active = true;

    if (ep_idx == 0) {
        if (data_len == 0) {
            g_bl_udc.in_ep[ep_idx].ep_active = false;
            bflb_usb_control_transfer_done();
        } else {
            data_len = MIN(data_len, g_bl_udc.in_ep[ep_idx].ep_mps);
            g_bl_udc.in_ep[ep_idx].xfer_len = data_len;
            bflb_usb_vdma_start_write(USB_FIFO_CXF, data, data_len);
        }
    } else {
        if (data_len == 0) {
            bflb_usb_send_zlp(ep_idx);
        } else {
            bflb_usb_vdma_start_write(usb_get_transfer_fifo(ep_idx), data, data_len);
        }
    }

    return 0;
}

int usbd_ep_start_read(uint8_t busid, const uint8_t ep, uint8_t *data, uint32_t data_len)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!data && data_len) {
        return -1;
    }

    if (!g_bl_udc.out_ep[ep_idx].ep_enable) {
        return -2;
    }

    if (data_len == 0) {
        return 0;
    }

    g_bl_udc.out_ep[ep_idx].xfer_buf = (uint8_t *)data;
    g_bl_udc.out_ep[ep_idx].xfer_len = data_len;
    g_bl_udc.out_ep[ep_idx].actual_xfer_len = 0;
    g_bl_udc.out_ep[ep_idx].ep_active = true;

    if (ep_idx == 0) {
        data_len = MIN(data_len, g_bl_udc.out_ep[ep_idx].ep_mps);
        g_bl_udc.out_ep[ep_idx].xfer_len = data_len;
        bflb_usb_vdma_start_read(USB_FIFO_CXF, data, data_len);
    } else {
        bflb_usb_vdma_start_read(usb_get_transfer_fifo(ep_idx), data, data_len);
    }
    return 0;
}

void USBD_IRQHandler(uint8_t busid)
{
    uint32_t glb_intstatus;
    uint32_t dev_intstatus;
    uint32_t subgroup_intstatus;
    uint32_t regval;
    uint8_t ep_idx;

    glb_intstatus = getreg32(BFLB_USB_BASE + USB_GLB_ISR_OFFSET);

    if (glb_intstatus & USB_DEV_INT) {
        dev_intstatus = getreg32(BFLB_USB_BASE + USB_DEV_IGR_OFFSET);
        if (dev_intstatus & USB_INT_G0) {
            subgroup_intstatus = bflb_usb_get_source_group_intstatus(0);

            if (subgroup_intstatus & USB_CX_SETUP_INT) {
                bflb_usb_vdma_start_read(USB_FIFO_CXF, g_setup_buffer, 8);
                while (getreg32(BFLB_USB_BASE + USB_VDMA_CXFPS1_OFFSET) & USB_VDMA_START_CXF) {
                }

                bflb_usb_source_group_int_clear(3, USB_VDMA_CMPLT_CXF);

                usbd_event_ep0_setup_complete_handler(busid, g_setup_buffer);
            }
        }
        if (dev_intstatus & USB_INT_G1) {
        }
        if (dev_intstatus & USB_INT_G2) {
            subgroup_intstatus = bflb_usb_get_source_group_intstatus(2);

            if (subgroup_intstatus & USB_SUSP_INT) {
                bflb_usb_source_group_int_clear(2, USB_SUSP_INT);

                bflb_usb_reset_fifo(USB_FIFO_F0);
                bflb_usb_reset_fifo(USB_FIFO_F1);
                bflb_usb_reset_fifo(USB_FIFO_F2);
                bflb_usb_reset_fifo(USB_FIFO_F3);
#if defined(BL616D) || defined(BL616L)
                bflb_usb_reset_fifo(USB_FIFO_F4);
                bflb_usb_reset_fifo(USB_FIFO_F5);
                bflb_usb_reset_fifo(USB_FIFO_F6);
                bflb_usb_reset_fifo(USB_FIFO_F7);
#endif
                bflb_usb_reset_fifo(USB_FIFO_CXF);

                usbd_event_suspend_handler(busid);
            }
            if (subgroup_intstatus & USB_RESM_INT) {
                bflb_usb_source_group_int_clear(2, USB_RESM_INT);
                usbd_event_resume_handler(busid);
            }
            if (subgroup_intstatus & USB_TX0BYTE_INT) {
                for (uint8_t i = 1; i < USB_MAX_EP_WITH_EP0; i++) {
                    if (bflb_usb_get_tx_zlp_intstatus() & (1 << (i - 1))) {
                        bflb_usb_clear_tx_zlp_intstatus(i);
                        usbd_event_ep_in_complete_handler(busid, i | 0x80, 0);
                    }
                }
                bflb_usb_source_group_int_clear(2, USB_TX0BYTE_INT);
            }
            if (subgroup_intstatus & USB_RX0BYTE_INT) {
                for (uint8_t i = 1; i < USB_MAX_EP_WITH_EP0; i++) {
                    if (bflb_usb_get_rx_zlp_intstatus() & (1 << (i - 1))) {
                        bflb_usb_clear_rx_zlp_intstatus(i);
                        usbd_event_ep_out_complete_handler(busid, i, 0);
                    }
                }

                bflb_usb_source_group_int_clear(2, USB_RX0BYTE_INT);
            }
            if (subgroup_intstatus & USBRST_INT) {
                bflb_usb_source_group_int_clear(2, USBRST_INT);

                bflb_usb_reset_fifo(USB_FIFO_F0);
                bflb_usb_reset_fifo(USB_FIFO_F1);
                bflb_usb_reset_fifo(USB_FIFO_F2);
                bflb_usb_reset_fifo(USB_FIFO_F3);
#if defined(BL616D) || defined(BL616L)
                bflb_usb_reset_fifo(USB_FIFO_F4);
                bflb_usb_reset_fifo(USB_FIFO_F5);
                bflb_usb_reset_fifo(USB_FIFO_F6);
                bflb_usb_reset_fifo(USB_FIFO_F7);
#endif
                bflb_usb_reset_fifo(USB_FIFO_CXF);

                regval = getreg32(BFLB_USB_BASE + USB_DEV_SMT_OFFSET);
                regval &= ~USB_SOFMT_MASK;
#ifdef CONFIG_USB_HS
                regval |= USB_SOF_TIMER_MASK_AFTER_RESET_HS;
#else
                regval |= USB_SOF_TIMER_MASK_AFTER_RESET_FS;
#endif
                putreg32(regval, BFLB_USB_BASE + USB_DEV_SMT_OFFSET);

                arch_memset(&g_bl_udc, 0, sizeof(g_bl_udc));
                usbd_event_reset_handler(busid);
            }
        }
        if (dev_intstatus & USB_INT_G3) {
            subgroup_intstatus = bflb_usb_get_source_group_intstatus(3);
            bflb_usb_source_group_int_clear(3, subgroup_intstatus);
            if (subgroup_intstatus & USB_VDMA_CMPLT_CXF) {
                if (g_bl_udc.in_ep[0].ep_active) {
                    g_bl_udc.in_ep[0].ep_active = false;
                    g_bl_udc.in_ep[0].actual_xfer_len = g_bl_udc.in_ep[0].xfer_len - bflb_usb_vdma_get_remain_size(USB_FIFO_CXF);
                    if (g_bl_udc.in_ep[0].actual_xfer_len < g_bl_udc.in_ep[0].ep_mps) {
                        bflb_usb_control_transfer_done();
                    }
                    usbd_event_ep_in_complete_handler(busid, 0x80, g_bl_udc.in_ep[0].actual_xfer_len);
                } else {
                    g_bl_udc.out_ep[0].ep_active = false;
                    g_bl_udc.out_ep[0].actual_xfer_len = g_bl_udc.out_ep[0].xfer_len - bflb_usb_vdma_get_remain_size(USB_FIFO_CXF);
                    usbd_event_ep_out_complete_handler(busid, 0x00, g_bl_udc.out_ep[0].actual_xfer_len);
                }
            }

            for (uint8_t i = 0; i < USB_MAX_EP_EXCLUDE_EP0; i++) {
                if (subgroup_intstatus & (1 << (i + 1))) {
                    ep_idx = bflb_usb_get_fifo_ep(i);
                    if (g_bl_udc.in_ep[ep_idx].ep_active) {
                        g_bl_udc.in_ep[ep_idx].ep_active = 0;
                        g_bl_udc.in_ep[ep_idx].actual_xfer_len = g_bl_udc.in_ep[ep_idx].xfer_len - bflb_usb_vdma_get_remain_size(i);
                        usbd_event_ep_in_complete_handler(busid, ep_idx | 0x80, g_bl_udc.in_ep[ep_idx].actual_xfer_len);
                    } else if (g_bl_udc.out_ep[ep_idx].ep_active) {
                        g_bl_udc.out_ep[ep_idx].ep_active = 0;
                        g_bl_udc.out_ep[ep_idx].actual_xfer_len = g_bl_udc.out_ep[ep_idx].xfer_len - bflb_usb_vdma_get_remain_size(i);
                        usbd_event_ep_out_complete_handler(busid, ep_idx & 0x7f, g_bl_udc.out_ep[ep_idx].actual_xfer_len);
                    }
                }
            }
        }
        if (dev_intstatus & USB_INT_G4) {
        }
    }
}

#ifdef CONFIG_USBDEV_TEST_MODE
void usbd_execute_test_mode(uint8_t busid, uint8_t test_mode)
{
    uint32_t regval;

    switch (test_mode) {
        case 1: // Test_J
        {
            regval = getreg32(BFLB_USB_BASE + USB_PHY_TST_OFFSET);
            regval |= USB_TST_JSTA;
            putreg32(regval, BFLB_USB_BASE + USB_PHY_TST_OFFSET);
            bflb_usb_control_transfer_done();
        } break;
        case 2: // Test_K
        {
            regval = getreg32(BFLB_USB_BASE + USB_PHY_TST_OFFSET);
            regval |= USB_TST_KSTA;
            putreg32(regval, BFLB_USB_BASE + USB_PHY_TST_OFFSET);

            bflb_usb_control_transfer_done();
        } break;
        case 3: // TEST_SE0_NAK
        {
            regval = getreg32(BFLB_USB_BASE + USB_PHY_TST_OFFSET);
            regval |= USB_TST_SE0NAK;
            putreg32(regval, BFLB_USB_BASE + USB_PHY_TST_OFFSET);

            bflb_usb_control_transfer_done();
        } break;
        case 4: // Test_Packet
        {
            bflb_usb_control_transfer_done();
            regval = getreg32(BFLB_USB_BASE + USB_PHY_TST_OFFSET);
            regval |= USB_TST_PKT;
            putreg32(regval, BFLB_USB_BASE + USB_PHY_TST_OFFSET);

            __attribute__((aligned(32))) uint8_t temp[53];
            uint8_t *pp;
            uint8_t i;
            pp = temp;

            for (i = 0; i < 9; i++) /*JKJKJKJK x 9*/
                *pp++ = 0x00;

            for (i = 0; i < 8; i++) /* 8*AA */
                *pp++ = 0xAA;

            for (i = 0; i < 8; i++) /* 8*EE */
                *pp++ = 0xEE;

            *pp++ = 0xFE;

            for (i = 0; i < 11; i++) /* 11*FF */
                *pp++ = 0xFF;

            *pp++ = 0x7F;
            *pp++ = 0xBF;
            *pp++ = 0xDF;
            *pp++ = 0xEF;
            *pp++ = 0xF7;
            *pp++ = 0xFB;
            *pp++ = 0xFD;
            *pp++ = 0xFC;
            *pp++ = 0x7E;
            *pp++ = 0xBF;
            *pp++ = 0xDF;
            *pp++ = 0xEF;
            *pp++ = 0xF7;
            *pp++ = 0xFB;
            *pp++ = 0xFD;
            *pp++ = 0x7E;

            bflb_usb_vdma_start_write(USB_FIFO_CXF, temp, 53);

            regval = getreg32(BFLB_USB_BASE + USB_DEV_CXCFE_OFFSET);
            regval |= USB_TST_PKDONE;
            putreg32(regval, BFLB_USB_BASE + USB_DEV_CXCFE_OFFSET);

        } break;
        case 5: // Test_Force_Enable
            break;

        default:
            break;
    }
}
#endif
