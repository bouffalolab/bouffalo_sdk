#include "bflb_mtimer.h"
#include "bflb_efuse.h"
#include "board.h"
#include "stdlib.h"
#include "string.h"
#include "memtester.h"

#if defined(BL616)
#include "bl616_common.h"
#include "bl616_glb.h"
// #include "bl616_sec_eng.h"
#include "bl616_tzc_sec.h"
#include "bl616_psram.h"
#endif

#if defined(BL616L)
#include "bl616l_common.h"
#include "bl616l_glb.h"
// #include "bl616_sec_eng.h"
#include "bl616l_tzc_sec.h"
#include "bl616l_psram.h"
#endif

#if defined(BL616D)
#include "bl616d_common.h"
#include "bl616d_glb.h"
// #include "bl606p_sec_eng.h"
#include "bl616d_psram.h"
#endif

#if defined(BL606P)
#include "bl606p_common.h"
#include "bl606p_glb.h"
// #include "bl606p_sec_eng.h"
#include "bl606p_psram.h"
#endif

#define UHS_PSRAM_ADDR       (0x50000000)
#define BL606P_X8_PSRAM_ADDR (0x54000000)
#define BL616_X8_PSRAM_ADDR  (0xA8000000)
#define BL616L_X8_PSRAM_ADDR (0x88000000)
#define BL616D_X8_PSRAM_ADDR (0x88000000)
#define MEMTESTER_M0
// #define MEMTESTER_D0

typedef struct _semc_test_config {
    uint32_t baseAddr;
    uint32_t testSize;
    uint32_t loopNum;
    uint32_t dramFreq;
    uint32_t enableCache;
} semc_test_config_t;

int fail_stop = 1;

// uint64_t l1c_hit = 0, l1c_miss = 0;
// uint32_t read_hit_h = 0, read_hit_l = 0, read_miss_h = 0, read_miss_l = 0;
// uint32_t write_hit_h = 0, write_hit_l = 0, write_miss_h = 0, write_miss_l = 0;

int main(void)
{
    bflb_efuse_device_info_type device_info;

    board_init();

    char memsuffix = 'B';
    /* --------------- stress test --------------- */
    semc_test_config_t testConfig;
#if defined(BL808)
    testConfig.baseAddr = UHS_PSRAM_ADDR;
    testConfig.dramFreq = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PSRAMA) / 1000000;
#elif defined(BL606P)
    testConfig.baseAddr = BL606P_X8_PSRAM_ADDR;
    testConfig.dramFreq = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PSRAMB) / 1000000;
#elif defined(BL616)
    testConfig.baseAddr = BL616_X8_PSRAM_ADDR;
    testConfig.dramFreq = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PSRAMB) / 1000000;
#elif defined(BL616L)
    testConfig.baseAddr = BL616L_X8_PSRAM_ADDR;
    testConfig.dramFreq = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PSRAMB) / 1000000;
#elif defined(BL616D)
    testConfig.baseAddr = BL616D_X8_PSRAM_ADDR;
    testConfig.dramFreq = Clock_Peripheral_Clock_Get(BL_PERIPHERAL_CLOCK_PSRAMB) / 1000000;
#endif
    testConfig.testSize = 0;
    testConfig.loopNum = 1000;
    testConfig.enableCache = 1;

    printf("mtimer clk:%d\r\n", CPU_Get_MTimer_Clock());
    printf("psram clk :%d\r\n", testConfig.dramFreq);
    printf("Now CPU size_t:%d\r\n", sizeof(size_t));

    bflb_efuse_get_device_info(&device_info);
    if (device_info.psram_info == 0) {
        printf("This chip has no psram, please disable CONFIG_PSRAM\r\n");
        while (1) {}
    }
#if defined(BL808)
    switch (device_info.psram_info) {
        case 2:
            testConfig.testSize = 32 * 1024 * 1024;
            break;
        case 3:
            testConfig.testSize = 64 * 1024 * 1024;
            break;
        default:
            printf("Psram info error\r\n");
            while (1) {}
            break;
    }
#else
    switch (device_info.psram_info) {
        case 1:
            testConfig.testSize = 4 * 1024 * 1024;
            break;
        case 2:
            testConfig.testSize = 8 * 1024 * 1024;
            break;
        case 3:
            testConfig.testSize = 16 * 1024 * 1024;
            break;
        default:
            printf("Psram info error\r\n");
            while (1) {}
            break;
    }
#endif

#if defined(MEMTESTER_M0)
    if (!testConfig.enableCache) {
        /* Disable D cache */
        csi_dcache_disable();
    }

    printf("\r\n########## Print out from target board ##########\r\n");
    printf("\r\n PSRAM r/w test settings:\r\n");
    printf("      Base Addr: 0x%x;\r\n", testConfig.baseAddr);
    printf("      Test Size: %d Bytes;\r\n", testConfig.testSize);
    printf("      Test Loop: %d;\r\n", testConfig.loopNum);
    printf(" PSRAM PLL Freq: %d MHz;\r\n", testConfig.dramFreq);
    printf("   Enable Cache: %d;\r\n\r\n", testConfig.enableCache);

    /* Run memory stress test: 64MByte, loop=1, page_size = 1kbyte */
    memtester_main(testConfig.baseAddr, testConfig.testSize, &memsuffix, testConfig.loopNum, (1 * 1024));
#endif

    while (1) {
#ifdef __riscv_muldiv
        int dummy;
        /* In lieu of a halt instruction, induce a long-latency stall. */
        __asm__ __volatile__("div %0, %0, zero"
                             : "=r"(dummy));
#endif
    }

    printf("memtester stress test done!\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
