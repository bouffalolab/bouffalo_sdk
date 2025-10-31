#include "bflb_pec_v2_instr.h"

#define POS_OP        (28)
#define CALC_POS_RS1  (0)
#define CALC_POS_RS2  (3)
#define CALC_POS_IMM  (3)
#define CALC_POS_RD   (11)
#define CALC_POS_OP   (14)

#define INVALID_SIDE  (0xF0000000)
#define INVALID_INSTR (0xFFFFFFFF)

uint32_t pec_instr_side(uint32_t side, uint32_t delay, uint32_t side_count)
{
    uint32_t side_max;
    uint32_t delay_max;
    uint32_t side_pos;

    if (side_count > 7) {
        return INVALID_SIDE;
    }
    side_max = 1 << side_count;
    delay_max = 1 << (8 - side_count);
    side_pos = 28 - side_count; 
    if (side >= side_max) {
        return INVALID_SIDE;
    }
    if (delay >= delay_max) {
        return INVALID_SIDE;
    }
    return (side << side_pos) | (delay << 20);
}

static uint32_t jmp_pin(uint8_t addr, uint8_t pin_base, uint8_t offset, uint32_t side, uint8_t cond)
{
    uint32_t instr;

    if (pin_base > 4) {
        return INVALID_INSTR;
    }
    if (offset > 7) {
        return INVALID_INSTR;
    }
    instr = 0 << POS_OP;
    instr |= (addr << 0);
    instr |= (pin_base << 8);
    instr |= (offset << 11);
    if (cond) {
        instr |= (1 << 19);   /* jump when pin level is high */
    } else {
        instr &= ~(1 << 19);  /* jump when pin level is low */
    }
    instr |= side;
    return instr;
}

uint32_t pec_instr_JPH(uint8_t addr, uint8_t pin_base, uint8_t offset, uint32_t side)
{
    return jmp_pin(addr, pin_base, offset, side, 1);
}

uint32_t pec_instr_JPL(uint8_t addr, uint8_t pin_base, uint8_t offset, uint32_t side)
{
    return jmp_pin(addr, pin_base, offset, side, 0);
}

static uint32_t jmp_reg(uint8_t addr, uint8_t reg, uint8_t dec, uint32_t side, uint8_t cond)
{
    uint32_t instr;

    if (reg > 15) {
        return INVALID_INSTR;
    }
    if (reg > 7 && dec != 0) {
        return INVALID_INSTR;
    }
    instr = 1 << POS_OP;
    instr |= (addr << 0);
    instr |= (reg << 8);
    if (dec) {
        instr |= 1 << 12;    /* reg value decrease one after complete */
    } else {
        instr &= ~(1 << 12); /* no action after complete */
    }
    if (cond) {
        instr |= (1 << 19);  /* jump when pin level is high */
    } else {
        instr &= ~(1 << 19); /* jump when pin level is low */
    }
    instr |= side;
    return instr;
}

uint32_t pec_instr_JRZ(uint8_t addr, uint8_t reg, uint8_t dec, uint32_t side)
{
    return jmp_reg(addr, reg, dec, side, 0);
}

uint32_t pec_instr_JRNZ(uint8_t addr, uint8_t reg, uint8_t dec, uint32_t side)
{
    return jmp_reg(addr, reg, dec, side, 1);
}

uint32_t pec_instr_JRS(uint8_t addr, uint8_t state, uint32_t side)
{
    return jmp_reg(addr, state, 0, side, 0);
}

static uint32_t jmp_cmp(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side, uint8_t cond)
{
    uint32_t instr;

    if (reg1 > 7 || reg2 > 7) {
        return INVALID_INSTR;
    }
    if (post1 > 2 || post2 > 2) {
        return INVALID_INSTR;
    }
    if (cond > 3) {
        return INVALID_INSTR;
    }
    instr = 2 << POS_OP;
    instr |= (addr << 0);
    instr |= (reg1 << 8);
    instr |= (post1 << 11);
    instr |= (reg2 << 13);
    instr |= (post2 << 16);
    instr |= (cond << 18);
    instr |= side;
    return instr;
}

uint32_t pec_instr_JEQU(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side)
{
    return jmp_cmp(addr, reg1, post1, reg2, post2, side, 0);
}

uint32_t pec_instr_JNEQ(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side)
{
    return jmp_cmp(addr, reg1, post1, reg2, post2, side, 1);
}

uint32_t pec_instr_JLET(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side)
{
    return jmp_cmp(addr, reg1, post1, reg2, post2, side, 2);
}

