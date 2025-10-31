#ifndef __BFLB_PEC_V2_INSTR_H__
#define __BFLB_PEC_V2_INSTR_H__

#include "stdint.h"

#define PEC_R0                                                 (0)
#define PEC_R1                                                 (1)
#define PEC_R2                                                 (2)
#define PEC_R3                                                 (3)
#define PEC_R4                                                 (4)
#define PEC_R5                                                 (5)
#define PEC_R6                                                 (6)
#define PEC_R7                                                 (7)
#define PEC_ISR                                                (6)
#define PEC_OSR                                                (7)

#define PEC_INSTR_JRS_ISRE                                     (8)
#define PEC_INSTR_JRS_ISRNE                                    (9)
#define PEC_INSTR_JRS_OSRE                                     (10)
#define PEC_INSTR_JRS_OSRNE                                    (11)
#define PEC_INSTR_JRS_ISRF                                     (12)
#define PEC_INSTR_JRS_ISRNF                                    (13)
#define PEC_INSTR_JRS_OSRF                                     (14)
#define PEC_INSTR_JRS_OSRNF                                    (15)

#define PEC_INSTR_CMP_NONE                                     (0)
#define PEC_INSTR_CMP_DEC                                      (1)
#define PEC_INSTR_CMP_INC                                      (2)

#define PEC_INSTR_BIT_NONE                                     (0)
#define PEC_INSTR_BIT_CLR                                      (1)
#define PEC_INSTR_BIT_SET                                      (2)
#define PEC_INSTR_BIT_TOG                                      (3)

#define PEC_INSTR_PULL_R0                                      (0)
#define PEC_INSTR_PULL_R1                                      (1)
#define PEC_INSTR_PULL_R2                                      (2)
#define PEC_INSTR_PULL_R3                                      (3)
#define PEC_INSTR_PULL_R4                                      (4)
#define PEC_INSTR_PULL_R5                                      (5)
#define PEC_INSTR_PULL_R6                                      (6)
#define PEC_INSTR_PULL_R7                                      (7)
#define PEC_INSTR_PULL_ISR                                     (6)
#define PEC_INSTR_PULL_OSR                                     (7)
#define PEC_INSTR_PULL_0S                                      (8)
#define PEC_INSTR_PULL_1S                                      (9)
#define PEC_INSTR_PULL_PRE                                     (10)

#define PEC_INSTR_PUSH_FORCE                                   (0)
#define PEC_INSTR_PUSH_DROP                                    (1)

#define PEC_INSTR_MOV_SRC_R0                                   (0)
#define PEC_INSTR_MOV_SRC_R1                                   (1)
#define PEC_INSTR_MOV_SRC_R2                                   (2)
#define PEC_INSTR_MOV_SRC_R3                                   (3)
#define PEC_INSTR_MOV_SRC_R4                                   (4)
#define PEC_INSTR_MOV_SRC_R5                                   (5)
#define PEC_INSTR_MOV_SRC_R6                                   (6)
#define PEC_INSTR_MOV_SRC_R7                                   (7)
#define PEC_INSTR_MOV_SRC_ISR                                  (6)
#define PEC_INSTR_MOV_SRC_OSR                                  (7)
#define PEC_INSTR_MOV_SRC_0S                                   (8)
#define PEC_INSTR_MOV_SRC_1S                                   (9)
#define PEC_INSTR_MOV_SRC_PINVAL_0                             (10)
#define PEC_INSTR_MOV_SRC_PINVAL_1                             (11)
#define PEC_INSTR_MOV_SRC_PINVAL_2                             (12)
#define PEC_INSTR_MOV_SRC_PINVAL_3                             (13)
#define PEC_INSTR_MOV_SRC_PINVAL_4                             (14)

