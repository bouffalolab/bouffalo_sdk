#include <stdio.h>

#include CHIP_HDR
#include CHIP_GLB_HDR
#include CHIP_SYS_HDR
#include <bflb_sec_trng.h>

#include <bl_sys.h>

#include <openthread_port.h>

#include <openthread/platform/memory.h>
#include <openthread/platform/entropy.h>
#include <openthread/platform/misc.h>


void otPlatReset(otInstance *aInstance) 
{
    bl_sys_reset_system();
}

otPlatResetReason otPlatGetResetReason(otInstance *aInstance)
{
    BL_RST_REASON_E rstinfo = bl_sys_rstinfo_get();

    switch (rstinfo) {
        case BL_RST_HARDWARE_WATCHDOG:
        return OT_PLAT_RESET_REASON_WATCHDOG;
        case BL_RST_BOD:
        return OT_PLAT_RESET_REASON_OTHER;
        case BL_RST_HBN:
        return OT_PLAT_RESET_REASON_EXTERNAL;
        case BL_RST_POWER_OFF:
        return OT_PLAT_RESET_REASON_EXTERNAL;
        case BL_RST_SOFTWARE:
        return OT_PLAT_RESET_REASON_SOFTWARE;
        default:
        return OT_PLAT_RESET_REASON_UNKNOWN;
    }

    return OT_PLAT_RESET_REASON_UNKNOWN;
}

void otPlatAssertFail(const char *aFilename, int aLineNumber)
{
    printf("otPlatAssertFail, %s @ %d\r\n", aFilename, aLineNumber);
    configASSERT(false);
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

void *otPlatCAlloc(size_t aNum, size_t aSize)
{
    return calloc(aNum, aSize);
}

void otPlatFree(void *aPtr)
{
    free(aPtr);
}

otError otPlatEntropyGet(uint8_t *aOutput, uint16_t aOutputLength) 
{
    bflb_trng_readlen(aOutput, aOutputLength);
    return OT_ERROR_NONE;
}