uint32_t pec_instr_JLEQ(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side)
{
    return jmp_cmp(addr, reg1, post1, reg2, post2, side, 3);
}
uint32_t pec_instr_JHIT(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side)
{
    return jmp_cmp(addr, reg2, post2, reg1, post1, side, 3);
}

uint32_t pec_instr_JHEQ(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side)
{
    return jmp_cmp(addr, reg2, post2, reg1, post1, side, 2);
}

static uint32_t jmp_bit(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t post, uint32_t side, uint8_t cond, uint8_t always)
{
    uint32_t instr;

    if (reg > 7) {
        return INVALID_INSTR;
    }
    if (bit > 31) {
        return INVALID_INSTR;
    }
    if (post > 3) {
        return INVALID_INSTR;
    }
    instr = 3 << POS_OP;
    instr |= (addr << 0);
    instr |= (reg << 8);
    instr |= (bit << 11);
    instr |= (post << 16);
    if (always) {
        instr |= (1 << 18);
    } else {
        instr &= ~(1 << 18);
        if (cond) {
            instr |= (1 << 19);
        } else {
            instr &= ~(1 << 19);
        }
    }
    instr |= side;
    return instr;
}

uint32_t pec_instr_JBZ(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t post, uint32_t side)
{
    return jmp_bit(addr, reg, bit, post, side, 0, 0);
}

uint32_t pec_instr_JBNZ(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t post, uint32_t side)
{
    return jmp_bit(addr, reg, bit, post, side, 1, 0);
}

uint32_t pec_instr_JBA(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t post, uint32_t side)
{
    return jmp_bit(addr, reg, bit, post, side, 0, 1);
}

uint32_t pec_instr_GOTO(uint8_t addr, uint32_t side)
{
    return jmp_bit(addr, 0, 0, PEC_INSTR_BIT_NONE, side, 0, 1);
}

static uint32_t out_in(uint8_t reg, uint8_t bit, uint8_t pin_base, uint8_t dir, uint32_t side)
{
    uint32_t instr;

    instr = 4 << POS_OP;
    if (dir) {
        instr |= (1 << 19); /* shift from OSR to reg */
        if (reg <= 10 && bit != 0 && bit <= 32) {
            instr |= (reg << 5);
            instr |= ((bit - 1) << 0);
        } else if (reg == 11 && bit == 8) {
            instr |= (reg << 5);
            instr |= ((bit - 1) << 0);
        } else if (reg == 12 && bit == 32) {
            instr |= (reg << 5);
            instr |= ((bit - 1) << 0);
        } else {
            return INVALID_INSTR;
        }
        if (reg == 9 || reg == 10) {
            instr |= (pin_base << 9);
        }
    } else {
        instr &= ~(1 << 19); /* shift from reg to ISR */
        if (reg <= 10 && bit != 0 && bit <= 32) {
            instr |= (reg << 5);
            instr |= ((bit - 1) << 0);
        } else {
            return INVALID_INSTR;
        }
        if (reg == 10) {
            instr |= (pin_base << 9);
        }
    }
    instr |= side;
    return instr;
}

uint32_t pec_instr_OUTREG(uint8_t reg, uint8_t bit, uint32_t side)
{
    return out_in(reg, bit, 0, 1, side);
}

uint32_t pec_instr_OUTNULL(uint8_t bit, uint32_t side)
{
    return out_in(8, bit, 0, 1, side);
}

uint32_t pec_instr_OUTPINVAL(uint8_t pin_base, uint8_t bit, uint32_t side)
{
    return out_in(9, bit, pin_base, 1, side);
}

uint32_t pec_instr_OUTPINDIR(uint8_t pin_base, uint8_t bit, uint32_t side)
{
    return out_in(10, bit, pin_base, 1, side);
}

uint32_t pec_instr_OUTPC(uint8_t bit, uint32_t side)
{
    return out_in(11, 8, 0, 1, side);
}

uint32_t pec_instr_OUTEXEC(uint32_t side)
{
    return out_in(12, 32, 0, 1, side);
}

uint32_t pec_instr_INREG(uint8_t reg, uint8_t bit, uint32_t side)
{
    return out_in(reg, bit, 0, 0, side);
}

uint32_t pec_instr_IN0S(uint8_t bit, uint32_t side)
{
    return out_in(8, bit, 0, 0, side);
}

uint32_t pec_instr_IN1S(uint8_t bit, uint32_t side)
{
    return out_in(9, bit, 0, 0, side);
}

uint32_t pec_instr_INPINVAL(uint8_t pin_base, uint8_t bit, uint32_t side)
{
    return out_in(10, bit, pin_base, 0, side);
}

