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
                    lbuia               s6,(s0),1,0         /* get token */

                                                            /* advance source pointer */

                    srli                s4,s6, 4            /* get literal length, keep token in a6 */

                    and                 s8,s6, s10          /* match len in token: low 4-bit */

                    beqz                s4, getOffset       /* jump forward if there are no literals */

getLiteralLen:                                              /* if length is 15, then more length info follows */

                    bne                 s10,s4,gotLiteralLen/* jump forward if we have the complete length */

getLiteralLenLoop:  lbuia               s3,(s0),1,0         /* read another byte */

                                                            /* advance source pointer */

                                                            /* check if end reached */

                    add                 s4,s4,s3            /* add byte to length */

                                                            /* if 0xff, get next length byte */
                    beq                 s11,s3,getLiteralLenLoop
gotLiteralLen:

                                                            /* copy literal */
copy_literal:
                    lbuia               s3, (s0), 1, 0
                    addi                s4, s4, -1
                    sbia                s3, (s1), 1, 0
                    beqz                s4, copy_literal_done
                    lbuia               s3, (s0), 1, 0
                    addi                s4, s4, -1
                    sbia                s3, (s1), 1, 0
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

getMatchLenLoop:    lbuia               s3,(s0),1,0         /* read another byte */

                                                            /* advance source pointer */

                                                            /* check if end reached */

                    add                 s8,s8,s3            /* add byte to length */

                                                            /* if 0xff, get another match len byte */
                    beq                 s11,s3,getMatchLenLoop
gotMatchLen:

                                                            /* minimum match length is 4 bytes */
                                                            /* copy 4 bytes */
copy_4bytes:
                    lbuia               s3, (s2), 1, 0
                    sbia                s3, (s1), 1, 0
                    lbuia               s3, (s2), 1, 0
                    sbia                s3, (s1), 1, 0
                    lbuia               s3, (s2), 1, 0
                    sbia                s3, (s1), 1, 0
                    lbuia               s3, (s2), 1, 0
                    sbia                s3, (s1), 1, 0

                    beqz                s8, copy_match_done
                                                            /* copy left match data */
copy_match:
                    lbuia               s3, (s2), 1, 0
                    addi                s8, s8, -1
                    sbia                s3, (s1), 1, 0
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
