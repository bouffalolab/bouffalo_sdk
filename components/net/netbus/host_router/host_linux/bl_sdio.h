
/**
 ******************************************************************************
 *
 * @file bl_sdio.h
 *
 * Copyright (C) BouffaloLab 2017-2018
 *
 ******************************************************************************
 */

#ifndef _BL_SDIO_H_
#define _BL_SDIO_H_


//////////////////////dnld fw//////////////////////////////////
#define MBIT(x)    (((u32)1) << (x))
#define HOST_INT_RSR_REG        0x01
#define HOST_INT_RSR_MASK       0x3F
#define RD_BITMAP_L         0x04
#define RD_BITMAP_U         0x05
#define WR_BITMAP_L         0x06
#define WR_BITMAP_U         0x07
#define CARD_FW_STATUS0_REG     0x60
#define CARD_MISC_CFG_REG       0x6C
#define CARD_MISC_CFG_REG       0x6C
#define IO_PORT_0_REG           0x78
#define IO_PORT_1_REG           0x79
#define IO_PORT_2_REG           0x7A

#define USE_15_DATA_PORT

#ifdef USE_15_DATA_PORT
#define DATA_PORT_MSK 0x0000fffe
#define MAX_PORT_NUM 16
#else
#define DATA_PORT_MSK 0x00007ffe
#define MAX_PORT_NUM 15
//#define DATA_PORT_MSK 0x00000002
//#define MAX_PORT_NUM 2
#endif

#define BLOCK_MODE  1
#define BYTE_MODE   0

#define BL_SDIO_BLOCK_SIZE 256

#define REG_PORT            0

#define BL_SDIO_IO_PORT_MASK        0xfffff

#define BL_SDIO_BYTE_MODE_MASK  0x80000000

#define SDIO_MPA_ADDR_BASE      0x1000
#define CTRL_PORT           0
#define CTRL_PORT_MASK          0x0001

#define CMD_PORT_UPLD_INT_MASK      (0x1U<<6)
#define CMD_PORT_DNLD_INT_MASK      (0x1U<<7)
#define HOST_TERM_CMD53         (0x1U << 2)
#define REG_PORT            0
#define MEM_PORT            0x10000

#define CMD53_NEW_MODE          (0x1U << 0)
#define CMD_PORT_RD_LEN_EN      (0x1U << 2)
#define CMD_PORT_AUTO_EN        (0x1U << 0)
#define CMD_PORT_SLCT           0x8000
#define UP_LD_CMD_PORT_HOST_INT_STATUS  (0x40U)
#define DN_LD_CMD_PORT_HOST_INT_STATUS  (0x80U)

#define BL_MP_AGGR_BUF_SIZE_16K (16384)
#define BL_MP_AGGR_BUF_SIZE_32K (32768)

/* Misc. Config Register : Auto Re-enable interrupts */
#define AUTO_RE_ENABLE_INT              BIT(4)

/* Host Control Registers : Configuration */
#define CONFIGURATION_REG       0x00
/* Host Control Registers : Host power up */
#define HOST_POWER_UP           (0x1U << 1)

/* Host Control Registers : Upload host interrupt mask */
#define UP_LD_HOST_INT_MASK     (0x1U)
/* Host Control Registers : Download host interrupt mask */
#define DN_LD_HOST_INT_MASK     (0x2U)

/* Host Control Registers : Upload host interrupt status */
#define UP_LD_HOST_INT_STATUS       (0x1U)
/* Host Control Registers : Download host interrupt status */
#define DN_LD_HOST_INT_STATUS       (0x2U)

/* Host Control Registers : Host interrupt status */
#define CARD_INT_STATUS_REG     0x28

/* Card Control Registers : Card I/O ready */
#define CARD_IO_READY                   (0x1U << 3)
/* Card Control Registers : Download card ready */
#define DN_LD_CARD_RDY                  (0x1U << 0)

