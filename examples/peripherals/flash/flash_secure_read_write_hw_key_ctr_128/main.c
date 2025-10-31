#include "partition.h"
#include "bflb_flash.h"
#include "bflb_sec_aes.h"
#include "board.h"

uint8_t aes_ctr_128bit_iv[16] = {
    0x00, 0x6C, 0xB6, 0xDB, 0xC0, 0x54, 0x3B, 0x59,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define AES_DATA_LEN 48
#define PARTITION_NAME_TYPE_DATA    "DATA"

ATTR_NOCACHE_RAM_SECTION uint8_t aes_ctr_pt_buffer[AES_DATA_LEN] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f
};

ATTR_NOCACHE_RAM_SECTION uint8_t aes_ctr_ct_value[AES_DATA_LEN] = {
    0x3a, 0x98, 0xb6, 0x84, 0x85, 0x26, 0x18, 0x5c,
    0x46, 0x52, 0x7a, 0xd9, 0x34, 0xfa, 0xaf, 0xcf,
    0x82, 0x51, 0x07, 0xdc, 0x10, 0xde, 0xf5, 0x23,
    0xab, 0x4a, 0x78, 0x7f, 0x9e, 0x5b, 0x90, 0xe7,
    0x4c, 0xd2, 0xf7, 0x83, 0x12, 0x23, 0x89, 0xee,
    0x07, 0x7a, 0xc0, 0x1e, 0xd0, 0x49, 0x70, 0x1b
};

ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_ctr_ct_buffer[AES_DATA_LEN];
ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_ctr_tmp_buffer[AES_DATA_LEN];
ATTR_NOCACHE_NOINIT_RAM_SECTION uint8_t aes_ctr_tmp_buffer2[AES_DATA_LEN];

static void bflb_data_compare(const uint8_t *expected, uint8_t *input, uint32_t len)
{
    int i = 0;
    for (i = 0; i < len; i++) {
        if (input[i] != expected[i]) {
            printf("Compare fail at %d,input %02x, but expect %02x\r\n", i, input[i], expected[i]);
            while (1) {
            }
        }
    }
}

int main(void)
{
    board_init();
    uint32_t ret;
    uint32_t data_store_addr;
    struct bflb_device_s *aes;
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_entry_config pt_data_entry;
    pt_table_id_type active_id;

    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);
    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);
    ret = pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)PARTITION_NAME_TYPE_DATA, &pt_data_entry);

    if (PT_ERROR_SUCCESS != ret) {
        printf("PtTable get active entry fail!\r\n");
        while(1);
    } else {
        printf("data address: %08x \r\n", pt_data_entry.start_address[0]);
    }
    data_store_addr = pt_data_entry.start_address[0];

    aes = bflb_device_get_by_name(BFLB_NAME_SEC_AES);
    bflb_group0_request_aes_access(aes);
    bflb_aes_init(aes);
    bflb_aes_set_mode(aes, AES_MODE_CTR);
    bflb_aes_set_hwkey_source(aes, 0);
    bflb_aes_select_hwkey(aes,2,0);
    bflb_aes_setkey(aes, NULL, 128);

    bflb_aes_encrypt(aes, aes_ctr_pt_buffer, aes_ctr_128bit_iv, aes_ctr_ct_buffer, AES_DATA_LEN);
    bflb_data_compare(aes_ctr_ct_buffer, aes_ctr_ct_value, AES_DATA_LEN);
    printf("aes ctr128 encrypt success\r\n");

    bflb_flash_erase(data_store_addr, AES_DATA_LEN);
    memset(aes_ctr_tmp_buffer, 0, AES_DATA_LEN);

    bflb_flash_write(data_store_addr, aes_ctr_ct_buffer, AES_DATA_LEN);
    bflb_flash_read(data_store_addr, aes_ctr_tmp_buffer, AES_DATA_LEN);
    bflb_data_compare(aes_ctr_ct_buffer, aes_ctr_tmp_buffer, AES_DATA_LEN);
    printf("flash write & read success\r\n");

    bflb_aes_decrypt(aes, &aes_ctr_tmp_buffer[0], aes_ctr_128bit_iv, &aes_ctr_tmp_buffer2[0], AES_DATA_LEN);
    bflb_data_compare(aes_ctr_pt_buffer, aes_ctr_tmp_buffer2, AES_DATA_LEN);
    printf("aes ctr128 decrypt success\r\n");

    while (1) {
    }
}