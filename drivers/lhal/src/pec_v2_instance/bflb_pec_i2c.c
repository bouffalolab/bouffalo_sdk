#include "bflb_pec_v2.h"
#include "bflb_pec_v2_instance.h"
#include "bflb_pec_common.h"
#include "bflb_clock.h"

/*** note
+-----+----------------------+-----+------------+----------------+---------+------+------+-------+-------+----+
| reg | 31                25 | 24  | 23      16 | 15           6 |    5    |  4   |   3  |   2   |   1   | 0  |
+-----+----------------------+-----+------------+----------------+---------+------+------+-------+-------+----+
| R0  | addr10_high or addr7 | dir | addr10_low |                | head10r | nack | stop | start | add10 | en |
+-----+----------------------+-----+------------+----------------+---------+------+------+-------+-------+----+
| R1  | tHD;STA                                 | tSU;STA                                                     |
+-----+-----------------------------------------+-------------------------------------------------------------+
| R2  | data bytes                              | tTIGH                                                       |
+-----+-----------------------------------------+-------------------------------------------------------------+
| R3  | iterator of delay                                                                                     |
+-----+-------------------------------------------------------------------------------------------------------+
| R4  | iterator of bits                                                                                      |
+-----+-------------------------------------------------------------------------------------------------------+
| R5  | iterator of bytes                                                                                     |
+-----+-------------------------------------------------------------------------------------------------------+
IPC-0 end flag
*******************************************************************************/

#define ADDR_WRITE       (57)
#define ADDR_READ        (92)
#define ADDR_STOP        (49)

#define PIN_SCL_BASE     (0)
#define PIN_SDA_BASE     (1)
#define PIN_SCL_SET      (PEC_INSTR_SET_DST_PINDIR_0)
#define PIN_SDA_SET      (PEC_INSTR_SET_DST_PINDIR_1)

#define PIN_LOW          (0)
#define PIN_HIGH         (1)

#define CR_I2C_EN        (0)  /* start one frame, set by software and cleared by hardware */
#define CR_ADDR_10B      (1)  /* 0: 7bit address, 1: 10bit address */
#define CR_START         (2)  /* generate start signal or not before data */
#define CR_STOP          (3)  /* generate stop signal or not after all data */
#define CR_NAK           (4)  /* nak or ack at last data when read */
#define CR_HEAD10R       (5)  /* whether transmit seconnd byte of 10bit address */

#define ST_START_SDA_LOW (8)  /* error flag: sda is always low before start signal */
#define ST_START_SCL_LOW (9)  /* error flag: scl is low in start phase */
#define ST_TO_WRITE_DATA (10) /* SCL is low more than timeout when write data */
#define ST_TO_WRITE_ACK  (11) /* SCL is low more than timeout in ack phase when write data */
#define ST_TO_READ_DATA  (12) /* SCL is low more than timeout when read data */
#define ST_TO_READ_ACK   (13) /* SCL is low more than timeout in ack phase when read data */
#define ST_NAK           (14) /* error flag: slave send nak signal */

#define CR_DIR           (24) /* 0: write, 1: read */

