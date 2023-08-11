#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <partition.h>
#include <utils_sha256.h>
#include <bflb_mtd.h>
#include <bflb_ota.h>
#include <bflb_flash.h>

typedef struct ota_parm_s 
{
    bflb_mtd_handle_t mtd_handle;
    uint32_t file_size;
    uint32_t sector_erased[((HOSAL_OTA_FILE_SIZE_MAX + 4095)/4096 + 31)/32];
}bflb_ota_parm_t;


static bflb_ota_parm_t *ota_parm = NULL; 

static int bflb_ota_erase(uint32_t offset, uint32_t len)
{
    uint32_t start_index, end_index, index;
    int ret;
    
    start_index = offset / 4096;
    end_index = (offset + len - 1) / 4096;
    
    for (index = start_index; index <= end_index; index++) {
        if ((ota_parm->sector_erased[index / 32] & (1U << (index % 32))) == 0) {
            ret = bflb_mtd_erase(ota_parm->mtd_handle, index * 4096, 4096);
            if (ret) {
                printf("mtd erase failed\r\n");
                return -1;
            }
            ota_parm->sector_erased[index / 32] |= (1U << (index % 32));
        }
    }
    
    return 0;
}

int bflb_ota_start(uint32_t file_size)
{
    int ret;
    uint32_t bin_size, ota_addr;
    pt_table_entry_config pt_fw_entry;
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;

    if (ota_parm != NULL) {
        printf("ota had start\r\n");
        return 0;
    }
    
    ota_parm = (bflb_ota_parm_t *)malloc(sizeof(bflb_ota_parm_t));
    if (NULL == ota_parm) {
        printf("have not enough memory\r\n");
        return -1;
    }
    memset(ota_parm, 0, sizeof(bflb_ota_parm_t));

    ota_parm->file_size = file_size;

    /* Set flash operation function, read via xip */
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

    if (PT_TABLE_ID_INVALID == active_id) {
        printf("No valid PT\r\n");
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }
    printf("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

    if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)BL_MTD_PARTITION_NAME_FW_DEFAULT, &pt_fw_entry))
    {
        printf("PtTable get active entry fail!\r\n");
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    ret = bflb_mtd_open(BL_MTD_PARTITION_NAME_FW_DEFAULT, &ota_parm->mtd_handle, BFLB_MTD_OPEN_FLAG_BACKUP);

    if (ret) {
        free(ota_parm);
        ota_parm = NULL;
        printf("Open Default FW partition failed\r\n");
        return -1;
    }

    ota_addr = pt_fw_entry.start_address[!pt_fw_entry.active_index];
    bin_size = pt_fw_entry.max_len[!pt_fw_entry.active_index];
    
    if (file_size > bin_size) {
        printf("file size is more than partition size\r\n");
        bflb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }
    printf("Starting OTA. OTA size is %lu\r\n", bin_size);
    printf("[OTA] activeIndex is %u, use OTA address=%08x\r\n", pt_fw_entry.active_index, (unsigned int)ota_addr);
    return ret;
}

int bflb_ota_update(uint32_t filesize, uint32_t offset, uint8_t *buf, uint32_t buf_len)
{
    uint32_t file_size, ret;
    if (ota_parm == NULL) {
        printf("please start ota first\r\n");
        return -1;
    }
    
    file_size = ota_parm->file_size;
    if ((filesize > file_size) || (NULL == buf) || ((offset + buf_len) > file_size) || (buf_len == 0)) {
        printf("parm is error!\r\n");
        return -1;
    }
    
    ret = bflb_ota_erase(offset, buf_len);
    if (ret) {
        printf("erase failed\r\n");
        return -1;
    }
    
    ret = bflb_mtd_write(ota_parm->mtd_handle, offset, buf_len, buf);
    if (ret) {
        printf("mtd write failed\r\n");
        return -1;
    }
    
    return ret;
}

