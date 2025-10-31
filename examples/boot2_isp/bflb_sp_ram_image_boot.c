/**
  ******************************************************************************
  * @file    bflb_sp_ram_image_boot.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "bflb_eflash_loader_interface.h"
#include "bflb_sp_port.h"
#include "bflb_sp_bootinfo.h"
#include "bflb_sp_media_boot.h"
#include "bflb_sp_boot_decompress.h"
#include "bflb_sp_common.h"
#include "partition.h"

#include "bflb_flash.h"
#include "bflb_sec_sha.h"

#if BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX > 0

ram_img_config_t ram_img_config[BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX] = { 0 };
#define LZ4D_FLAG 0x184D2204
extern void unlz4(const void *aSource, void *aDestination, uint32_t FileLen);

/****************************************************************************/ /**
 * @brief  Boot2 cal image hash
 *
 * @param  src: image address on flash
 * @param  total_len: total len to copy
 * @param  hash:hash pointer
 *
 * @return 0 for success ,-1 for fail
 *
*******************************************************************************/
static int bflb_sp_cal_ram_img_hash(uint32_t src, uint32_t total_len, uint8_t *hash)
{
    //flash_read_via_xip(src, dest,total_len);
    uint32_t cur_len = 0;
    uint32_t read_len = 0;
    uint32_t cal_hash[8];

    bflb_sha_init(sha, SHA_MODE_SHA256);
    bflb_sha256_start(sha, &ctx_sha256);

    while (cur_len < total_len) {
        read_len = total_len - cur_len;
        if (read_len > BFLB_BOOT2_READBUF_SIZE) {
            read_len = BFLB_BOOT2_READBUF_SIZE;
        }
        bflb_flash_read(src + cur_len, g_boot2_read_buf, read_len);
        bflb_sha256_update(sha, &ctx_sha256, g_boot2_read_buf, read_len);
        cur_len += read_len;
    }
    bflb_sha256_finish(sha, &ctx_sha256, (uint8_t *)cal_hash);
    if (memcmp(hash, cal_hash, 32) != 0) {
        BOOT2_MSG_ERR("Cal hash error\r\n");
        return -1;
    }
    return 0;
}

/****************************************************************************/ /**
 * @brief  Boot2 copy image from flash to ram
 *
 * @param  src: image address on flash
 * @param  dest: ram address to copy to
 * @param  total_len: total len to copy
 *
 * @return 0 for success ,-1 for fail
 *
*******************************************************************************/
static int bflb_sp_do_ram_img_copy(uint32_t src, uint32_t dest, uint32_t total_len, uint8_t **hash)
{
    //flash_read_via_xip(src, dest,total_len);
    uint32_t cur_len = 0;
    uint32_t read_len = 0;
    uint32_t *p = (uint32_t *)g_boot2_read_buf;
    uint32_t xip_addr = 0;
    int ret = 0;

    g_boot2_read_buf[0] = 0x0;
    g_boot2_read_buf[1] = 0x0;
    bflb_flash_read(src, g_boot2_read_buf, 4);
    if (*p == LZ4D_FLAG) {
        if (*hash != NULL) {
            BOOT2_MSG_DBG("Cal hash\r\n");
            ret = bflb_sp_cal_ram_img_hash(src, total_len, *hash);
            if (ret != 0) {
                return BFLB_BOOT2_IMG_HASH_ERROR;
            }
        }
        xip_addr = hal_boot2_get_xip_addr(src);
        BOOT2_MSG_DBG("LZ4 image found, Decompress from %x to %x\r\n", xip_addr, dest);
        if (xip_addr != 0) {
            unlz4((const void *)xip_addr, (void *)dest, total_len);
            BOOT2_MSG_DBG("LZ4 done\r\n");
            bflb_l1c_dcache_clean_all();
            return 0;
        } else {
            BOOT2_MSG_ERR("Get XIP Addr fail\r\n");
            return -1;
        }
    }
    while (cur_len < total_len) {
        read_len = total_len - cur_len;
        if (read_len > BFLB_BOOT2_READBUF_SIZE) {
            read_len = BFLB_BOOT2_READBUF_SIZE;
        }
        bflb_flash_read(src + cur_len, g_boot2_read_buf, read_len);
        arch_memcpy((void *)(dest + cur_len), g_boot2_read_buf, read_len);
        cur_len += read_len;
    }

    bflb_l1c_dcache_clean_range((void *)dest, total_len);
    if (*hash != NULL) {
        BOOT2_MSG_DBG("Cal hash\r\n");
        ret = bflb_sp_cal_ram_img_hash(src, total_len, *hash);
        if (ret != 0) {
            return BFLB_BOOT2_IMG_HASH_ERROR;
        }
    }
    return 0;
}