#define PEC_INSTR_MOV_DST_R0                                   (0)
#define PEC_INSTR_MOV_DST_R1                                   (1)
#define PEC_INSTR_MOV_DST_R2                                   (2)
#define PEC_INSTR_MOV_DST_R3                                   (3)
#define PEC_INSTR_MOV_DST_R4                                   (4)
#define PEC_INSTR_MOV_DST_R5                                   (5)
#define PEC_INSTR_MOV_DST_R6                                   (6)
#define PEC_INSTR_MOV_DST_R7                                   (7)
#define PEC_INSTR_MOV_DST_ISR                                  (6)
#define PEC_INSTR_MOV_DST_OSR                                  (7)
#define PEC_INSTR_MOV_DST_PINVAL_0                             (8)
#define PEC_INSTR_MOV_DST_PINDIR_0                             (9)
#define PEC_INSTR_MOV_DST_PINVAL_1                             (10)
#define PEC_INSTR_MOV_DST_PINDIR_1                             (11)
#define PEC_INSTR_MOV_DST_PINVAL_2                             (12)
#define PEC_INSTR_MOV_DST_PINDIR_2                             (13)
#define PEC_INSTR_MOV_DST_EXEC                                 (14)
#define PEC_INSTR_MOV_DST_PC                                   (15)

#define PEC_INSTR_MOV_OP_NONE                                  (0)
#define PEC_INSTR_MOV_OP_INV                                   (1)
#define PEC_INSTR_MOV_OP_EX16                                  (2)
#define PEC_INSTR_MOV_OP_EX816                                 (3)
#define PEC_INSTR_MOV_OP_EX832                                 (4)
#define PEC_INSTR_MOV_OP_REV8                                  (5)
#define PEC_INSTR_MOV_OP_REV16                                 (6)
#define PEC_INSTR_MOV_OP_REV32                                 (7)

#define PEC_INSTR_SET_DST_R0                                   (0)
#define PEC_INSTR_SET_DST_R1                                   (1)
#define PEC_INSTR_SET_DST_R2                                   (2)
#define PEC_INSTR_SET_DST_R3                                   (3)
#define PEC_INSTR_SET_DST_R4                                   (4)
#define PEC_INSTR_SET_DST_R5                                   (5)
#define PEC_INSTR_SET_DST_R6                                   (6)
#define PEC_INSTR_SET_DST_R7                                   (7)
#define PEC_INSTR_SET_DST_ISR                                  (6)
#define PEC_INSTR_SET_DST_OSR                                  (7)
#define PEC_INSTR_SET_DST_PINVAL_0                             (8)
#define PEC_INSTR_SET_DST_PINDIR_0                             (9)
#define PEC_INSTR_SET_DST_PINVAL_1                             (10)
#define PEC_INSTR_SET_DST_PINDIR_1                             (11)
#define PEC_INSTR_SET_DST_PINVAL_2                             (12)
#define PEC_INSTR_SET_DST_PINDIR_2                             (13)
#define PEC_INSTR_SET_DST_PINVAL_3                             (14)
#define PEC_INSTR_SET_DST_PINDIR_3                             (15)

#define PEC_PIN_IN_0                                           (0)
#define PEC_PIN_IN_1                                           (1)
#define PEC_PIN_IN_2                                           (2)
#define PEC_PIN_IN_3                                           (3)
#define PEC_PIN_IN_4                                           (4)
#define PEC_PIN_OUT_0                                          (0)
#define PEC_PIN_OUT_1                                          (1)
#define PEC_PIN_OUT_2                                          (2)
#define PEC_PIN_SET_0                                          (0)
#define PEC_PIN_SET_1                                          (1)
#define PEC_PIN_SET_2                                          (2)
#define PEC_PIN_SET_3                                          (3)

