/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

/* Standard includes. */
#include "string.h"

#ifdef configCLINT_BASE_ADDRESS
	#warning The configCLINT_BASE_ADDRESS constant has been deprecated.  configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS are currently being derived from the (possibly 0) configCLINT_BASE_ADDRESS setting.  Please update to define configMTIME_BASE_ADDRESS and configMTIMECMP_BASE_ADDRESS dirctly in place of configCLINT_BASE_ADDRESS.  See https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef configMTIME_BASE_ADDRESS
	#warning configMTIME_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtime register then set configMTIME_BASE_ADDRESS to the mapped address.  Otherwise set configMTIME_BASE_ADDRESS to 0.  See https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef configMTIMECMP_BASE_ADDRESS
	#warning configMTIMECMP_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtimecmp register then set configMTIMECMP_BASE_ADDRESS to the mapped address.  Otherwise set configMTIMECMP_BASE_ADDRESS to 0.  See https://www.freertos.org/Using-FreeRTOS-on-RISC-V.html
#endif

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
	#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
	#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
to use a statically allocated array as the interrupt stack.  Alternative leave
configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
linker variable names __freertos_irq_stack_top has the same value as the top
of the stack used by main.  Using the linker script method will repurpose the
stack that was used by main before the scheduler was started for use as the
interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
	static __attribute__ ((aligned(16))) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 };
	const StackType_t xISRStackTop = ( StackType_t ) &( xISRStack[ configISR_STACK_SIZE_WORDS & ~portBYTE_ALIGNMENT_MASK ] );

	/* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
	the task stacks, and so will legitimately appear in many positions within
	the ISR stack. */
	#define portISR_STACK_FILL_BYTE	0xee
#else
	extern const uint32_t __freertos_irq_stack_top[];
	const StackType_t xISRStackTop = ( StackType_t ) __freertos_irq_stack_top;
#endif

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt( void ) __attribute__(( weak ));

/*-----------------------------------------------------------*/

/* Used to program the machine timer compare register. */
uint64_t ullNextTime = 0ULL;
const uint64_t *pullNextTime = &ullNextTime;
const size_t uxTimerIncrementsForOneTick = ( size_t ) ( ( configCPU_CLOCK_HZ ) / ( configTICK_RATE_HZ ) ); /* Assumes increment won't go over 32-bits. */
uint32_t const ullMachineTimerCompareRegisterBase = configMTIMECMP_BASE_ADDRESS;
volatile uint32_t * pullMachineTimerCompareRegister = NULL;

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 )
	#warning This path not tested, or even compiled yet.

	static const uint8_t ucExpectedStackBytes[] = {
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
									portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE };	\

	#define portCHECK_ISR_STACK() configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else
	/* Define the function away. */
	#define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

/*-----------------------------------------------------------*/

#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )

	void vPortSetupTimerInterrupt( void )
	{
	uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
	volatile uint32_t * const pulTimeHigh = ( volatile uint32_t * const ) ( ( configMTIME_BASE_ADDRESS ) + 4UL ); /* 8-byte typer so high 32-bit word is 4 bytes up. */
	volatile uint32_t * const pulTimeLow = ( volatile uint32_t * const ) ( configMTIME_BASE_ADDRESS );
	volatile uint32_t ulHartId;

		__asm volatile( "csrr %0, mhartid" : "=r"( ulHartId ) );
		pullMachineTimerCompareRegister  = ( volatile uint32_t * ) ( ullMachineTimerCompareRegisterBase + ( ulHartId * sizeof( uint64_t ) ) );

		do
		{
			ulCurrentTimeHigh = *pulTimeHigh;
			ulCurrentTimeLow = *pulTimeLow;
		} while( ulCurrentTimeHigh != *pulTimeHigh );

		ullNextTime = ( uint64_t ) ulCurrentTimeHigh;
		ullNextTime <<= 32ULL; /* High 4-byte word is 32-bits up. */
		ullNextTime |= ( uint64_t ) ulCurrentTimeLow;
		ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
		pullMachineTimerCompareRegister[0] = (uint32_t)ullNextTime;
		pullMachineTimerCompareRegister[1] = (uint32_t)(ullNextTime >> 32);

		/* Prepare the time to use after the next tick interrupt. */
		ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
	}
