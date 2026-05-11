/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
/*
 * a0: compressed data pool
 * a1: decompressed data pool
 * a2: total lengeth of the compressed data file
 */
/* void unlz4(const void *aSource, void *aDestination, uint32_t FileLen); */

#if !defined(BFLB_LZ4_NO_XTHEAD) && (defined(__riscv_xthead) || defined(__riscv_xtheadc))
.macro lbu_inc dst, ptr
                    lbuia               \dst, (\ptr), 1, 0
.endm

.macro sb_inc src, ptr
                    sbia                \src, (\ptr), 1, 0
.endm
#else
.macro lbu_inc dst, ptr
                    lbu                 \dst, 0(\ptr)
                    addi                \ptr, \ptr, 1
.endm

.macro sb_inc src, ptr
                    sb                  \src, 0(\ptr)
                    addi                \ptr, \ptr, 1
.endm
#endif

                    .func               unlz4

                    .global             unlz4

                    .type               unlz4,%function

                    .align              6
                    .section .tcm_code.unlz4.text
unlz4:
                    addi                sp, sp, -64         /* save fp and return-address */
                    sw                  ra, 60(sp)
                    sw                  s0, 56(sp)
                    /* addi                s0, sp, 64 */    /* no frame pointer needed */

                    sw                  s1, 0(sp)
                    sw                  s2, 4(sp)
                    sw                  s3, 8(sp)
                    sw                  s4, 12(sp)
                    sw                  s5, 16(sp)
                    sw                  s6, 20(sp)
                    sw                  s7, 24(sp)
                    sw                  s8, 28(sp)
                    sw                  s9, 32(sp)
                    sw                  s10, 36(sp)
                    sw                  s11, 40(sp)

                    mv                  s0, a0
                    mv                  s1, a1
                    add                 s9, a2,a0           /* s9 = end of the compressed data */

                    addi                s0, s0, 7           /* !!! unaligned access needed !!! */
                    addi                s9, s9, -8          /* remove EoS and checksum */

loadConst:          li                  s10, 0xf            /* getLength */
                    li                  s11, 0xff           /* getLengthLoop */

blockStart:
                    lw                  s2, 0(s0)           /* get block length of compressed data */
                                                            /* !!! unaligned access needed !!! */
                                                            /* s2 = compressed data len of current block */

                    addi                s0,s0,4             /* advance source pointer, s0 -> first token */

                    add                 s5,s2,s0            /* point a5 to end of compressed data */

getToken:
                    lbu_inc             s6, s0              /* get token */

                                                            /* advance source pointer */

                    srli                s4,s6, 4            /* get literal length, keep token in a6 */

                    and                 s8,s6, s10          /* match len in token: low 4-bit */

                    beqz                s4, getOffset       /* jump forward if there are no literals */

getLiteralLen:                                              /* if length is 15, then more length info follows */

                    bne                 s10,s4,gotLiteralLen/* jump forward if we have the complete length */

getLiteralLenLoop:  lbu_inc             s3, s0              /* read another byte */

                                                            /* advance source pointer */

                                                            /* check if end reached */

                    add                 s4,s4,s3            /* add byte to length */

                                                            /* if 0xff, get next length byte */
                    beq                 s11,s3,getLiteralLenLoop
gotLiteralLen:

                                                            /* copy literal */
copy_literal:
                    lbu_inc             s3, s0
                    addi                s4, s4, -1
                    sb_inc              s3, s1
                    beqz                s4, copy_literal_done
                    lbu_inc             s3, s0
                    addi                s4, s4, -1
                    sb_inc              s3, s1
                    bnez                s4, copy_literal
copy_literal_done:

                    mv                  s2,s0               /* update index */

                    beq                 s5,s2,blockDone     /* no match data for last token in block */

getOffset:
                    lhu                 s7, 0(s0)           /* !!! unaligned access needed !!! */
                    addi                s0, s0, 2
                    sub                 s2, s1, s7          /* subtract from destination; this will become the match position */


getMatchLen:
                                                            /* if length is 15, then more length info follows */
                    bne                 s10,s8,gotMatchLen  /* jump forward if we have the complete length */

getMatchLenLoop:    lbu_inc             s3, s0              /* read another byte */

                                                            /* advance source pointer */

                                                            /* check if end reached */

                    add                 s8,s8,s3            /* add byte to length */

                                                            /* if 0xff, get another match len byte */
                    beq                 s11,s3,getMatchLenLoop
gotMatchLen:

                                                            /* minimum match length is 4 bytes */
                                                            /* copy 4 bytes */
copy_4bytes:
                    lbu_inc             s3, s2
                    sb_inc              s3, s1
                    lbu_inc             s3, s2
                    sb_inc              s3, s1
                    lbu_inc             s3, s2
                    sb_inc              s3, s1
                    lbu_inc             s3, s2
                    sb_inc              s3, s1

                    beqz                s8, copy_match_done
                                                            /* copy left match data */
copy_match:
                    lbu_inc             s3, s2
                    addi                s8, s8, -1
                    sb_inc              s3, s1
                    bnez                s8, copy_match

copy_match_done:
                                                            /* check if we've reached the end of the compressed data */
                    blt                 s0,s5,getToken      /* if not, go get the next token */

blockDone:
                    blt                 s0,s9,blockStart    /* there is another block of compressed data */

unlz4_exit:
                    lw                  s1, 0(sp)
                    lw                  s2, 4(sp)
                    lw                  s3, 8(sp)
                    lw                  s4, 12(sp)
                    lw                  s5, 16(sp)
                    lw                  s6, 20(sp)
                    lw                  s7, 24(sp)
                    lw                  s8, 28(sp)
                    lw                  s9, 32(sp)
                    lw                  s10, 36(sp)
                    lw                  s11, 40(sp)

                    lw                  ra, 60(sp)
                    lw                  s0, 56(sp)
                    addi                sp, sp, 64
                    ret                                     /* restore fp, then return */

                    .size               unlz4,.-unlz4

                    .endfunc