/****************************************************************************/ /**
 * @brief  Boot2 do ram iamge boot,check partition to boot these images up
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return None
 *
*******************************************************************************/
int bflb_sp_do_ram_image_boot(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry)
{
    int ret;
    uint32_t img_len = 0;
    uint32_t img_offset = 0;
    char *img_name[BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX];
    uint32_t ram_img_cnt = 0;
    uint32_t i = 0;
    uint8_t active_index = 0;
    uint32_t hash[8];
    uint8_t *phash = (uint8_t *)hash;
    int try_again = 0;
    int32_t bflb_sp_boot2_rollback_ptentry(pt_table_id_type active_id,
                                        pt_table_stuff_config * pt_stuff, pt_table_entry_config * pt_entry);

    hal_boot2_get_ram_img_cnt(img_name, &ram_img_cnt);
    if (ram_img_cnt == 0) {
        return try_again;
    }
    for (i = 0; i < ram_img_cnt; i++) {
        ret = pt_table_get_active_entries_by_name(pt_stuff, (uint8_t *)img_name[i], pt_entry);
        if (PT_ERROR_SUCCESS == ret) {
            /* we use pt_entry->len as flag for ram load, this
               is very tricky but no rsvd word found */
            if ((pt_entry->len & 0xF0000000) == 0) {
                continue;
            }
            /* first, take it as invalid */
            ram_img_config[i].valid = 0;
            ram_img_config[i].boot_addr = pt_entry->len & 0xfffffff0;
            ram_img_config[i].core = pt_entry->len & 0x0f;
            active_index = pt_entry->active_index & 0x01;
            BOOT2_MSG_DBG("header addr:%x\r\n", pt_entry->start_address[active_index]);
            bflb_flash_read(pt_entry->start_address[active_index],
                            g_boot2_read_buf, BFLB_BOOT2_READBUF_SIZE);
            hal_boot2_get_img_info(g_boot2_read_buf, &img_offset, &img_len, &phash);
            if (img_len) {
                BOOT2_MSG_DBG("copy from %x to %x,len %d\r\n",
                      pt_entry->start_address[active_index] + img_offset,
                      ram_img_config[i].boot_addr,
                      img_len);
                ret = bflb_sp_do_ram_img_copy(pt_entry->start_address[active_index] + img_offset,
                                           ram_img_config[i].boot_addr,
                                           img_len, &phash);
                if (BFLB_BOOT2_IMG_HASH_ERROR == ret) {
                    ret = bflb_sp_boot2_rollback_ptentry(active_id, &pt_stuff[active_id], pt_entry);
                    if (ret == BFLB_BOOT2_SUCCESS) {
                        try_again = 1;
                        break;
                    } else {
                        BOOT2_MSG_DBG("Roll back entry fail\r\n");
                    }
                } else if (0 == ret) {
                    /* all success */
                    ram_img_config[i].valid = 1;
                }
            } else {
                BOOT2_MSG_DBG("Get ram image infor fail\r\n");
            }
        } else {
            BOOT2_MSG_DBG("%s not found\r\n", img_name[i]);
        }
    }
    return try_again;
}
#endif