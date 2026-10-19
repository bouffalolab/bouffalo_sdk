#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_ef_ctrl.h"

#define EFUSE_DATA_LEN        4
#define EFUSE_DATA_START_ADDR 0x1c

int main(void)
{
    struct bflb_device_s *efuse_dev;
    uint32_t efuse_data_write[EFUSE_DATA_LEN] = { 0xAAAAAAAA, 0x55555555, 0x12345678, 0x87654321 };
    uint32_t efuse_data_read[EFUSE_DATA_LEN] = { 0 };
    int i = 0;

    board_init();

    efuse_dev = bflb_device_get_by_name("ef_ctrl");
    if (NULL == efuse_dev) {
        printf("efuse device driver not found!\r\n");
        while (1)
            ;
    }

    printf("\r\neFuse read write case!\r\n");
    printf("this case should be demo on bare efuse region !!!\r\n");

    /* just write data to efuse data reg, not program */
    printf("\r\nwrite but not program\r\n");
    bflb_ef_ctrl_write_direct(efuse_dev, EFUSE_DATA_START_ADDR, (uint32_t *)efuse_data_write, EFUSE_DATA_LEN, 0);

    printf("read back \r\n");
    bflb_ef_ctrl_read_direct(efuse_dev, EFUSE_DATA_START_ADDR, (uint32_t *)efuse_data_read, EFUSE_DATA_LEN, 0);
    for (i = 0; i < EFUSE_DATA_LEN; i++) {
        if (efuse_data_read[i] != efuse_data_write[i]) {
            printf("Efuse read back error\r\n");
            printf("read=%08x,expected=%08x\r\n", efuse_data_read[i], efuse_data_write[i]);
        }
    }

    /* we should get zeros since last write not programmed into eFuse(program=0) */
    printf("read back with reload\r\n");
    bflb_ef_ctrl_read_direct(efuse_dev, EFUSE_DATA_START_ADDR, (uint32_t *)efuse_data_read, EFUSE_DATA_LEN, 1);
    for (i = 0; i < EFUSE_DATA_LEN; i++) {
        if (efuse_data_read[i] != 0) {
            printf("Efuse read back with reload error\r\n");
            printf("read=%08x,expected=%08x\r\n", efuse_data_read[i], 0);
        }
    }

    /* write data to efuse data reg and program */
    printf("\r\nwrite and program\r\n");
    bflb_ef_ctrl_write_direct(efuse_dev, EFUSE_DATA_START_ADDR, (uint32_t *)efuse_data_write, EFUSE_DATA_LEN, 1);

    printf("read back \r\n");
    bflb_ef_ctrl_read_direct(efuse_dev, EFUSE_DATA_START_ADDR, (uint32_t *)efuse_data_read, EFUSE_DATA_LEN, 0);
    for (i = 0; i < EFUSE_DATA_LEN; i++) {
        if (efuse_data_read[i] != efuse_data_write[i]) {
            printf("Efuse read back error\r\n");
            printf("read=%08x,expected=%08x\r\n", efuse_data_read[i], efuse_data_write[i]);
        }
    }

    printf("read back with reload\r\n");
    bflb_ef_ctrl_read_direct(efuse_dev, EFUSE_DATA_START_ADDR, (uint32_t *)efuse_data_read, EFUSE_DATA_LEN, 1);
    for (i = 0; i < EFUSE_DATA_LEN; i++) {
        if (efuse_data_read[i] != efuse_data_write[i]) {
            printf("Efuse read back with reload error\r\n");
            printf("read=%08x,expected=%08x\r\n", efuse_data_read[i], efuse_data_write[i]);
        }
    }

    /* change the zero bits into 1 and program */
    printf("\r\nwrite and program the zero bits\r\n");
    for (i = 0; i < EFUSE_DATA_LEN; i++) {
        efuse_data_write[i] = ~efuse_data_write[i];
    }
    bflb_ef_ctrl_write_direct(efuse_dev, EFUSE_DATA_START_ADDR, (uint32_t *)efuse_data_write, EFUSE_DATA_LEN, 1);

    /* now,all the data should be 0xffffffff */
    printf("read back with reload\r\n");
    bflb_ef_ctrl_read_direct(efuse_dev, EFUSE_DATA_START_ADDR, (uint32_t *)efuse_data_read, EFUSE_DATA_LEN, 1);
    for (i = 0; i < EFUSE_DATA_LEN; i++) {
        if (efuse_data_read[i] != 0xffffffff) {
            printf("Efuse read back with reload error\r\n");
            printf("read=%08x,expected=%08x\r\n", efuse_data_read[i], 0xffffffff);
        }
    }

    printf("case finished!\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
