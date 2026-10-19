#include "bl_lp_internal.h"

#ifdef BL_WIFI_LP_FW
uint64_t (*shared_CPU_Get_MTimer_Counter)(void) = NULL;
void (*shared_arch_delay_ms)(uint32_t) = NULL;
void (*shared_arch_delay_us)(uint32_t) = NULL;
int32_t (*shared_CPU_Reset_MTimer)(void) = NULL;
int32_t (*shared_lpfw_calculate_beacon_delay)(uint32_t *, uint64_t, uint64_t, uint32_t) = NULL;
int32_t (*shared_lpfw_beacon_delay_sliding_win_get_average)(uint32_t *) = NULL;
int32_t (*shared_AON_Set_LDO11_SOC_Sstart_Delay)(uint32_t) = NULL;
int32_t (*shared_PDS_Default_Level_Config)(uint32_t *, uint32_t) = NULL;
#endif

uint32_t *shared_func_array[32];

static int8_t set_shared_func(uint8_t index, uint32_t *func)
{
    if ((((uint32_t)func >> 28) & 0xF) == 0xA) {
        return -1;
    }

    shared_func_array[index] = func;
    return 0;
}

void shared_func_init(void)
{
    int8_t flag = 0;

    flag |= set_shared_func(0, (uint32_t *)CPU_Get_MTimer_Counter);
    flag |= set_shared_func(1, (uint32_t *)arch_delay_ms);
    flag |= set_shared_func(2, (uint32_t *)arch_delay_us);
    flag |= set_shared_func(3, (uint32_t *)CPU_Reset_MTimer);
    flag |= set_shared_func(4, (uint32_t *)lpfw_calculate_beacon_delay);
    flag |= set_shared_func(5, (uint32_t *)lpfw_beacon_delay_sliding_win_get_average);
    flag |= set_shared_func(6, (uint32_t *)AON_Set_LDO11_SOC_Sstart_Delay);
    flag |= set_shared_func(7, (uint32_t *)PDS_Default_Level_Config);

    if (flag != 0) {
        BL_LP_LOG("shared_func_init err!\r\n");
    }
}
