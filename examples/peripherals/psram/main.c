#include "bflb_core.h"
#include "bflb_l1c.h"
#include "board.h"

#define BFLB_PSRAM_TEST_SIZE (16 * 1024 * 1024)
#define BFLB_PSRAM_TEST_ADDR (BFLB_PSRAM_BASE + (0 * 1024 * 1024))

void test32(void)
{
    uint32_t i, val;

    printf("============= check uint32_t ==============\r\n");

    for (i = 0; i < BFLB_PSRAM_TEST_SIZE; i += 4) {
        *((volatile uint32_t *)(BFLB_PSRAM_TEST_ADDR + i)) = i / 4;
    }

    for (i = 0; i < BFLB_PSRAM_TEST_SIZE; i += 4) {
        val = *((volatile uint32_t *)(BFLB_PSRAM_TEST_ADDR + i));
        if (i / 4 != val) {
            printf("addr = 0x%08X, val = 0x%08X, expect = 0x%08X\r\n", (BFLB_PSRAM_TEST_ADDR + i), val, i / 4);
            printf("psram check fail\r\n");
            while (1)
                ;
        }
    }
}

void test16(void)
{
    uint32_t i, val;

    printf("============= check uint16_t ==============\r\n");

    for (i = 0; i < BFLB_PSRAM_TEST_SIZE; i += 2) {
        *((volatile uint16_t *)(BFLB_PSRAM_TEST_ADDR + i)) = i / 2;
    }
    for (i = 0; i < BFLB_PSRAM_TEST_SIZE; i += 2) {
        val = *((volatile uint16_t *)(BFLB_PSRAM_TEST_ADDR + i));

        if (((i / 2) & 0xffff) != val) {
            printf("addr = 0x%08X, val = 0x%08X, expect = 0x%08X\r\n", (BFLB_PSRAM_TEST_ADDR + i), val, (i / 2) & 0xffff);
            printf("psram check fail\r\n");
            while (1) {
            }
        }
    }
}

void test8(void)
{
    uint32_t i;
    uint8_t val;

    printf("============= check uint8_t ==============\r\n");

    for (i = 0; i < BFLB_PSRAM_TEST_SIZE; i++) {
        *((volatile uint8_t *)(BFLB_PSRAM_TEST_ADDR + i)) = i;
    }

    for (i = 0; i < BFLB_PSRAM_TEST_SIZE; i++) {
        val = *((volatile uint8_t *)(BFLB_PSRAM_TEST_ADDR + i));

        if ((i & 0xff) != val) {
            printf("addr = 0x%08X, val = 0x%08X, expect = 0x%08X\r\n", (BFLB_PSRAM_TEST_ADDR + i), val, i & 0xff);
            printf("psram check fail\r\n");
            while (1) {
            }
        }
    }
}

int main(void)
{
    board_init();

    printf(" psram read write test \r\n");

    bflb_l1c_dcache_clean_all();

    bflb_l1c_dcache_disable();

    test8();
    test16();
    test32();

    printf(" test success\r\n");

    while (1) {
    }
}