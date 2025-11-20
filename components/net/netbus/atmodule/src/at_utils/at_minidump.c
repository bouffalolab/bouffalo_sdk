#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <partition.h>
#include <bflb_flash.h>
#include "utils_hex.h"
#include "at_pal.h"

static uint32_t core_addr;
static size_t core_size;

#define BUF_SIZE 1024

int at_minidump()
{
    if (core_size == 0) {
        printf("[MINIDUMP] Error: core_size is 0\r\n");
        return -1;
    }
    
    uint8_t *buf = (uint8_t *)at_malloc(BUF_SIZE);
    uint8_t *hex_buf = (uint8_t *)at_malloc(BUF_SIZE * 2 + 1);
    int ret;
    int remain_size = core_size;

    if (!buf || !hex_buf) {
        printf("[MINIDUMP] Error: memory allocation failed\r\n");
        if (buf) at_free(buf);
        if (hex_buf) at_free(hex_buf);
        return -1;
    }

    at_write_data("\r\n", 2);
    for(int i = 0; remain_size > 0; i++) {
        int chunk = remain_size > BUF_SIZE ? BUF_SIZE : remain_size;
        ret = bflb_flash_read(core_addr + core_size - remain_size, buf, chunk);
        if(ret == 0) {
            remain_size -= chunk;
            utils_bin2hex(hex_buf, buf, chunk);
            hex_buf[chunk * 2] = '\0';
            at_write_data(hex_buf, chunk * 2);
            at_write_data("\r\n", 2);
        } else {
            printf("[MINIDUMP] Error: flash read failed at offset 0x%08x\r\n", core_addr + core_size - remain_size);
            at_free(buf);
            at_free(hex_buf);
            return -1;
        }
    }
    at_write_data("\r\n", 2);

    at_free(buf);
    at_free(hex_buf);

    return 0;
}

int at_minidump_init(uint32_t flash_addr, size_t flash_size)
{
    core_addr = flash_addr;
    core_size = flash_size;
    return 0;
}