#else
	void vPortSetupTimerInterrupt( void )
	{
		uint64_t ulCurrentTime;
		volatile uint32_t ulHartId;

        __asm volatile( "csrr %0, mhartid" : "=r"( ulHartId ) );
        __asm volatile( "csrr %0, time" : "=r"( ulCurrentTime ) );
        pullMachineTimerCompareRegister  = ( volatile uint32_t * ) ( ullMachineTimerCompareRegisterBase + ( ulHartId * sizeof( uint64_t ) ) );

        ullNextTime = ( uint64_t ) ulCurrentTime;
        ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
        pullMachineTimerCompareRegister[0] = (uint32_t)ullNextTime;
        pullMachineTimerCompareRegister[1] = (uint32_t)(ullNextTime >> 32);

        /* Prepare the time to use after the next tick interrupt. */
        ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
	}
#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIME_BASE_ADDRESS != 0 ) */
/*-----------------------------------------------------------*/

/* Used to keep track of the number of nested calls to taskENTER_CRITICAL().  This
will be set to 0 prior to the first task being started. */
portLONG ulCriticalNesting = 0x9999UL;

/* Used to record one tack want to swtich task after enter critical area, we need know it
 * and implement task switch after exit critical area */
portLONG pendsvflag = 0;


StackType_t *pxPortInitialiseStack( StackType_t * pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    StackType_t *stk  = NULL;
    register int *gp __asm("x3");
    uintptr_t temp = (uintptr_t)pxTopOfStack;

    temp &= 0xFFFFFFF8UL;
    stk = (StackType_t *)temp;

#ifndef __riscv_32e
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F31         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F30         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F29         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F28         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F27         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F26         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F25         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F24         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F23         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F22         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F21          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F20          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F19         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F18         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F17         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F16         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F15         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F14         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F13         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F12         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F11         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F10         */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F9          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F8          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F7          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F6          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F5          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F4          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F3          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F2          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F1          */
    *(--stk)  = (uintptr_t)0x1234567812345678L;       /* F0          */
#endif

    *(--stk)  = (uintptr_t)pxCode;                    /* Entry Point */

#ifndef __riscv_32e
    *(--stk)  = (uintptr_t)0x3131313131313131L;       /* X31         */
    *(--stk)  = (uintptr_t)0x3030303030303030L;       /* X30         */
    *(--stk)  = (uintptr_t)0x2929292929292929L;       /* X29         */
    *(--stk)  = (uintptr_t)0x2828282828282828L;       /* X28         */
    *(--stk)  = (uintptr_t)0x2727272727272727L;       /* X27         */
    *(--stk)  = (uintptr_t)0x2626262626262626L;       /* X26         */
    *(--stk)  = (uintptr_t)0x2525252525252525L;       /* X25         */
    *(--stk)  = (uintptr_t)0x2424242424242424L;       /* X24         */
    *(--stk)  = (uintptr_t)0x2323232323232323L;       /* X23         */
    *(--stk)  = (uintptr_t)0x2222222222222222L;       /* X22         */
    *(--stk)  = (uintptr_t)0x2121212121212121L;       /* X21         */
    *(--stk)  = (uintptr_t)0x2020202020202020L;       /* X20         */
    *(--stk)  = (uintptr_t)0x1919191919191919L;       /* X19         */
    *(--stk)  = (uintptr_t)0x1818181818181818L;       /* X18         */
    *(--stk)  = (uintptr_t)0x1717171717171717L;       /* X17         */
    *(--stk)  = (uintptr_t)0x1616161616161616L;       /* X16         */
#endif
    *(--stk)  = (uintptr_t)0x1515151515151515L;       /* X15         */
    *(--stk)  = (uintptr_t)0x1414141414141414L;       /* X14         */
    *(--stk)  = (uintptr_t)0x1313131313131313L;       /* X13         */
    *(--stk)  = (uintptr_t)0x1212121212121212L;       /* X12         */
    *(--stk)  = (uintptr_t)0x1111111111111111L;       /* X11         */
    *(--stk)  = (uintptr_t)pvParameters;              /* X10         */
    *(--stk)  = (uintptr_t)0x0909090909090909L;       /* X9          */
    *(--stk)  = (uintptr_t)0x0808080808080808L;       /* X8          */
    *(--stk)  = (uintptr_t)0x0707070707070707L;       /* X7          */
    *(--stk)  = (uintptr_t)0x0606060606060606L;       /* X6          */
    *(--stk)  = (uintptr_t)0x0505050505050505L;       /* X5          */
    *(--stk)  = (uintptr_t)0x0404040404040404L;       /* X4          */
    *(--stk)  = (uintptr_t)gp;                        /* X3          */
    *(--stk)  = (uintptr_t)0xFFFFFFFFFFFFFFFEL;       /* X1          */

    return stk;
}

