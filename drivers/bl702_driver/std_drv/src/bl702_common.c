/**
  ******************************************************************************
  * @file    bl702_common.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#include "l1c_reg.h"
#include "bl702_common.h"

/** @addtogroup  BL702_Periph_Driver
 *  @{
 */

/** @defgroup DRIVER_COMMON DRIVER_COMMON
 *  @brief Digger driver common functions
 *  @{
 */

/** @defgroup DRIVER_Private_Type
 *  @{
 */


/*@} end of group DRIVER_Private_Type*/

/** @defgroup DRIVER_Private_Defines
 *  @{
 */

/*@} end of group DRIVER_Private_Defines */

/** @defgroup DRIVER_Private_Variables
 *  @{
 */
pFunc __Interrupt_Handlers[IRQn_LAST]={0};

/*@} end of group DRIVER_Private_Variables */

/** @defgroup DRIVER_Global_Variables
 *  @{
 */

/*@} end of group DRIVER_Global_Variables */

/** @defgroup DRIVER_Private_FunctionDeclaration
 *  @{
 */

/*@} end of group DRIVER_Private_FunctionDeclaration */

/** @defgroup DRIVER_Private_Functions
 *  @{
 */

/*@} end of group DRIVER_Private_Functions */

/** @defgroup DRIVER_Public_Functions
 *  @{
 */

void Trap_Handler(void){
    unsigned long cause;
    unsigned long epc;
    unsigned long tval;
    uint8_t isecall=0;

    MSG("Trap_Handler\r\n");

    cause = read_csr(mcause);
    MSG("mcause=%08x\r\n",(uint32_t)cause);
    epc=read_csr(mepc);
    MSG("mepc:%08x\r\n",(uint32_t)epc);
    tval=read_csr(mtval);
    MSG("mtval:%08x\r\n",(uint32_t)tval);

    cause=(cause&0x3ff);
    switch(cause){
        case 1:
            MSG("Instruction access fault\r\n");
            break;
        case 2:
            MSG("Illegal instruction\r\n");
            break;
        case 3:
            MSG("Breakpoint\r\n");
            break;
        case 4:
            MSG("Load address misaligned\r\n");
            break;
        case 5:
            MSG("Load access fault\r\n");
            break;
        case 6:
            MSG("Store/AMO address misaligned\r\n");
            break;
        case 7:
            MSG("Store/AMO access fault\r\n");
            break;
        case 8:
            MSG("Environment call from U-mode\r\n");
            epc+=4;
            write_csr(mepc,epc);
            break;
        case 11:
            MSG("Environment call from M-mode\r\n");
            epc+=4;
            write_csr(mepc,epc);
            isecall=1;
            break;
        default:
            MSG("Cause num=%d\r\n",(uint32_t)cause);
            epc+=4;
            write_csr(mepc,epc);
            break;
    }

    if(!isecall){
        while(1);
    }
}

void Interrupt_Handler_Register(IRQn_Type irq,pFunc interruptFun)
{
    if(irq<IRQn_LAST){
        __Interrupt_Handlers[irq]=interruptFun;
    }
}

void Interrupt_Handler(void)
{
    pFunc interruptFun;
    uint32_t num=0;
    volatile uint32_t ulMEPC = 0UL, ulMCAUSE = 0UL;

    /* Store a few register values that might be useful when determining why this
    function was called. */
    __asm volatile( "csrr %0, mepc" : "=r"( ulMEPC ) );
    __asm volatile( "csrr %0, mcause" : "=r"( ulMCAUSE ) );
    
    if((ulMCAUSE&0x80000000)==0){
        /*Exception*/
        MSG("Exception should not be here\r\n");
    }else{
        num=ulMCAUSE&0x3FF;
        if(num<IRQn_LAST){
            interruptFun=__Interrupt_Handlers[num];
            if(NULL!=interruptFun){
                interruptFun();
            }else{
                MSG("Interrupt num:%d IRQHandler not installed\r\n",(unsigned int)num);
                if(num>=IRQ_NUM_BASE){
                    MSG("Peripheral Interrupt num:%d \r\n",(unsigned int)num-IRQ_NUM_BASE);
                }
                while(1);
            }
        }else{
            MSG("Unexpected interrupt num:%d\r\n",(unsigned int)num);
        }
    }
}

void FreeRTOS_Interrupt_Handler(void)
{
    Interrupt_Handler();
}