struct bl_sdio_card_reg {
    u8 start_rd_port;
    u8 start_wr_port;
    u8 base_0_reg;
    u8 base_1_reg;
    u8 poll_reg;
    u8 host_int_enable;
    u8 host_int_rsr_reg;
    u8 host_int_status_reg;
    u8 host_int_mask_reg;
    u8 scratch_0_0;
    u8 scratch_1_3;
    u8 sdio_int_mask;
    u32 data_port_mask;
    u8 io_port_0_reg;
    u8 io_port_1_reg;
    u8 io_port_2_reg;
    u8 max_mp_regs;
    u8 rd_bitmap_l;
    u8 rd_bitmap_u;
    u8 rd_bitmap_1l;
    u8 rd_bitmap_1u;
    u8 wr_bitmap_l;
    u8 wr_bitmap_u;
    u8 wr_bitmap_1l;
    u8 wr_bitmap_1u;
    u8 rd_len_p0_l;
    u8 rd_len_p0_u;
    u8 card_misc_cfg_reg;
    u8 card_cfg_2_1_reg;
    u8 cmd_rd_len_0;
    u8 cmd_rd_len_1;
    u8 cmd_rd_len_2;
    u8 cmd_rd_len_3;
    u8 cmd_cfg_0;
    u8 cmd_cfg_1;
    u8 cmd_cfg_2;
    u8 cmd_cfg_3;
    u8 fw_dump_ctrl;
    u8 fw_dump_start;
    u8 fw_dump_end;
};

static const struct bl_sdio_card_reg bl_reg_sd606 = {
    .start_rd_port = 1,
    .start_wr_port = 1,
    .base_0_reg = 0x0040,
    .base_1_reg = 0x0041,
    .poll_reg = 0x30,
    .host_int_enable = UP_LD_HOST_INT_MASK | DN_LD_HOST_INT_MASK,
    .host_int_rsr_reg = 0x1,
    .host_int_mask_reg = 0x02,
    .host_int_status_reg = 0x03,
    .scratch_0_0 = 0x60,
    .scratch_1_3 = 0x67,
    .sdio_int_mask = 0x3f,
    .data_port_mask = DATA_PORT_MSK,
    .io_port_0_reg = 0x78,
    .io_port_1_reg = 0x79,
    .io_port_2_reg = 0x7A,
    .max_mp_regs = 64,
    .rd_bitmap_l = 0x04,
    .rd_bitmap_u = 0x05,
    .wr_bitmap_l = 0x06,
    .wr_bitmap_u = 0x07,
    .rd_len_p0_l = 0x08,
    .rd_len_p0_u = 0x09,
    .card_misc_cfg_reg = 0x6c,
};

int bl_sdio_register_drv(void);
void bl_sdio_unregister_drv(void);
int bl_read_reg(struct bl_eth_device *bl_eth_device, u32 reg, u8 *data);
int bl_read16(struct bl_eth_device *dev, u32 reg, u16 *data);
int bl_read32(struct bl_eth_device *dev, u32 reg, u32 *data);
int bl_write_reg(struct bl_eth_device *bl_eth_device, u32 reg, u8 data);
void bl_get_rd_len(struct bl_eth_device *bl_eth_device, u32 reg_l, u32 reg_u, u32 *len);
int bl_get_rd_port(struct bl_eth_device *bl_eth_device, u32 *port);
int bl_get_wr_port(struct bl_eth_device *bl_eth_device, u32 *port);
int bl_read_data_sync(struct bl_eth_device *bl_eth_device, u8 *buffer, u32 len, u32 port);
int bl_read_data_sync_claim0(struct bl_eth_device *bl_eth_device, u8 *buffer, u32 len, u32 port);
int bl_write_data_sync(struct bl_eth_device *bl_eth_device, u8 *buffer, u32 pkt_len, u32 port);
void bl_irq_hdlr(struct sdio_func *func);

#endif /* _BL_SDIO_H_ */