#ifdef __cplusplus
extern "C" {
#endif

uint32_t pec_instr_side(uint32_t side, uint32_t delay, uint32_t side_count);

uint32_t pec_instr_JPH(uint8_t addr, uint8_t pin_base, uint8_t offset, uint32_t side);
uint32_t pec_instr_JPL(uint8_t addr, uint8_t pin_base, uint8_t offset, uint32_t side);

uint32_t pec_instr_JRZ(uint8_t addr, uint8_t reg, uint8_t dec, uint32_t side);
uint32_t pec_instr_JRNZ(uint8_t addr, uint8_t reg, uint8_t dec, uint32_t side);
uint32_t pec_instr_JRS(uint8_t addr, uint8_t state, uint32_t side);

uint32_t pec_instr_JEQU(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side);
uint32_t pec_instr_JNEQ(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side);
uint32_t pec_instr_JLET(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side);
uint32_t pec_instr_JLEQ(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side);
uint32_t pec_instr_JHIT(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side);
uint32_t pec_instr_JHEQ(uint8_t addr, uint8_t reg1, uint8_t post1, uint8_t reg2, uint8_t post2, uint32_t side);

uint32_t pec_instr_JBZ(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t post, uint32_t side);
uint32_t pec_instr_JBNZ(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t post, uint32_t side);
uint32_t pec_instr_JBA(uint8_t addr, uint8_t reg, uint8_t bit, uint8_t post, uint32_t side);
uint32_t pec_instr_GOTO(uint8_t addr, uint32_t side);

uint32_t pec_instr_OUTREG(uint8_t reg, uint8_t bit, uint32_t side);
uint32_t pec_instr_OUTNULL(uint8_t bit, uint32_t side);
uint32_t pec_instr_OUTPINVAL(uint8_t pin_base, uint8_t bit, uint32_t side);
uint32_t pec_instr_OUTPINDIR(uint8_t pin_base, uint8_t bit, uint32_t side);
uint32_t pec_instr_OUTPC(uint8_t bit, uint32_t side);
uint32_t pec_instr_OUTEXEC(uint32_t side);
uint32_t pec_instr_INREG(uint8_t reg, uint8_t bit, uint32_t side);
uint32_t pec_instr_IN0S(uint8_t bit, uint32_t side);
uint32_t pec_instr_IN1S(uint8_t bit, uint32_t side);
uint32_t pec_instr_INPINVAL(uint8_t pin_base, uint8_t bit, uint32_t side);

uint32_t pec_instr_PULL(uint8_t replace, uint32_t side);
uint32_t pec_instr_PULLE(uint8_t replace, uint32_t side);
uint32_t pec_instr_PULLB(uint32_t side);
uint32_t pec_instr_PULLEB(uint32_t side);
uint32_t pec_instr_PUSH(uint8_t drop, uint32_t side);
uint32_t pec_instr_PUSHF(uint8_t drop, uint32_t side);
uint32_t pec_instr_PUSHB(uint32_t side);
uint32_t pec_instr_PUSHFB(uint32_t side);

uint32_t pec_instr_MOV(uint8_t dst, uint8_t src, uint8_t bit, uint8_t cnt, uint32_t side);
uint32_t pec_instr_MOVREG(uint8_t dst, uint8_t src, uint8_t op, uint32_t side);

uint32_t pec_instr_SET(uint8_t dst, uint16_t imm, uint32_t side);

uint32_t pec_instr_IPCS(uint8_t bit, uint32_t side);
uint32_t pec_instr_IPCC(uint8_t bit, uint32_t side);
uint32_t pec_instr_IPCSM(uint8_t bit, uint32_t side);
uint32_t pec_instr_IPCCM(uint8_t bit, uint32_t side);
uint32_t pec_instr_JIPCS(uint8_t addr, uint8_t bit, uint8_t toggle, uint32_t side);
uint32_t pec_instr_JIPCC(uint8_t addr, uint8_t bit, uint8_t toggle, uint32_t side);
uint32_t pec_instr_JIPCSM(uint8_t addr, uint8_t bit, uint8_t toggle, uint32_t side);
uint32_t pec_instr_JIPCCM(uint8_t addr, uint8_t bit, uint8_t toggle, uint32_t side);

uint32_t pec_instr_AND(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_ANDI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_OR(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_ORI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_XOR(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_XORI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_BSET(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_BSETI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_BCLR(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_BCLRI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_BINV(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_BINVI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_ADD(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_ADDI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_SUB(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_SUBI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_SLL(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_SLLI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_SRL(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_SRLI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_ROR(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_RORI(uint8_t rd, uint8_t rs, uint8_t imm, uint32_t side);
uint32_t pec_instr_MIN(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_MAX(uint8_t rd, uint8_t rs1, uint8_t rs2, uint32_t side);
uint32_t pec_instr_CLZ(uint8_t rd, uint8_t rs, uint32_t side);
uint32_t pec_instr_CTZ(uint8_t rd, uint8_t rs, uint32_t side);

uint32_t pec_instr_NOP(uint8_t reg, uint32_t side);

#ifdef __cplusplus
}
#endif

#endif /* __BFLB_PEC_V2_INSTR_H__ */
