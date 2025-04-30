#include <stdint.h>
//#include "encoding.h"
#define read_csr(reg) ({ unsigned long __tmp;                                   \
            asm volatile ("csrr %0, " #reg : "=r"(__tmp));                      \
            __tmp; })
#include <FreeRTOS.h>
//#include <atomic.h>

/* Normal trap timepoint graph:
 *
 *  trapin           trapout
 *    |-------t1--------|
 *
 * Switch trap timepoint graph:
 *
 *  trapin  switch   trapout
 *    |--t2---|----t3---|
 *
 * t3 + t1 + t2 need excluded from ulRunTimeCounter of TCB
 */

/* one certain task t1 stats in ullRunningTrapCost during running. */
static uint64_t ullRunningTrapCost = 0ULL;

/* trap current runtime counter */
static uint64_t ullTrapAllCurrentTime = 0ULL;

/* extra cost between switch context */
uint32_t ulSwitchOutExtra = 0UL;
uint32_t ulSwitchInExtra = 0UL;

/* record last trap info */
uint32_t ulLastTrapCount = 0UL;
int32_t lLastTrapNum = 0L;

void (*pvReportIrqCost)(int irqnum, uint64_t cost) = NULL;
void (*pvReportExpCost)(int expnum, uint64_t cost) = NULL;

void vPortRegisterReportIrqCost(void (*pvFun)(int irqnum, uint64_t cost))
{
    pvReportIrqCost = pvFun;
}

void vPortRegisterReportExpCost(void (*pvFun)(int expnum, uint64_t cost))
{
    pvReportExpCost = pvFun;
}

/* called in trap, before ISR execute stat last trap cost */
void vCountLastTrapCost( void )
{
    /* first trap */
    if( lLastTrapNum == 0 ) {
        return;
    }

    /* last trap isn't switch context trap,
       or else reset ulSwitchOutExtra */
    if( ulSwitchOutExtra == 0 ) {
        ullRunningTrapCost += ulLastTrapCount;
    }
    else {
        ulSwitchOutExtra = 0;
    }

    if( lLastTrapNum > 0 ) {
        if(pvReportExpCost)
            pvReportExpCost( lLastTrapNum & 0x000003FF, ulLastTrapCount );
    } else {
        if(pvReportIrqCost)
            pvReportIrqCost( lLastTrapNum & 0x000003FF, ulLastTrapCount );
    }
}

void vPortUpdateSwitchOutExtra( uint64_t ullNow )
{
    ulSwitchOutExtra = ( uint32_t ) ullNow - ( uint32_t ) read_csr( mscratch );
}

uint64_t ullPortGetRunningTrapCostAndSwitchExtra( void )
{
    return ulSwitchInExtra + ullRunningTrapCost + ulSwitchOutExtra;
}

void vPortResetRunningTrapCost( void )
{
    ullTrapAllCurrentTime += ulSwitchInExtra + ullRunningTrapCost + ulSwitchOutExtra;
    ullRunningTrapCost = 0;
}

uint64_t ullPortGetRunningTrapCostFromTask( void )
{
    return ullRunningTrapCost + ulLastTrapCount;
}

uint32_t ulPortGetSwitchInExtra( void )
{
    return ulSwitchInExtra;
}

/* all trap runtime include prev tasks cost and current task cost. */
uint64_t ullPortGetTrapAllCurrentTime( void )
{
    return ullTrapAllCurrentTime;
}