int bflb_ota_finish(uint8_t check_hash)
{
    if (ota_parm == NULL) {
        printf("please start ota first\r\n");
        return -1;
    }
    uint32_t bin_size; 
    pt_table_entry_config pt_fw_entry;
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;
    int status = 0;
    if (ota_parm->file_size <= 32) {
        bflb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    bin_size = ota_parm->file_size - 32;

    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);
    
    if (PT_TABLE_ID_INVALID == active_id) {
        printf("No valid PT\r\n");
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)BL_MTD_PARTITION_NAME_FW_DEFAULT, &pt_fw_entry))
    {
        printf("PtTable_Get_Active_Entries fail\r\n");
        blfb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    printf("[OTA] prepare OTA partition info\r\n");

    if (check_hash) {
#define CHECK_IMG_BUF_SIZE   512
        uint8_t sha_check[32] = {0};
        uint8_t dst_sha[32] = {0};
        uint32_t read_size;
        sha256_context sha256_ctx;
        int i, offset = 0;
        uint8_t *r_buf;

        r_buf = malloc(CHECK_IMG_BUF_SIZE);
        if (r_buf == NULL) {
            blfb_mtd_close(ota_parm->mtd_handle);
            free(ota_parm);
            ota_parm = NULL;
            printf("malloc error\r\n");
            return -1;
        }

        utils_sha256_init(&sha256_ctx);
        utils_sha256_starts(&sha256_ctx);
    
        memset(sha_check, 0, 32);
        memset(dst_sha , 0, 32);
        offset = 0;
        while (offset < bin_size) {
            (bin_size - offset >= CHECK_IMG_BUF_SIZE) ? (read_size = CHECK_IMG_BUF_SIZE):(read_size = bin_size - offset);
            if (blfb_mtd_read(ota_parm->mtd_handle, offset, read_size, r_buf)) {
                printf("mtd read failed\r\n");
                bflb_mtd_close(ota_parm->mtd_handle);
                free(ota_parm);
                ota_parm = NULL;
                free(r_buf);
                return -1;
            }
            
            utils_sha256_update(&sha256_ctx, (const uint8_t *)r_buf, read_size);
            offset += read_size;
        }

        utils_sha256_finish(&sha256_ctx, sha_check);
        free(r_buf);

        blfb_mtd_read(ota_parm->mtd_handle, offset, 32, dst_sha);
        for (i = 0; i < 32; i++) {
            printf("%02X", dst_sha[i]);
        }
        puts("\r\nHeader SET SHA256 Checksum:");
        for (i = 0; i < 32; i++) {
            printf("%02X", sha_check[i]);
        }

        if (memcmp(sha_check, (const void *)dst_sha, 32) != 0) {
            printf("sha256 check error\r\n");
            bflb_mtd_close(ota_parm->mtd_handle);
            free(ota_parm);
            ota_parm = NULL;
            utils_sha256_free(&sha256_ctx);
            return -1;
        }

        utils_sha256_free(&sha256_ctx);
    } 

    pt_fw_entry.len = bin_size;
    printf("[OTA] Update PARTITION, partition len is %lu\r\n", pt_fw_entry.len);
    pt_fw_entry.active_index = !(pt_fw_entry.active_index & 0x01);
    pt_fw_entry.age++;
    status = pt_table_update_entry(!active_id, &pt_table_stuff[active_id], &pt_fw_entry);
    if (status != 0) {
        printf("pt table update fail! %d\r\n", status);
    }

    blfb_mtd_close(ota_parm->mtd_handle);
    free(ota_parm);
    ota_parm = NULL;

    return 0;
}

int bflb_ota_read(uint32_t offset, uint8_t *buf, uint32_t buf_len)
{
    if (ota_parm == NULL) {
        printf("please start ota first\r\n");
        return -1;
    }

    if ((NULL == buf) || ((offset + buf_len) > ota_parm->file_size) || (buf_len == 0)) {
        printf("parm is error!\r\n");
        return -1;
    }

    return blfb_mtd_read(ota_parm->mtd_handle, offset, buf_len, buf);
}

