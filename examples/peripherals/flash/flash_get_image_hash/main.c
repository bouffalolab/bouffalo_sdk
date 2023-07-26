#include "board.h"
#include "fw_header.h"
#include "bflb_sf_ctrl.h"
#include "bflb_flash.h"
#include "bflb_sec_sha.h"

#define XIP_READBUF_SIZE    4096
#define BFLB_FW_IMG_OFFSET  0x1000
extern void bflb_hexdump(const void *ptr, uint32_t buflen);

ATTR_NOCACHE_RAM_SECTION struct bflb_sha256_ctx_s  sha256_ctx ;
struct bflb_device_s *sha256_handle = NULL;
ATTR_NOCACHE_RAM_SECTION uint32_t read_buf[XIP_READBUF_SIZE/4] = {0};

int32_t bflb_spi_flash_read_xip(uint32_t addr, uint32_t *data, uint32_t len)
{
    arch_memcpy_fast(data,
                    (uint8_t *)(BFLB_FLASH_XIP_BASE + addr),
                    len);

    return 0;
}

static int32_t bflb_image_cal_hash(uint32_t startaddr, uint32_t total_len)
{
    int32_t deallen = 0;
    int32_t readlen = 0;
    uint32_t addr = startaddr;

    while (deallen < total_len) {
        readlen = total_len - deallen;

        if (readlen > XIP_READBUF_SIZE) {
            readlen = XIP_READBUF_SIZE;
        }

        bflb_spi_flash_read_xip(addr, read_buf, readlen);

        //bflb_hexdump((uint8_t *)read_buf,readlen);

        /* update hash */
        bflb_sha256_update(sha256_handle, &sha256_ctx, (uint8_t *)read_buf, readlen);

        addr += readlen;
        deallen += readlen;
    }

    return 0;
}

int main(void)
{
    uint32_t image_len = 0;
    uint32_t img_hash_cal[32 / 4];
    uint32_t img_hash_hdr[32 / 4];
    uint32_t offset=0;
    uint32_t i=0;

    board_init();

    printf("calculate image hash\r\n");

    offset=bflb_flash_get_image_offset()-BFLB_FW_IMG_OFFSET+BFLB_FW_LENGTH_OFFSET;
    /* read firmware length  */
    bflb_flash_read(offset, (uint8_t*)&image_len, 4);

    offset=bflb_flash_get_image_offset()-BFLB_FW_IMG_OFFSET+BFLB_FW_HASH_OFFSET;
    /* read firmware length  */
    bflb_flash_read(offset, (uint8_t*)&img_hash_hdr, 32);

    printf("image length in header=%d\r\n",image_len);

    printf("sha init\r\n");
    sha256_handle = bflb_device_get_by_name("sha");
    bflb_sha_init(sha256_handle, SHA_MODE_SHA256);
    bflb_sha256_start(sha256_handle, &sha256_ctx);

    printf("sha update\r\n");
    bflb_image_cal_hash(0, image_len);

    printf("finish\r\n");
    bflb_sha256_finish(sha256_handle, &sha256_ctx, (uint8_t *)img_hash_cal);

    for(i=0;i<sizeof(img_hash_cal)/sizeof(img_hash_cal[0]);i++){
        if(img_hash_cal[i]!=img_hash_hdr[i]){
            printf("hash cal error:\r\n");
            printf("calculated:\r\n");
            bflb_hexdump(img_hash_cal,32);
            printf("header:\r\n");
            bflb_hexdump(img_hash_hdr,32);
            while(1);
        }
    }

    printf("hash calculate success\r\n");
    while (1) {
    }
}