BaseType_t xPortStartScheduler( void )
{
	ulCriticalNesting = 0UL;
extern void xPortStartFirstTask( void );

	#if( configASSERT_DEFINED == 1 )
	{
		// volatile uint32_t mtvec = 0;

		// /* Check the least significant two bits of mtvec are 00 - indicating
		// single vector mode. */
		// __asm volatile( "csrr %0, mtvec" : "=r"( mtvec ) );
		// configASSERT( ( mtvec & 0x03UL ) == 0 );

		/* Check alignment of the interrupt stack - which is the same as the
		stack that was being used by main() prior to the scheduler being
		started. */
		configASSERT( ( xISRStackTop & portBYTE_ALIGNMENT_MASK ) == 0 );

		#ifdef configISR_STACK_SIZE_WORDS
		{
			memset( ( void * ) xISRStack, portISR_STACK_FILL_BYTE, sizeof( xISRStack ) );
		}
		#endif	 /* configISR_STACK_SIZE_WORDS */
	}
	#endif /* configASSERT_DEFINED */

	/* If there is a CLINT then it is ok to use the default implementation
	in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
	configure whichever clock is to be used to generate the tick interrupt. */
	//vPortSetupTimerInterrupt();

	#if( ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) )
	{
		/* Enable mtime and external interrupts.  1<<7 for timer interrupt, 1<<11
		for external interrupt.  _RB_ What happens here when mtime is not present as
		with pulpino? */
		__asm volatile( "csrs mie, %0" :: "r"(0x880) );

		/* enable M-mode soft IRQ in CLIC, TODO: priority 2 */
		*(volatile uint8_t *)(0xe0801001 + 3 * 4) = 1;
		*(volatile uint8_t *)(0xe0801003 + 3 * 4) = 0x1f;

		/* enable M-mode timer IRQ in CLIC,TODO: priority 1 */
		*(volatile uint8_t *)(0xe0801001 + 7 * 4) = 1;
		*(volatile uint8_t *)(0xe0801003 + 7 * 4) = 0x2f;
		extern void CORET_IRQHandler(void);
        void Interrupt_Handler_Register(int,void(*)(void));
		Interrupt_Handler_Register(7, CORET_IRQHandler);

		/* TODO: start clock of the mtimer */
    #ifndef __riscv_32e
		*(volatile uint32_t *)0x20009014 = 0x80000001;
    #else
        *(volatile uint32_t *)0x2000E130 = 0x80000001;
    #endif
	}
	#elif( ( configMTIME_BASE_ADDRESS == 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) )
	{
		/* Enable mtime and external interrupts.  1<<7 for timer interrupt, 1<<11
		for external interrupt.  _RB_ What happens here when mtime is not present as
		with pulpino? */
		__asm volatile( "csrs mie, %0" :: "r"(0x888) );
		/* TODO: start clock of the mtimer */
	}
	#else
	{
		/* Enable external interrupts. */
		__asm volatile( "csrs mie, %0" :: "r"(0x800) );
	}
	#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) */

	vPortSetupTimerInterrupt();
	xPortStartFirstTask();

	/* Should not get here as after calling xPortStartFirstTask() only tasks
	should be executing. */
	return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* Not implemented. */
	for( ;; );
}

void vTaskEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    ulCriticalNesting ++;
}

void vTaskExitCritical( void )
{
    if (ulCriticalNesting == 0) {
        while(1);
    }

    ulCriticalNesting --;
    if (ulCriticalNesting == 0)
    {
        portENABLE_INTERRUPTS();

        if (pendsvflag)
        {
            pendsvflag = 0;
            portYIELD();
        }
    }
}

#if configUSE_PREEMPTION == 0
void xPortSysTickHandler( void )
{
    portLONG ulDummy;

    ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
    xTaskIncrementTick();
    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}

#else
void xPortSysTickHandler( void )
{
    portLONG ulDummy;

    ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        if (xTaskIncrementTick() != pdFALSE)
        {
            portYIELD_FROM_ISR(pdTRUE);
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}
#endif

void vPortYieldHandler( void )
{
    uint32_t ulSavedInterruptMask;

    ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();

    vTaskSwitchContext();

    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulSavedInterruptMask );
}

void CORET_IRQHandler(void)
{
    vPortSetupTimerInterrupt();
    xPortSysTickHandler();
}