int bflb_ota_check(void)
{
    if (ota_parm == NULL) {
        printf("please start ota first\r\n");
        return -1;
    }
    uint32_t bin_size; 

    if (ota_parm->file_size <= 32) {
        blfb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    bin_size = ota_parm->file_size - 32;

#define CHECK_IMG_BUF_SIZE   512
    uint8_t sha_check[32] = {0};
    uint8_t dst_sha[32] = {0};
    uint32_t read_size;
    sha256_context sha256_ctx;
    int i, offset = 0;
    uint8_t *r_buf;

    r_buf = malloc(CHECK_IMG_BUF_SIZE);
    if (r_buf == NULL) {
        blfb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
        printf("malloc error\r\n");
        return -1;
    }

    utils_sha256_init(&sha256_ctx);
    utils_sha256_starts(&sha256_ctx);

    memset(sha_check, 0, 32);
    memset(dst_sha , 0, 32);
    offset = 0;
    while (offset < bin_size) {
        (bin_size - offset >= CHECK_IMG_BUF_SIZE) ? (read_size = CHECK_IMG_BUF_SIZE):(read_size = bin_size - offset);
        if (blfb_mtd_read(ota_parm->mtd_handle, offset, read_size, r_buf)) {
            printf("mtd read failed\r\n");
            blfb_mtd_close(ota_parm->mtd_handle);
            free(ota_parm);
            ota_parm = NULL;
            free(r_buf);
            return -1;
        }
        
        utils_sha256_update(&sha256_ctx, (const uint8_t *)r_buf, read_size);
        offset += read_size;
    }

    utils_sha256_finish(&sha256_ctx, sha_check);
    free(r_buf);

    blfb_mtd_read(ota_parm->mtd_handle, offset, 32, dst_sha);
    for (i = 0; i < 32; i++) {
        printf("%02X", dst_sha[i]);
    }
    puts("\r\nHeader SET SHA256 Checksum:");
    for (i = 0; i < 32; i++) {
        printf("%02X", sha_check[i]);
    }

    if (memcmp(sha_check, (const void *)dst_sha, 32) != 0) {
        printf("sha256 check error\r\n");
        blfb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
        utils_sha256_free(&sha256_ctx);
        return -1;
    }

    utils_sha256_free(&sha256_ctx);

    return 0;
}
int bflb_ota_apply(void)
{
    if (ota_parm == NULL) {
        printf("please start ota first\r\n");
        return -1;
    }
    uint32_t bin_size; 
    pt_table_entry_config pt_fw_entry;
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;
    int status = 0;
    
    if (ota_parm->file_size <= 32) {
        blfb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    bin_size = ota_parm->file_size - 32;

    active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);
    
    if (PT_TABLE_ID_INVALID == active_id) {
        printf("No valid PT\r\n");
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    if (pt_table_get_active_entries_by_name(&pt_table_stuff[active_id], (uint8_t *)BL_MTD_PARTITION_NAME_FW_DEFAULT, &pt_fw_entry))
    {
        printf("PtTable_Get_Active_Entries fail\r\n");
        blfb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
        return -1;
    }

    pt_fw_entry.len = bin_size;
    printf("[OTA] Update PARTITION, partition len is %lu\r\n", pt_fw_entry.len);
    pt_fw_entry.active_index = !(pt_fw_entry.active_index & 0x01);
    pt_fw_entry.age++;
    status = pt_table_update_entry(!active_id, &pt_table_stuff[active_id], &pt_fw_entry);
    if (status != 0) {
        printf("pt table update fail! %d\r\n", status);
    }

    blfb_mtd_close(ota_parm->mtd_handle);
    free(ota_parm);
    ota_parm = NULL;
    return 0;
}
void bflb_ota_abort(void)
{
    if (ota_parm != NULL) 
    {
        blfb_mtd_close(ota_parm->mtd_handle);
        free(ota_parm);
        ota_parm = NULL;
    }

}