/****************************************************************************//**
 * @brief      delay us 
 *
 * @param[in]  core:  systemcoreclock
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
#ifdef ARCH_ARM
#ifndef __GNUC__
__WEAK
__ASM void ATTR_TCM_SECTION ASM_Delay_Us(uint32_t core,uint32_t cnt)
{
    lsrs r0,#0x10
    muls r0,r1,r0
    mov  r2,r0
    lsrs r2,#0x04
    lsrs r2,#0x03
    cmp  r2,#0x01
    beq  end
    cmp  r2,#0x00
    beq  end
loop
    mov  r0,r0
    mov  r0,r0
    mov  r0,r0
    mov  r0,r0
    mov  r0,r0
    subs r2,r2,#0x01
    cmp  r2,#0x00
    bne  loop
end
    bx   lr
}
#else
__WEAK
void ATTR_TCM_SECTION ASM_Delay_Us(uint32_t core,uint32_t cnt)
{
    __asm__ __volatile__(
        "lsr r0,#0x10\n\t"
        "mul r0,r1,r0\n\t"
        "mov r2,r0\n\t"
        "lsr r2,#0x04\n\t"
        "lsr r2,#0x03\n\t"
        "cmp r2,#0x01\n\t"
        "beq end\n\t"
        "cmp r2,#0x00\n\t"
        "beq end\n"
        "loop :"
        "mov r0,r0\n\t"
        "mov r0,r0\n\t"
        "mov r0,r0\n\t"
        "mov r0,r0\n\t"
        "mov r0,r0\n\t"
        "sub r2,r2,#0x01\n\t"
        "cmp r2,#0x00\n\t"
        "bne loop\n"
        "end :"
        "mov r0,r0\n\t"
    );
}
#endif
#endif
#ifdef ARCH_RISCV
__WEAK
void ATTR_TCM_SECTION ASM_Delay_Us(uint32_t core,uint32_t cnt)
{
    uint32_t codeAddress = 0;
    uint32_t divVal = 40;
    
    codeAddress = (uint32_t)&ASM_Delay_Us;
    
    /* 1M=100K*10, so multiple is 10 */
    /* loop function take 4 instructions, so instructionNum is 4 */
    /* if codeAddress locate at IROM space and irom_2t_access is 1, then irom2TAccess=2, else irom2TAccess=1 */
    /* divVal = multiple*instructionNum*irom2TAccess */
    if(((codeAddress&(0xF<<24))>>24)==0x01){
        /* IROM space */
        if(BL_GET_REG_BITS_VAL(BL_RD_REG(L1C_BASE,L1C_CONFIG),L1C_IROM_2T_ACCESS)){
            /* instruction 2T */
            divVal = 80;
        }
    }
    
    __asm__ __volatile__(
        ".align 4\n\t"
        "lw       a4,%1\n\t"
        "lui   a5,0x18\n\t"
        "addi  a5,a5,1696\n\t"
        "divu  a5,a4,a5\n\t"
        "sw       a5,%1\n\t"
        "lw       a4,%1\n\t"
        "lw       a5,%0\n\t"
        "mul   a5,a4,a5\n\t"
        "sw       a5,%1\n\t"
        "lw       a4,%1\n\t"
        "lw       a5,%2\n\t"
        "divu  a5,a4,a5\n\t"
        "sw    a5,%1\n\t"
        "lw    a5,%1\n\t"
        "li    a4,0x1\n\t"
        "beq   a5,zero,end\n\t"
        "beq   a5,a4,end\n\t"
        "nop\n\t"
        "nop\n\t"
        ".align 4\n\t"
        "loop  :\n"
        "addi  a4,a5,-1\n\t"
        "mv    a5,a4\n\t"
        "bnez  a5,loop\n\t"
        "nop\n\t"
        "end   :\n\t"
        "nop\n"
        :                                              /* output */
        :"m"(cnt),"m"(core),"m"(divVal)                /* input */
        :"t1","a4","a5"                                /* destruct description */
    );
}
#endif
#endif

/****************************************************************************//**
 * @brief      delay us 
 *
 * @param[in]  cnt:  delay cnt us
 *
 * @return none
 *
 *******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION BL702_Delay_US(uint32_t cnt)
{
    ASM_Delay_Us(SystemCoreClockGet(),cnt);
}
#endif
/****************************************************************************//**
 * @brief      delay ms 
 *
 * @param[in]  cnt:  delay cnt ms
 *
 * @return none
 *
 *******************************************************************************/
#ifndef BFLB_USE_ROM_DRIVER
__WEAK
void ATTR_TCM_SECTION BL702_Delay_MS(uint32_t cnt)
{
    uint32_t i = 0;
    uint32_t count = 0;
    
    if(cnt>=1024){
        /* delay (n*1024) ms */
        count = 1024;
        for(i=0;i<(cnt/1024);i++){
            BL702_Delay_US(1024*1000);
        }
    }
    if(cnt&0x3FF){
        /* delay (1-1023)ms */
        count = cnt & 0x3FF;
        BL702_Delay_US(count*1000);
    }
    //BL702_Delay_US(((cnt<<10)-(cnt<<4)-(cnt<<3)));
}
#endif
/*
char *_sbrk(int incr)
{}
*/
/*@} end of group DRIVER_Public_Functions */

/*@} end of group DRIVER_COMMON */

/*@} end of group BL702_Periph_Driver */