static uint32_t pull_push(uint8_t dir, uint8_t full_empty, uint8_t block, uint8_t reg, uint8_t drop, uint32_t side)
{
    uint32_t instr;

    instr = 5 << POS_OP;
    if (full_empty) {
        instr |= (1 << 18);
    }
    if (block) {
        instr |= (1 << 17);
    }
    if (dir) {
        instr |= (1 << 19); /* pull data from tx_fifo to OSR */
        if (block == 0) {
            if (reg <= 10) {
                instr |= (reg << 0);
            } else {
                return INVALID_INSTR;
            }
        }
    } else {
        instr &= ~(1 << 19); /* push data from ISR to rx_fifo */
        if (block == 0) {
            if (drop == PEC_INSTR_PUSH_DROP) {
                instr |= (1 << 0);
            } else if (drop == PEC_INSTR_PUSH_FORCE) {
                instr &= ~(1 << 0);
            } else {
                return INVALID_INSTR;
            }
        }
    }
    instr |= side;
    return instr;
}

uint32_t pec_instr_PULL(uint8_t replace, uint32_t side)
{
    return pull_push(1, 0, 0, replace, 0, side);
}

uint32_t pec_instr_PULLE(uint8_t replace, uint32_t side)
{
    return pull_push(1, 1, 0, replace, 0, side);
}

uint32_t pec_instr_PULLB(uint32_t side)
{
    return pull_push(1, 0, 1, 0, 0, side);
}

uint32_t pec_instr_PULLEB(uint32_t side)
{
    return pull_push(1, 1, 1, 0, 0, side);
}

uint32_t pec_instr_PUSH(uint8_t drop, uint32_t side)
{
    return pull_push(0, 0, 0, 0, drop, side);
}

uint32_t pec_instr_PUSHF(uint8_t drop, uint32_t side)
{
    return pull_push(0, 1, 0, 0, drop, side);
}

uint32_t pec_instr_PUSHB(uint32_t side)
{
    return pull_push(0, 0, 1, 0, 0, side);
}

uint32_t pec_instr_PUSHFB(uint32_t side)
{
    return pull_push(0, 1, 1, 0, 0, side);
}

uint32_t pec_instr_MOV(uint8_t dst, uint8_t src, uint8_t bit, uint8_t cnt, uint32_t side)
{
    uint32_t instr;

    instr = 6 << POS_OP;
    if (bit > 31) {
        return INVALID_INSTR;
    }
    if ((cnt == 0) || ((cnt > 16) && (cnt < 32)) || (cnt > 32)) {
        return INVALID_INSTR;
    }
    if (bit + cnt > 32) {
        return INVALID_INSTR;
    }
    if (src > 14 || dst > 15) {
        return INVALID_INSTR;
    }
    instr |= (src << 0);
    instr |= (dst << 7);
    instr |= (bit << 11);
    instr |= ((cnt - 1) << 16);
    instr |= side;
    return instr;
}

uint32_t pec_instr_MOVREG(uint8_t dst, uint8_t src, uint8_t op, uint32_t side)
{
    uint32_t instr;

    instr = 6 << POS_OP;
    if (src > 7 || dst > 7) {
        return INVALID_INSTR;
    }
    if (op > 7) {
        return INVALID_INSTR;
    }
    instr |= (src << 0);
    instr |= (op << 4);
    instr |= (dst << 7);
    instr |= (0x1F << 11); /* bit position */
    instr |= (0xF << 16);  /* bit count */
    instr |= side;
    return instr;
}

uint32_t pec_instr_SET(uint8_t dst, uint16_t imm, uint32_t side)
{
    uint32_t instr;

    instr = 7 << POS_OP;
    if (dst > 15) {
        return INVALID_INSTR;
    }
    instr |= imm << 0;
    instr |= (dst << 16);
    instr |= side;
    return instr;
}

static uint32_t ipc(uint8_t addr, uint8_t bit, uint8_t write, uint8_t set, uint8_t sm, uint8_t toggle, uint32_t side)
{
    uint32_t instr;

    instr = 8 << POS_OP;
    if (bit > 31) {
        return INVALID_INSTR;
    }
    instr |= (addr << 0);
    instr |= (bit << 8);
    if (write) {
        instr |= (1 << 18);
    } else {
        instr &= ~(1 << 18);
    }
    if (set) {
        instr |= (1 << 19);
    } else {
        instr &= ~(1 << 19);
    }
    if (sm) {
        instr |= (1 << 14);
    } else {
        instr &= ~(1 << 14);
    }
    if (toggle) {
        instr |= (1 << 13);
    } else {
        instr &= ~(1 << 13);
    }
    instr |= side;
    return instr;
}