int bflb_pec_i2c_init(struct bflb_device_s *dev, struct bflb_pec_i2c_s *i2c)
{
    struct bflb_pec_cfg_s cfg;
    uint8_t mem, pin;
    int ret;

    ret = bflb_pec_memory_size_check(dev, i2c->mem, 122, &mem);
    if (ret != PEC_STS_OK) {
        return ret;
    }

    bflb_pec_disable(dev);
    cfg.clkdiv = 4;                                    /*!< clock divisor, frequency = clock freq / (clkdiv + 1) */
    cfg.pc_origin = mem + 0;                           /*!< beginning of the program for state machine, 0~255 is available */
    cfg.side_en = 0;                                   /*!< allow instructions to perform sideset optionally, rather than on every instruction */
    cfg.side_pindir = 0;                               /*!< side data is asserted to pin OEs or pin values */
    cfg.fifo_join = PEC_FIFO_JOIN_NONE;                /*!< if rx_fifo and tx_fifo join */
    cfg.out_dir = PEC_SHIFT_DIR_TO_LEFT;               /*!< shift OSR direction */
    cfg.in_dir = PEC_SHIFT_DIR_TO_LEFT;                /*!< shift ISR direction */
    cfg.auto_pull = 0;                                 /*!< pull automatically when the output shift register is emptied */
    cfg.auto_push = 0;                                 /*!< push automatically when the input shift register is filled */
    cfg.wrap_top = 255;                                /*!< after reaching this address, execution is wrapped to wrap_bottom */
    cfg.wrap_bottom = 0;                               /*!< ater reaching wrap_top, execution is wrapped to this address */
    cfg.dma_tx = 0;                                    /*!< enable signal of dma_tx_req/ack interface */
    cfg.dma_rx = 0;                                    /*!< enable signal of dma_rx_req/ack interface */
    cfg.thresh_bits_pull = 32;                         /*!< number of bits shifted out of TXSR before autopull or conditional pull, 1~31, 0 present 32 */
    cfg.thresh_bits_push = 32;                         /*!< number of bits shifted into RXSR before autopush or conditional push, 1~31, 0 present 32 */
    cfg.thresh_fifo_tx = 0;                            /*!< threshold of TX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if rx_join_tx */
    cfg.thresh_fifo_rx = 0;                            /*!< threshold of RX FIFO for interrupt and DMA, 0~15 if no join, 0~31 if tx_join_rx */
    bflb_pec_init(dev, &cfg);

    pin = i2c->pin_sda % 64;
    bflb_pec_pin_out_base_set(dev, PIN_SDA_BASE, pin);
    bflb_pec_pin_out_count_set(dev, PIN_SDA_BASE, 1);
    bflb_pec_pin_set_base_set(dev, PIN_SDA_BASE, pin);
    bflb_pec_pin_set_count_set(dev, PIN_SDA_BASE, 1);
    bflb_pec_pin_in_base_set(dev, PIN_SDA_BASE, pin);
    bflb_pec_pad_oe_inv_enable_bit(dev, pin);
    bflb_pec_pad_oe_clr_bit(dev, pin);
    bflb_pec_pad_o_clr_bit(dev, pin);
    pin = i2c->pin_scl % 64;
    bflb_pec_pin_out_base_set(dev, PIN_SCL_BASE, pin);
    bflb_pec_pin_out_count_set(dev, PIN_SCL_BASE, 1);
    bflb_pec_pin_set_base_set(dev, PIN_SCL_BASE, pin);
    bflb_pec_pin_set_count_set(dev, PIN_SCL_BASE, 1);
    bflb_pec_pin_in_base_set(dev, PIN_SCL_BASE, pin);
    bflb_pec_pad_oe_inv_enable_bit(dev, pin);
    bflb_pec_pad_oe_clr_bit(dev, pin);
    bflb_pec_pad_o_clr_bit(dev, pin);

    /* enable transfer */
    bflb_pec_mem_write(dev, mem + 0, pec_instr_IPCSM(0, 0));
    bflb_pec_mem_write(dev, mem + 1, pec_instr_JBZ(mem + 1, PEC_R0, CR_I2C_EN, PEC_INSTR_BIT_NONE, 0)); /* enable or disable */
    /* generate start */
    bflb_pec_mem_write(dev, mem + 2, pec_instr_JBZ(mem + 15, PEC_R0, CR_START, PEC_INSTR_BIT_NONE, 0)); /* generate start signal or not */
    bflb_pec_mem_write(dev, mem + 3, pec_instr_SET(PEC_INSTR_SET_DST_R4, 36, 0)); /* max 36=4*(8+1) clock when sda is set low by slave */
    bflb_pec_mem_write(dev, mem + 4, pec_instr_JPH(mem + 11, PIN_SDA_BASE, 0, 0));
    bflb_pec_mem_write(dev, mem + 5, pec_instr_JRNZ(mem + 7, PEC_R4, PEC_INSTR_CMP_DEC, 0));
    bflb_pec_mem_write(dev, mem + 6, pec_instr_JBA(mem + ADDR_STOP + 3, PEC_R0, ST_START_SDA_LOW, PEC_INSTR_BIT_SET, 0)); /* end if SDA is always low, > 36 clock */
    bflb_pec_mem_write(dev, mem + 7, pec_instr_SET(PIN_SCL_SET, PIN_LOW, pec_instr_side(0, 240, 0))); /* SCL low */
    bflb_pec_mem_write(dev, mem + 8, pec_instr_SET(PIN_SDA_SET, PIN_HIGH, pec_instr_side(0, 240, 0))); /* SDA high */
    bflb_pec_mem_write(dev, mem + 9, pec_instr_SET(PIN_SCL_SET, PIN_HIGH, pec_instr_side(0, 240, 0))); /* SCL high */
    bflb_pec_mem_write(dev, mem + 10, pec_instr_GOTO(mem + 4, 0));
    /* start */
    bflb_pec_mem_write(dev, mem + 11, pec_instr_SET(PIN_SCL_SET, PIN_HIGH, pec_instr_side(0, 240, 0))); /* SCL high */
    bflb_pec_mem_write(dev, mem + 12, pec_instr_JPH(mem + 14, PIN_SCL_BASE, 0, 0));
    bflb_pec_mem_write(dev, mem + 13, pec_instr_JBA(mem + ADDR_STOP + 3, PEC_R0, ST_START_SCL_LOW, PEC_INSTR_BIT_SET, 0)); /* end if SCL is low before start */
    bflb_pec_mem_write(dev, mem + 14, pec_instr_SET(PIN_SDA_SET, PIN_LOW, pec_instr_side(0, 240, 0))); /* SDA low */

    /* send address */
    bflb_pec_mem_write(dev, mem + 15, pec_instr_MOVREG(PEC_INSTR_MOV_DST_OSR, PEC_INSTR_MOV_DST_R0, PEC_INSTR_MOV_OP_NONE, 0));
    bflb_pec_mem_write(dev, mem + 16, pec_instr_SET(PEC_INSTR_SET_DST_ISR, mem + 18, 0)); /* restore return address */
    bflb_pec_mem_write(dev, mem + 17, pec_instr_GOTO(mem + ADDR_WRITE, 0));
    /* 10bit or 7bit address */
    bflb_pec_mem_write(dev, mem + 18, pec_instr_JBZ(mem + 22, PEC_R0, CR_ADDR_10B, PEC_INSTR_BIT_NONE, 0)); /* 7bit address */
    bflb_pec_mem_write(dev, mem + 19, pec_instr_JBNZ(mem + 22, PEC_R0, CR_HEAD10R, PEC_INSTR_BIT_NONE, 0)); /* only transfer first byte of 10bit address */
    bflb_pec_mem_write(dev, mem + 20, pec_instr_SET(PEC_INSTR_SET_DST_ISR, mem + 22, 0)); /* restore return address */
    bflb_pec_mem_write(dev, mem + 21, pec_instr_GOTO(mem + ADDR_WRITE, 0));

    /* get data bytes */
    bflb_pec_mem_write(dev, mem + 22, pec_instr_MOV(PEC_INSTR_MOV_DST_R5, PEC_INSTR_MOV_SRC_R2, 16, 16, 0)); /* data bytes */
    bflb_pec_mem_write(dev, mem + 23, pec_instr_JRZ(mem + ADDR_STOP, PEC_R5, PEC_INSTR_CMP_NONE, 0));
    bflb_pec_mem_write(dev, mem + 24, pec_instr_SUBI(PEC_R5, PEC_R5, 1, 0));
    /* determin write or read */
    bflb_pec_mem_write(dev, mem + 25, pec_instr_JBNZ(mem + 35, PEC_R0, CR_DIR, PEC_INSTR_BIT_NONE, 0));
    /* write */
    bflb_pec_mem_write(dev, mem + 26, pec_instr_SET(PEC_INSTR_SET_DST_OSR, 0, 0)); /* empty OSR */
    bflb_pec_mem_write(dev, mem + 27, pec_instr_OUTNULL(32, 0));
    bflb_pec_mem_write(dev, mem + 28, pec_instr_PULLEB(0));
    bflb_pec_mem_write(dev, mem + 29, pec_instr_JRS(mem + 31, PEC_INSTR_JRS_OSRNF, 0));
    bflb_pec_mem_write(dev, mem + 30, pec_instr_MOVREG(PEC_INSTR_MOV_DST_OSR, PEC_INSTR_MOV_SRC_OSR, PEC_INSTR_MOV_OP_EX832, 0));
    bflb_pec_mem_write(dev, mem + 31, pec_instr_SET(PEC_INSTR_SET_DST_ISR, mem + 33, 0)); /* use ISR as restore return address */
    bflb_pec_mem_write(dev, mem + 32, pec_instr_GOTO(mem + ADDR_WRITE, 0));
    bflb_pec_mem_write(dev, mem + 33, pec_instr_JRNZ(mem + 28, PEC_R5, PEC_INSTR_CMP_DEC, 0));
    bflb_pec_mem_write(dev, mem + 34, pec_instr_GOTO(mem + ADDR_STOP, 0)); /* complete write, goto stop phase */
    /* read */
    bflb_pec_mem_write(dev, mem + 35, pec_instr_SET(PEC_INSTR_SET_DST_ISR, 0, 0)); /* empty ISR */
    bflb_pec_mem_write(dev, mem + 36, pec_instr_SET(PEC_INSTR_SET_DST_OSR, mem + 38, 0)); /* use OSR as restore return address */
    bflb_pec_mem_write(dev, mem + 37, pec_instr_GOTO(mem + ADDR_READ, 0));
    bflb_pec_mem_write(dev, mem + 38, pec_instr_JRS(mem + 41, PEC_INSTR_JRS_ISRNF, 0));
    bflb_pec_mem_write(dev, mem + 39, pec_instr_MOVREG(PEC_INSTR_MOV_DST_ISR, PEC_INSTR_MOV_SRC_ISR, PEC_INSTR_MOV_OP_EX832, 0));
    bflb_pec_mem_write(dev, mem + 40, pec_instr_PUSHB(0));
    bflb_pec_mem_write(dev, mem + 41, pec_instr_JRNZ(mem + 36, PEC_R5, PEC_INSTR_CMP_DEC, 0));
    /* read NULL0 when byte % 4 != 0 */
    bflb_pec_mem_write(dev, mem + 42, pec_instr_JRS(mem + ADDR_STOP, PEC_INSTR_JRS_ISRE, 0));
    bflb_pec_mem_write(dev, mem + 43, pec_instr_JRS(mem + 47, PEC_INSTR_JRS_ISRF, 0));
    bflb_pec_mem_write(dev, mem + 44, pec_instr_IN0S(8, 0));
    bflb_pec_mem_write(dev, mem + 45, pec_instr_JRS(mem + 47, PEC_INSTR_JRS_ISRF, 0));
    bflb_pec_mem_write(dev, mem + 46, pec_instr_GOTO(mem + 44, 0));
    bflb_pec_mem_write(dev, mem + 47, pec_instr_MOVREG(PEC_INSTR_MOV_DST_ISR, PEC_INSTR_MOV_SRC_ISR, PEC_INSTR_MOV_OP_EX832, 0));
    bflb_pec_mem_write(dev, mem + 48, pec_instr_PUSHB(0));

    /* stop */
    bflb_pec_mem_write(dev, mem + 49, pec_instr_SET(PIN_SCL_SET, PIN_LOW, pec_instr_side(0, 240, 0))); /* SCL low */
    bflb_pec_mem_write(dev, mem + 50, pec_instr_JBNZ(mem + 53, PEC_R0, CR_STOP, PEC_INSTR_BIT_NONE, 0)); /* generate stop signal or not */
    bflb_pec_mem_write(dev, mem + 51, pec_instr_JBA(mem + 0, PEC_R0, CR_I2C_EN, PEC_INSTR_BIT_CLR, 0));
    bflb_pec_mem_write(dev, mem + 52, pec_instr_SET(PIN_SCL_SET, PIN_LOW, pec_instr_side(0, 240, 0))); /* SCL low */
    bflb_pec_mem_write(dev, mem + 53, pec_instr_SET(PIN_SDA_SET, PIN_LOW, pec_instr_side(0, 240, 0))); /* SDA low */
    bflb_pec_mem_write(dev, mem + 54, pec_instr_SET(PIN_SCL_SET, PIN_HIGH, pec_instr_side(0, 240, 0))); /* SCL high */
    bflb_pec_mem_write(dev, mem + 55, pec_instr_SET(PIN_SDA_SET, PIN_HIGH, pec_instr_side(0, 240, 0))); /* SDA high */
    bflb_pec_mem_write(dev, mem + 56, pec_instr_JBA(mem + 0, PEC_R0, CR_I2C_EN, PEC_INSTR_BIT_CLR, 0));

    /* write a byte */
    bflb_pec_mem_write(dev, mem + 57, pec_instr_SET(PEC_INSTR_SET_DST_R4, 7, 0));
    bflb_pec_mem_write(dev, mem + 58, pec_instr_SET(PIN_SCL_SET, PIN_LOW, 0)); /* SCL low */
    bflb_pec_mem_write(dev, mem + 59, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, 16, 16, 0));
    bflb_pec_mem_write(dev, mem + 60, pec_instr_JRNZ(mem + 60, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tHD;STA */
    bflb_pec_mem_write(dev, mem + 61, pec_instr_OUTPINDIR(PIN_SDA_BASE, 1, 0));
    bflb_pec_mem_write(dev, mem + 62, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, 0, 16, 0));
    bflb_pec_mem_write(dev, mem + 63, pec_instr_JRNZ(mem + 63, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tSU;STA */
    bflb_pec_mem_write(dev, mem + 64, pec_instr_SET(PIN_SCL_SET, PIN_HIGH, pec_instr_side(0, 1, 0))); /* SCL high, delay 1 for compensation to equal read data */
    bflb_pec_mem_write(dev, mem + 65, pec_instr_SET(PEC_INSTR_SET_DST_R3, 40 * 2000 / 2, 0)); /* 2ms timeout */
    bflb_pec_mem_write(dev, mem + 66, pec_instr_JPH(mem + 69, PIN_SCL_BASE, 0, 0));
    bflb_pec_mem_write(dev, mem + 67, pec_instr_JRNZ(mem + 66, PEC_R3, PEC_INSTR_CMP_DEC, 0));
    bflb_pec_mem_write(dev, mem + 68, pec_instr_JBA(mem + ADDR_STOP + 3, PEC_R0, ST_TO_WRITE_DATA, PEC_INSTR_BIT_SET, 0));/* SCL is in low level more than timeout */
    bflb_pec_mem_write(dev, mem + 69, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R2, 0, 16, 0));
    bflb_pec_mem_write(dev, mem + 70, pec_instr_JRNZ(mem + 70, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tHIGH */
    bflb_pec_mem_write(dev, mem + 71, pec_instr_JRNZ(mem + 58, PEC_R4, PEC_INSTR_CMP_DEC, 0));
    /* check ack or nck */
    bflb_pec_mem_write(dev, mem + 72, pec_instr_SET(PIN_SCL_SET, PIN_LOW, 0)); /* SCL low */
    bflb_pec_mem_write(dev, mem + 73, pec_instr_SET(PIN_SDA_SET, PIN_HIGH, 0)); /* release SDA */
    bflb_pec_mem_write(dev, mem + 74, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, 16, 16, 0));
    bflb_pec_mem_write(dev, mem + 75, pec_instr_JRNZ(mem + 75, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tHD;STA */
    bflb_pec_mem_write(dev, mem + 76, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, 0, 16, 0));
    bflb_pec_mem_write(dev, mem + 77, pec_instr_JRNZ(mem + 77, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tSU;STA */
    bflb_pec_mem_write(dev, mem + 78, pec_instr_SET(PIN_SCL_SET, PIN_HIGH, 0)); /* SCL high */
    bflb_pec_mem_write(dev, mem + 79, pec_instr_SET(PEC_INSTR_SET_DST_R3, 40 * 2000 / 2, 0)); /* 2ms timeout */
    bflb_pec_mem_write(dev, mem + 80, pec_instr_JPH(mem + 83, PIN_SCL_BASE, 0, 0));
    bflb_pec_mem_write(dev, mem + 81, pec_instr_JRNZ(mem + 80, PEC_R3, PEC_INSTR_CMP_DEC, 0));
    bflb_pec_mem_write(dev, mem + 82, pec_instr_JBA(mem + ADDR_STOP + 3, PEC_R0, ST_TO_WRITE_ACK, PEC_INSTR_BIT_SET, 0));/* SCL is in low level more than timeout */
    bflb_pec_mem_write(dev, mem + 83, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R2, 0, 16, 0));
    bflb_pec_mem_write(dev, mem + 84, pec_instr_JRNZ(mem + 84, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tHIGH */
    bflb_pec_mem_write(dev, mem + 85, pec_instr_JPH(mem + 87, PIN_SDA_BASE, 0, 0));
    bflb_pec_mem_write(dev, mem + 86, pec_instr_MOV(PEC_INSTR_MOV_DST_PC, PEC_INSTR_MOV_SRC_ISR, 0, 8, 0)); /* receive ack, return pre-write function */
    /* receive nak when write */
    bflb_pec_mem_write(dev, mem + 87, pec_instr_SET(PEC_INSTR_SET_DST_R3, mem + 33, 0)); /* set R3 = return address */
    bflb_pec_mem_write(dev, mem + 88, pec_instr_JNEQ(mem + 91, PEC_ISR, PEC_INSTR_CMP_NONE, PEC_R3, PEC_INSTR_CMP_NONE, 0));
    bflb_pec_mem_write(dev, mem + 89, pec_instr_JRNZ(mem + 91, PEC_R5, PEC_INSTR_CMP_NONE, 0));
    bflb_pec_mem_write(dev, mem + 90, pec_instr_MOV(PEC_INSTR_MOV_DST_PC, PEC_INSTR_MOV_SRC_ISR, 0, 8, 0)); /* return pre-write function, even nak when last data */
    bflb_pec_mem_write(dev, mem + 91, pec_instr_JBA(mem + ADDR_STOP + 3, PEC_R0, ST_NAK, PEC_INSTR_BIT_SET, 0)); /* slave nak */

    /* read a byte */
    bflb_pec_mem_write(dev, mem + 92, pec_instr_SET(PEC_INSTR_SET_DST_R4, 7, 0));
    bflb_pec_mem_write(dev, mem + 93, pec_instr_SET(PIN_SCL_SET, PIN_LOW, 0)); /* SCL low */
    bflb_pec_mem_write(dev, mem + 94, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, 16, 16, 0));
    bflb_pec_mem_write(dev, mem + 95, pec_instr_JRNZ(mem + 95, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tHD;STA */
    bflb_pec_mem_write(dev, mem + 96, pec_instr_SET(PIN_SDA_SET, PIN_HIGH, 0)); /* release SDA line */
    bflb_pec_mem_write(dev, mem + 97, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, 0, 16, 0));
    bflb_pec_mem_write(dev, mem + 98, pec_instr_JRNZ(mem + 98, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tSU;STA */
    bflb_pec_mem_write(dev, mem + 99, pec_instr_SET(PIN_SCL_SET, PIN_HIGH, 0)); /* SCL high */
    bflb_pec_mem_write(dev, mem + 100, pec_instr_SET(PEC_INSTR_SET_DST_R3, 40 * 2000 / 2, 0)); /* 2ms timeout */
    bflb_pec_mem_write(dev, mem + 101, pec_instr_JPH(mem + 104, PIN_SCL_BASE, 0, 0));
    bflb_pec_mem_write(dev, mem + 102, pec_instr_JRNZ(mem + 101, PEC_R3, PEC_INSTR_CMP_DEC, 0));
    bflb_pec_mem_write(dev, mem + 103, pec_instr_JBA(mem + ADDR_STOP + 3, PEC_R0, ST_TO_READ_DATA, PEC_INSTR_BIT_SET, 0));/* SCL is in low level more than timeout */
    bflb_pec_mem_write(dev, mem + 104, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R2, 0, 16, 0));
    bflb_pec_mem_write(dev, mem + 105, pec_instr_JRNZ(mem + 105, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tHIGH */
    bflb_pec_mem_write(dev, mem + 106, pec_instr_INPINVAL(PIN_SDA_BASE, 1, 0));
    bflb_pec_mem_write(dev, mem + 107, pec_instr_JRNZ(mem + 93, PEC_R4, PEC_INSTR_CMP_DEC, 0));
    /* send ack or nak when receive last data */
    bflb_pec_mem_write(dev, mem + 108, pec_instr_SET(PIN_SCL_SET, PIN_LOW, 0)); /* SCL low */
    bflb_pec_mem_write(dev, mem + 109, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, 16, 16, 0));
    bflb_pec_mem_write(dev, mem + 110, pec_instr_JRNZ(mem + 110, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tHD;STA */
    bflb_pec_mem_write(dev, mem + 111, pec_instr_JRNZ(mem + 115, PEC_R5, PEC_INSTR_CMP_NONE, 0)); /* not last data */
    bflb_pec_mem_write(dev, mem + 112, pec_instr_JBZ(mem + 115, PEC_R0, CR_NAK, PEC_INSTR_BIT_NONE, 0)); /* need ack */
    bflb_pec_mem_write(dev, mem + 113, pec_instr_SET(PIN_SDA_SET, PIN_HIGH, 0)); /* SDA high, nak */
    bflb_pec_mem_write(dev, mem + 114, pec_instr_GOTO(mem + 116, 0));
    bflb_pec_mem_write(dev, mem + 115, pec_instr_SET(PIN_SDA_SET, PIN_LOW, 0)); /* SDA low, ack */
    bflb_pec_mem_write(dev, mem + 116, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R1, 0, 16, 0));
    bflb_pec_mem_write(dev, mem + 117, pec_instr_JRNZ(mem + 117, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tSU;STA */
    bflb_pec_mem_write(dev, mem + 118, pec_instr_SET(PIN_SCL_SET, PIN_HIGH, 0)); /* SCL high */
    bflb_pec_mem_write(dev, mem + 119, pec_instr_MOV(PEC_INSTR_MOV_DST_R3, PEC_INSTR_MOV_SRC_R2, 0, 16, 0));
    bflb_pec_mem_write(dev, mem + 120, pec_instr_JRNZ(mem + 120, PEC_R3, PEC_INSTR_CMP_DEC, 0)); /* delay, tHIGH */
    bflb_pec_mem_write(dev, mem + 121, pec_instr_MOV(PEC_INSTR_MOV_DST_PC, PEC_INSTR_MOV_SRC_OSR, 0, 8, 0));

    bflb_pec_sm_reg_write(dev, PEC_R0, 0);
    bflb_pec_enable(dev);
    return PEC_STS_OK;
}

void bflb_pec_i2c_set_timing(struct bflb_device_s *dev, struct bflb_pec_i2c_timing_s *timing)
{
    uint32_t src, period, high, setup, hold;
    uint16_t div;

    if (timing->clk != 0) {
        src = bflb_clk_get_peripheral_clock(BFLB_DEVICE_TYPE_PEC, dev->idx);
        if (src >= 160 * 1000 * 1000) {
            div = 4; /* 160MHz / 4 = 40MHz */
        } else if (src >= 80 * 1000 * 1000) {
            div = 2; /* 80MHz / 2 = 40MHz */
        } else {
            div = 1;
        }
        src = src / div;
        period = (src + timing->clk / 2) / timing->clk;
        if (timing->clk <= 100 * 1000) {
            high = period / 2;
            setup = high / 2;
        } else {
            high = (period + 1) / 3;
            setup = high;
        }
        hold = period - high - setup;
    } else {
        div = timing->div;
        high = timing->time_high;
        setup = timing->time_setup;
        hold = timing->time_hold;
    }
    high = (high < 7) ? 0 : (high - 7);
    setup = (setup < 3) ? 0 : (setup - 3);
    hold = (hold < 3) ? 0 : (hold - 3);
    high = (high > 0xFFFF) ? 0xFFFF : high;
    setup = (setup > 0xFFFF) ? 0xFFFF : setup;
    hold = (hold > 0xFFFF) ? 0xFFFF : hold;
    bflb_pec_clock_div_set(dev, div - 1);
    bflb_pec_sm_regl_rs(dev, PEC_R2, high | ((~high) << 16));
    bflb_pec_sm_reg_write(dev, PEC_R1, (hold << 16) | setup);
}

int bflb_pec_i2c_is_busy(struct bflb_device_s *dev)
{
    uint8_t mem, addr;
    uint32_t en;

    mem = bflb_pec_mem_restore(dev);
    addr = bflb_pec_current_pc_get(dev);
    en = bflb_pec_sm_reg_read(dev, PEC_R0);
    en &= (1 << 0);
    if ((addr == (mem + 1)) && (en == 0)) {
        return 0;
    } else {
        return 1;
    }
}

uint32_t bflb_pec_i2c_get_err(struct bflb_device_s *dev)
{
    uint32_t val;

    val = bflb_pec_sm_reg_read(dev, PEC_R0);
    val &= 0x7F00;
    return val;
}

void bflb_pec_i2c_clear_err(struct bflb_device_s *dev)
{
    bflb_pec_sm_regl_rs(dev, PEC_R0, 0x7F << 16);
}

uint32_t bflb_pec_i2c_write(struct bflb_device_s *dev, struct bflb_pec_i2c_data_s *data)
{
    uint32_t val;
    int busy;
    uint16_t length;
    uint8_t level;

    bflb_pec_fifo_clr_tx(dev);
    val = data->length;
    val |= ((~val) << 16);
    bflb_pec_sm_regh_rs(dev, PEC_R2, val);
    if (data->slave_10bit) {
        val = data->slave_addr >> 8;
        val &= 3;
        val |= 0x78; /* 0b11110xx */
        val <<= 9;
        val |= (data->slave_addr & 0xFF);
    } else {
        val = data->slave_addr & 0x7F;
        val <<= 9;
    }
    val &= ~(1 << 8);
    val <<= 16;
    if (data->slave_10bit) {
        val |= (1 << CR_ADDR_10B);
    }
    if (data->start) {
        val |= (1 << CR_START);
    }
    if (data->stop) {
        val |= (1 << CR_STOP);
    }
    if (data->nak) {
        val |= (1 << CR_NAK);
    }
    if (data->head_10) {
        val |= (1 << CR_HEAD10R);
    }
    bflb_pec_sm_reg_write(dev, PEC_R0, val);
    bflb_pec_sm_reg_set_bit(dev, PEC_R0, CR_I2C_EN);
    length = 0;
    while (1) {
        busy = bflb_pec_i2c_is_busy(dev);
        if (busy == 0) {
            val = bflb_pec_i2c_get_err(dev);
            bflb_pec_i2c_clear_err(dev);
            return val;
        }
        level = bflb_pec_fifo_get_level_tx(dev);
        if ((level < PEC_FIFO_DEEPTH) && (length < data->length)) {
            if (length + 1 >= data->length) {
                val = data->buff[length];
                bflb_pec_fifo_write(dev, val);
                length += 1;
            } else if (length + 2 >= data->length) {
                val = data->buff[length];
                val |= data->buff[length + 1] << 8;
                bflb_pec_fifo_write(dev, val);
                length += 2;
            } else if (length + 3 >= data->length) {
                val = data->buff[length];
                val |= data->buff[length + 1] << 8;
                val |= data->buff[length + 2] << 16;
                bflb_pec_fifo_write(dev, val);
                length += 3;
            } else {
                val = data->buff[length];
                val |= data->buff[length + 1] << 8;
                val |= data->buff[length + 2] << 16;
                val |= data->buff[length + 3] << 24;
                bflb_pec_fifo_write(dev, val);
                length += 4;
            }
        }
    }
}

uint32_t bflb_pec_i2c_read(struct bflb_device_s *dev, struct bflb_pec_i2c_data_s *data)
{
    uint32_t val;
    int busy;
    uint16_t length;
    uint8_t level;

    bflb_pec_fifo_clr_rx(dev);
    val = data->length;
    val |= ((~val) << 16);
    bflb_pec_sm_regh_rs(dev, PEC_R2, val);
    if (data->slave_10bit) {
        val = data->slave_addr >> 8;
        val &= 3;
        val |= 0x78; /* 0b11110xx */
        val <<= 9;
        val |= (data->slave_addr & 0xFF);
    } else {
        val = data->slave_addr & 0x7F;
        val <<= 9;
    }
    val |= (1 << 8);
    val <<= 16;
    if (data->slave_10bit) {
        val |= (1 << CR_ADDR_10B);
    }
    if (data->start) {
        val |= (1 << CR_START);
    }
    if (data->stop) {
        val |= (1 << CR_STOP);
    }
    if (data->nak) {
        val |= (1 << CR_NAK);
    }
    if (data->head_10) {
        val |= (1 << CR_HEAD10R);
    }
    bflb_pec_sm_reg_write(dev, PEC_R0, val);
    bflb_pec_sm_reg_set_bit(dev, PEC_R0, CR_I2C_EN);
    length = 0;
    while (1) {
        level = bflb_pec_fifo_get_level_rx(dev);
        if ((level > 0) && (length < data->length)) {
            val = bflb_pec_fifo_read(dev);
            if (length + 1 >= data->length) {
                data->buff[length] = val & 0xFF;
                length += 1;
            } else if (length + 2 >= data->length) {
                data->buff[length] = val & 0xFF;
                data->buff[length + 1] = (val >> 8) & 0xFF;
                length += 2;
            } else if (length + 3 >= data->length) {
                data->buff[length] = val & 0xFF;
                data->buff[length + 1] = (val >> 8) & 0xFF;
                data->buff[length + 2] = (val >> 16) & 0xFF;
                length += 3;
            } else {
                data->buff[length] = val & 0xFF;
                data->buff[length + 1] = (val >> 8) & 0xFF;
                data->buff[length + 2] = (val >> 16) & 0xFF;
                data->buff[length + 3] = (val >> 24) & 0xFF;
                length += 4;
            }
        } else {
            busy = bflb_pec_i2c_is_busy(dev);
            level = bflb_pec_fifo_get_level_rx(dev);
            if ((busy == 0) && (level == 0)) {
                val= bflb_pec_i2c_get_err(dev);
                bflb_pec_i2c_clear_err(dev);
                return val;
            }
        }
    }
}

