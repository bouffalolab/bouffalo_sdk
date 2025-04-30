#include <stdio.h>
#include <string.h>
#include <bl_sys.h>
#include <bl_sec.h>

#include <openthread_port.h>
#include <openthread/platform/memory.h>
#include <openthread/platform/misc.h>
#include <openthread/platform/entropy.h>

void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    return calloc(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
    free(aPtr);
}

void otPlatReset(otInstance *aInstance) 
{
#if CFG_USE_PSRAM

extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
extern uint8_t _heap2_size; // @suppress("Type cannot be resolved")
extern uint8_t _heap3_size; // @suppress("Type cannot be resolved")
extern unsigned int xPortGetMinimumEverFreeHeapSize( void );
extern unsigned int xPortGetMinimumEverFreeHeapSizePsram( void );

    printf ("otPlatReset sram = %u/%u, psram = %u/%u\r\n", 
        (unsigned int) &_heap_size + (unsigned int) &_heap2_size - xPortGetMinimumEverFreeHeapSize(), 
        (unsigned int) &_heap_size + (unsigned int) &_heap2_size,
        (unsigned int) &_heap3_size - xPortGetMinimumEverFreeHeapSizePsram(),
        (unsigned int) &_heap3_size);

    vTaskDelay(500);
#else
extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
extern uint8_t _heap2_size; // @suppress("Type cannot be resolved")
extern unsigned int xPortGetMinimumEverFreeHeapSize( void );

    printf ("otPlatReset sram = %u/%u\r\n", 
        (unsigned int) &_heap_size + (unsigned int) &_heap2_size - xPortGetMinimumEverFreeHeapSize(), 
        (unsigned int) &_heap_size + (unsigned int) &_heap2_size);
    
    vTaskDelay(500);
#endif
    bl_sys_reset_por();
}

otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{

    BL_RST_REASON_E rstinfo = bl_sys_rstinfo_get();

    switch (rstinfo) {
        case BL_RST_WDT:
        return OT_PLAT_RESET_REASON_WATCHDOG;
        case BL_RST_BOR:
        return OT_PLAT_RESET_REASON_OTHER;
        case BL_RST_HBN:
        return OT_PLAT_RESET_REASON_EXTERNAL;
        case BL_RST_POR:
        return OT_PLAT_RESET_REASON_EXTERNAL;
        case BL_RST_SOFTWARE:
        return OT_PLAT_RESET_REASON_SOFTWARE;
        default:
        return OT_PLAT_RESET_REASON_UNKNOWN;
    }
}

void otPlatAssertFail(const char *aFilename, int aLineNumber)
{
    printf("otPlatAssertFail, %s @ %d\r\n", aFilename, aLineNumber);
}

void otPlatWakeHost(void)
{}

otError otPlatSetMcuPowerState(otInstance *aInstance, otPlatMcuPowerState aState)
{
    return OT_ERROR_NONE;
}
otPlatMcuPowerState otPlatGetMcuPowerState(otInstance *aInstance)
{
    return OT_PLAT_MCU_POWER_STATE_ON;
}

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength) 
{
    bl_rand_stream(aOutput, aOutputLength);
    return OT_ERROR_NONE;
}