uint32_t pec_instr_IPCS(uint8_t bit, uint32_t side)
{
    return ipc(0, bit, 1, 1, 0, 0, side);
}

uint32_t pec_instr_IPCC(uint8_t bit, uint32_t side)
{
    return ipc(0, bit, 1, 0, 0, 0, side);
}

uint32_t pec_instr_IPCSM(uint8_t bit, uint32_t side)
{
    return ipc(0, bit, 1, 1, 1, 0, side);
}

uint32_t pec_instr_IPCCM(uint8_t bit, uint32_t side)
{
    return ipc(0, bit, 1, 0, 1, 0, side);
}

uint32_t pec_instr_JIPCS(uint8_t addr, uint8_t bit, uint8_t toggle, uint32_t side)
{
    return ipc(addr, bit, 0, 1, 0, toggle, side);
}

uint32_t pec_instr_JIPCC(uint8_t addr, uint8_t bit, uint8_t toggle, uint32_t side)
{
    return ipc(addr, bit, 0, 0, 0, toggle, side);
}

uint32_t pec_instr_JIPCSM(uint8_t addr, uint8_t bit, uint8_t toggle, uint32_t side)
{
    return ipc(addr, bit, 0, 1, 1, toggle, side);
}

uint32_t pec_instr_JIPCCM(uint8_t addr, uint8_t bit, uint8_t toggle, uint32_t side)
{
    return ipc(addr, bit, 0, 0, 1, toggle, side);
}

static uint32_t calc_reg(uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t op, uint32_t side)
{
    uint32_t instr;

    instr = 9 << POS_OP;
    if (rd > 7 || rs1 > 7 || rs2 > 7) {
        return INVALID_INSTR;
    }
    if (op > 0x3F) {
        return INVALID_INSTR;
    }
    instr |= (rs1 << 0);
    instr |= (rs2 << 3);
    instr |= (rd << 11);
    instr |= (op << 14);
    instr |= side;
    return instr;
}

static uint32_t calc_imm(uint8_t rd, uint8_t rs, uint8_t imm, uint8_t op, uint32_t side)
{
    uint32_t instr;

    instr = 9 << POS_OP;
    if (rd > 7 || rs > 7) {
        return INVALID_INSTR;
    }
    if (op > 0x3F) {
        return INVALID_INSTR;
    }
    instr |= (rs << 0);
    instr |= (imm << 3);
    instr |= (rd << 11);
    instr |= (op << 14);
    instr |= side;
    return instr;
}

uint32_t pec_instr_AND(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 0, side);
}

uint32_t pec_instr_ANDI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 1, side);
}

uint32_t pec_instr_OR(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 2, side);
}

uint32_t pec_instr_ORI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 3, side);
}

uint32_t pec_instr_XOR(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 4, side);
}

uint32_t pec_instr_XORI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 5, side);
}

uint32_t pec_instr_BSET(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 6, side);
}

uint32_t pec_instr_BSETI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 7, side);
}

uint32_t pec_instr_BCLR(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 8, side);
}

uint32_t pec_instr_BCLRI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 9, side);
}

uint32_t pec_instr_BINV(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 10, side);
}

uint32_t pec_instr_BINVI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 11, side);
}

uint32_t pec_instr_ADD(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 12, side);
}

uint32_t pec_instr_ADDI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 13, side);
}

uint32_t pec_instr_SUB(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 14, side);
}

uint32_t pec_instr_SUBI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 15, side);
}

uint32_t pec_instr_SLL(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 16, side);
}

uint32_t pec_instr_SLLI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 17, side);
}

uint32_t pec_instr_SRL(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 18, side);
}

uint32_t pec_instr_SRLI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 19, side);
}

uint32_t pec_instr_ROR(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 20, side);
}

uint32_t pec_instr_RORI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side)
{
    return calc_imm(rd, rs, imm, 21, side);
}

uint32_t pec_instr_MIN(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 22, side);
}

uint32_t pec_instr_MAX(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side)
{
    return calc_reg(rd, rs1, rs2, 23, side);
}

uint32_t pec_instr_CLZ(uint8_t rd, uint8_t rs, uint32_t side)
{
    return calc_reg(rd, rs, 0, 24, side);
}

uint32_t pec_instr_CTZ(uint8_t rd, uint8_t rs, uint32_t side)
{
    return calc_reg(rd, rs, 0, 25, side);
}

uint32_t pec_instr_NOP(uint8_t reg, uint32_t side)
{
    return pec_instr_MOVREG(reg, reg, PEC_INSTR_MOV_OP_NONE, side);
}
