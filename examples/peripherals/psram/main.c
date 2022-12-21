#include "bflb_core.h"
#include "board.h"

void test32(void)
{
    uint32_t i, val;

    printf("============= check uint32_t ==============\r\n");

    for (i = 0; i < 256; i += 4) {
        *((volatile uint32_t *)(BFLB_PSRAM_BASE + i)) = i / 4;
    }

    for (i = 0; i < 256; i += 4) {
        val = *((volatile uint32_t *)(BFLB_PSRAM_BASE + i));
        printf("addr = 0x%08X, val = 0x%08X\r\n", (BFLB_PSRAM_BASE + i), *((volatile uint32_t *)(BFLB_PSRAM_BASE + i)));

        if (i / 4 != val) {
            printf("psram check fail\r\n");
            while (1) {
            }
        }
    }
}

void test16(void)
{
    uint32_t i, val;

    printf("============= check uint16_t ==============\r\n");

    for (i = 0; i < 256; i += 2) {
        *((volatile uint16_t *)(BFLB_PSRAM_BASE + i)) = i / 2;
    }

    for (i = 0; i < 256; i += 2) {
        val = *((volatile uint16_t *)(BFLB_PSRAM_BASE + i));
        printf("addr = 0x%08X, val = 0x%08X\r\n", (BFLB_PSRAM_BASE + i), *((volatile uint16_t *)(BFLB_PSRAM_BASE + i)));

        if (i / 2 != val) {
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

    for (i = 0; i < 256; i++) {
        *((volatile uint8_t *)(BFLB_PSRAM_BASE + i)) = i;
    }

    for (i = 0; i < 256; i++) {
        val = *((volatile uint8_t *)(BFLB_PSRAM_BASE + i));
        printf("addr = 0x%08X, val = 0x%08X\r\n", (BFLB_PSRAM_BASE + i), *((volatile uint8_t *)(BFLB_PSRAM_BASE + i)));

        if ((uint8_t)i != val) {
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

    test8();
    test16();
    test32();

    printf(" test success\r\n");

    while (1) {
    }
}