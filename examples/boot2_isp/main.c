/**
  ******************************************************************************
  * @file    main.c
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
#include "xz_config.h"

#include "bflb_flash.h"
#include "log.h"
#include "bflb_sec_sha.h"

void wdt_init(void)
{
    // BOOT2_MSG_DBG("WDT 30s!\r\n");
    boot2_wdt_init();
    boot2_wdt_feed();
    boot2_wdt_set_value(30000);
    // BOOT2_MSG_DBG("WDT init Done!\r\n");
}

enum bflb_sp_boot2_pre_deal_ret_t {

    BFLB_BOOT2_PRE_DEAL_SUSS = 0x0,
    BFLB_BOOT2_PRE_DEAL_NEED_RETRY,
    BFLB_BOOT2_PRE_DEAL_ENTRY_NOT_FOUND,
};

uint8_t *g_boot2_read_buf;
boot2_image_config g_boot_img_cfg[BFLB_SP_BOOT2_CPU_GROUP_MAX];
boot2_efuse_hw_config g_efuse_cfg;
uint8_t g_ps_mode = BFLB_PSM_ACTIVE;
uint32_t g_user_hash_ignored = 0;
uint8_t g_usb_init_flag = 0;
struct bflb_device_s *sha;
ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_ctx_s ctx_sha256;
uint32_t g_anti_rollback_flag[3];
uint32_t g_anti_ef_en = 0, g_anti_ef_app_ver = 0;
uint32_t g_no_active_fw_age = 0;

int bflb_sp_do_ram_image_boot(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff,
                           pt_table_entry_config *pt_entry);
/****************************************************************************/ /**
 * @brief  Boot2 runs error call back function
 *
 * @param  log: Log to print
 *
 * @return None
 *
*******************************************************************************/
static void bflb_sp_boot2_on_error(void *log)
{
    while (1) {
#if BFLB_SP_BOOT2_SUPPORT_USB_IAP
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_USB);
        if (0 == g_usb_init_flag) {
            bflb_eflash_loader_if_init();
            g_usb_init_flag = 1;
        }

        if (0 == bflb_eflash_loader_if_handshake_poll(1)) {
            bflb_eflash_loader_main();
        }
#endif
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_UART);
        if (0 == bflb_eflash_loader_if_handshake_poll(0)) {
            bflb_eflash_loader_main();
        }
        BOOT2_MSG_ERR("%s\r\n", (char *)log);
        arch_delay_ms(500);
    }
}

/****************************************************************************/ /**
 * @brief  Boot2 Dump partition entry
 *
 * @param  ptEntry: Partition entry pointer to dump
 *
 * @return None
 *
*******************************************************************************/
void bflb_sp_dump_pt_entry(pt_table_entry_config *pt_entry)
{
    BOOT2_MSG("Name=%s\r\n", pt_entry->name);
    BOOT2_MSG("Age=%d\r\n", (unsigned int)(pt_entry->age & 0x00ffffff));
    BOOT2_MSG("Stable=%d\r\n", (unsigned int)(pt_entry->age >> 24));
    BOOT2_MSG("active Index=%d\r\n", (unsigned int)pt_entry->active_index);
    BOOT2_MSG("active start_address=%x\r\n", (unsigned int)pt_entry->start_address[pt_entry->active_index & 0x01]);
}

/****************************************************************************/ /**
 * @brief  Boot2 check XZ FW and do decompression
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return 1 for find XZ FW and decompress success, 0 for other cases
 *
*******************************************************************************/
#if BFLB_SP_BOOT2_SUPPORT_DECOMPRESS
static int bflb_sp_boot2_check_xz_fw(pt_table_id_type activeID, pt_table_stuff_config *ptStuff, pt_table_entry_config *ptEntry)
{
    uint8_t buf[6];

    if (BFLB_BOOT2_SUCCESS != bflb_sp_mediaboot_read(ptEntry->start_address[ptEntry->active_index], buf, sizeof(buf))) {
        BOOT2_MSG_ERR("Read fw fail\r\n");
        return 0;
    }

    if (bflb_sp_boot2_verify_xz_header(buf) == 1) {
        BOOT2_MSG_DBG("XZ image\r\n");

        if (BFLB_BOOT2_SUCCESS == bflb_sp_boot2_update_fw(activeID, ptStuff, ptEntry)) {
            return 1;
        } else {
            BOOT2_MSG_ERR("Img decompress fail\r\n");
            /* Set flag to make it not boot */
            // ptEntry->active_index = 0;
            // ptEntry->start_address[0] = 0;
            return 0;
        }
    }

    return 0;
}
#endif

/****************************************************************************/ /**
 * @brief  Boot2 copy firmware from OTA region to normal region
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
static int bflb_sp_boot2_do_fw_copy(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry)
{
    uint8_t active_index = pt_entry->active_index;
    uint32_t src_address = pt_entry->start_address[active_index & 0x01];
    uint32_t dest_address = pt_entry->start_address[!(active_index & 0x01)];
    uint32_t dest_max_size = pt_entry->max_len[!(active_index & 0x01)];
    uint32_t total_len = pt_entry->len;
    uint32_t deal_len = 0;
    uint32_t cur_len = 0;

    BOOT2_MSG_DBG("OTA copy src address %x, dest address %x, total len %d\r\n", src_address, dest_address, total_len);

    if (SUCCESS != bflb_flash_erase(dest_address, dest_max_size)) {
        return BFLB_BOOT2_FLASH_ERASE_ERROR;
    }

    while (deal_len < total_len) {
        cur_len = total_len - deal_len;

        if (cur_len > BFLB_BOOT2_READBUF_SIZE) {
            cur_len = BFLB_BOOT2_READBUF_SIZE;
        }

        if (BFLB_BOOT2_SUCCESS != bflb_sp_mediaboot_read(src_address, g_boot2_read_buf, cur_len)) {
            return BFLB_BOOT2_FLASH_READ_ERROR;
        }

        if (SUCCESS != bflb_flash_write(dest_address, g_boot2_read_buf, cur_len)) {
            return BFLB_BOOT2_FLASH_WRITE_ERROR;
        }

        src_address += cur_len;
        dest_address += cur_len;
        deal_len += cur_len;
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Boot2 deal with one firmware
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 * @param  fwName: Firmware name pointer
 * @param  type: Firmware name ID
 *
 * @return 1 for partition table changed,need re-parse,0 for partition table or entry parsed successfully
 *
*******************************************************************************/
static uint32_t bflb_sp_boot2_deal_one_fw(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff,
                                       pt_table_entry_config *pt_entry, uint8_t *fw_name,
                                       pt_table_entry_type type)
{
    uint32_t ret;

    if (fw_name != NULL) {
        ret = pt_table_get_active_entries_by_name(pt_stuff, fw_name, pt_entry);
        if (PT_ERROR_SUCCESS != ret) {
            ret = BFLB_BOOT2_PRE_DEAL_ENTRY_NOT_FOUND;
            BOOT2_MSG_WAR("entry %s not found\r\n", fw_name);
        } else {
            BOOT2_MSG_DBG("entry %s found\r\n", fw_name);
        }
    } else {
        ret = pt_table_get_active_entries_by_id(pt_stuff, type, pt_entry);
        if (PT_ERROR_SUCCESS != ret) {
            ret = BFLB_BOOT2_PRE_DEAL_ENTRY_NOT_FOUND;
            BOOT2_MSG_WAR("entry ID %d not found\r\n", type);
        } else {
            BOOT2_MSG_DBG("entry ID %d found\r\n", type);
        }
    }

    if (PT_ERROR_SUCCESS == ret) {
#if BFLB_SP_BOOT2_SUPPORT_DECOMPRESS
        if (bflb_sp_boot2_check_xz_fw(active_id, pt_stuff, pt_entry) == 1) {
            /* xz image found, retry to get newest partition info */
            return BFLB_BOOT2_PRE_DEAL_NEED_RETRY;
        }
#endif
        /* Check if this partition need copy */
        if (pt_entry->active_index >= 2) {
            BOOT2_MSG_DBG("Do image copy\r\n");
            if (BFLB_BOOT2_SUCCESS == bflb_sp_boot2_do_fw_copy(active_id, pt_stuff, pt_entry)) {
                pt_entry->active_index = !(pt_entry->active_index & 0x01);
                /* do not care age, hold on origin age */
                pt_entry->age++;
                ret = pt_table_update_entry((pt_table_id_type)(!active_id), pt_stuff, pt_entry);
                if (ret == PT_ERROR_SUCCESS) {
                    /* copy done, retry to get newest partition info */
                    BOOT2_MSG_DBG("Done\r\n");
                    return BFLB_BOOT2_PRE_DEAL_NEED_RETRY;
                } else {
                    // BOOT2_MSG_ERR("Update Partition table entry fail\r\n");
                }
            }
        }
    }

    return ret;
}

/****************************************************************************/ /**
 * @brief  Boot2 Roll back pt entry
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return boot_error_code
 *
*******************************************************************************/
#ifdef BFLB_SP_BOOT2_ROLLBACK
int32_t bflb_sp_boot2_rollback_ptentry(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry)
{
    int32_t ret;

    pt_entry->active_index = !(pt_entry->active_index & 0x01);
    pt_entry->age++;
    /* rollback to old partition must be hold origin age */
    pt_entry->age = ((pt_entry->age & 0x00FFFFFF) | (g_no_active_fw_age & 0xFF000000));

    ret = pt_table_update_entry((pt_table_id_type)(!active_id), pt_stuff, pt_entry);

    if (ret != PT_ERROR_SUCCESS) {
        BOOT2_MSG_ERR("Update PT entry fail\r\n");
        return BFLB_BOOT2_FAIL;
    }

    return BFLB_BOOT2_SUCCESS;
}
#endif

/****************************************************************************/ /**
 * @brief  customer board init pre
 *
 * @param  active_id: active id
 * @param  pt_table_stuff: pointer of pt_table_stuff array
 * @param  pt_entry: pointer of pt_entry array
 *
 * @return None
 *
*******************************************************************************/
__WEAK void ATTR_TCM_SECTION bflb_sp_boot2_customer_init_on_pt_found_pre(pt_table_id_type active_id, pt_table_stuff_config *pt_table_stuff, pt_table_entry_config *pt_entry)
{
    (void)active_id;
    (void)pt_table_stuff;
    (void)pt_entry;
}

/****************************************************************************/ /**
 * @brief  Boot2 get mfg start up request
 *
 * @param  activeID: Active partition table ID
 * @param  ptStuff: Pointer of partition table stuff
 * @param  ptEntry: Pointer of active entry
 *
 * @return 0 for partition table changed,need re-parse,1 for partition table or entry parsed successfully
 *
*******************************************************************************/
static void bflb_sp_boot2_get_mfg_startreq(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry, uint8_t *user_fw_name)
{
    uint32_t ret;
    uint32_t len = 0;
    uint8_t tmp[16 + 1] = { 0 };

    /* case 1 0mfg and 1 mfg, should return and boot */
    if ((arch_memcmp(user_fw_name, "0mfg", 4) == 0) || (arch_memcmp(user_fw_name, "1mfg", 4) == 0)) {
        /* user_fw_name is not empty for user */
        hal_boot2_clr_user_fw();
        arch_memset(user_fw_name, 0, 4);
        arch_memcpy(user_fw_name, "mfg", 3);
        return;
    }

    /* case 2 other not 0 should return and boot */
    if (user_fw_name[0] != 0) {
        return;
    }

    ret = pt_table_get_active_entries_by_name(pt_stuff, (uint8_t *)"mfg", pt_entry);

    if (PT_ERROR_SUCCESS == ret) {
        BOOT2_MSG_DBG("read mfg flag addr:%x\r\n", pt_entry->start_address[0] + MFG_START_REQUEST_OFFSET);
        bflb_flash_read(pt_entry->start_address[0] + MFG_START_REQUEST_OFFSET, tmp, sizeof(tmp) - 1);
        if (tmp[0] == '0' || tmp[0] == '1') {
            len = strlen((char *)tmp) - 1;
            if (len < 9) {
                arch_memcpy(user_fw_name, &tmp[1], len);
                BOOT2_MSG_DBG("flag:%s\r\n", tmp);
            }
        }
    } else {
        BOOT2_MSG_DBG("MFG not found\r\n");
    }
}

/****************************************************************************/ /**
 * @brief  Boot2 main function
 *
 * @param  None
 *
 * @return Return value
 *
*******************************************************************************/
int main(void)
{
    uint32_t ret = 0, i = 0;
    pt_table_stuff_config pt_table_stuff[2];
    pt_table_id_type active_id;
    /* Init to zero incase only one cpu boot up*/
    pt_table_entry_config pt_entry[BFLB_SP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint32_t boot_header_addr[BFLB_SP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint8_t boot_need_rollback[BFLB_SP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint8_t user_fw_name[9] = { 0 };
    uint32_t user_fw;
#ifdef BFLB_SP_BOOT2_ROLLBACK
    uint8_t roll_backed = 0;
#endif
    uint8_t mfg_mode_flag = 0;
    //boot_clk_config clk_cfg;
    uint8_t flash_cfg_buf[4 + sizeof(spi_flash_cfg_type) + 4] = { 0 };
    uint32_t crc;
    uint8_t *flash_cfg = NULL;
    uint32_t flash_cfg_len = 0;

    /* init watch dog */
    wdt_init();

    hal_boot2_init_clock();
    bflb_sp_boot2_start_timer();

    bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_UART);
    bflb_eflash_loader_if_init();

#if (BFLB_SP_BOOT2_MODE == BOOT2_MODE_RELEASE)
    bflb_uart_set_console(NULL);
#endif

#if (BFLB_SP_BOOT2_MODE == BOOT2_MODE_DEBUG)
    bflb_uart_set_console(uartx);
#endif

    hal_boot2_get_efuse_cfg(&g_efuse_cfg);
    simple_malloc_init(g_malloc_buf, sizeof(g_malloc_buf));
    g_boot2_read_buf = vmalloc(BFLB_BOOT2_READBUF_SIZE);
    ret = hal_boot2_custom(NULL);
    BOOT2_MSG_DBG("custom 0x%x\r\n", ret);

    ret = bflb_flash_init();
    if (ret) {
        bflb_uart_set_console(uartx);
        while (1) {
            BOOT2_MSG_ERR("flash cfg err\r\n");
            bflb_mtimer_delay_ms(500);
            bflb_sp_boot2_on_error("flash cfg err\r\n");
        }
    }
    BOOT2_MSG_DBG("flash init %d\r\n", (int)ret);

    BOOT2_MSG_DBG("Boot2 start:%s,%s\r\n", __DATE__, __TIME__);
    BOOT2_MSG_DBG("Group=%d,CPU Count=%d\r\n", BFLB_SP_BOOT2_CPU_GROUP_MAX, BFLB_SP_BOOT2_CPU_MAX);

    BOOT2_MSG_DBG("ver:%s\r\n", BFLB_BOOT2_VER);

    /* Reset Sec_Eng */
    hal_boot2_reset_sec_eng();
    sha = bflb_device_get_by_name(BFLB_NAME_SEC_SHA);

    bflb_group0_request_sha_access(sha);
    /* Get power save mode */
    g_ps_mode = bflb_sp_read_power_save_mode();

    /* Get User specified FW */
    user_fw = hal_boot2_get_user_fw();
    arch_memcpy(user_fw_name, &user_fw, 4);
    BOOT2_MSG_DBG("user_fw %s\r\n", user_fw_name);

    /* Set flash operation function, read via xip */
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

#if BFLB_SP_BOOT2_SUPPORT_USB_IAP
    if (memcmp(user_fw_name, (char *)"USB", 3) == 0) {
        hal_boot2_clr_user_fw();
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_USB);
        bflb_eflash_loader_if_init();
        g_usb_init_flag = 1;
        if (0 == bflb_eflash_loader_if_handshake_poll(user_fw_name[3])) {
            bflb_eflash_loader_main();
        }
    }
#endif

#if BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX > 0
    /* Deal with ram image: copy from flash to ram like */
    do {
        active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

        if (PT_TABLE_ID_INVALID == active_id) {
            // bflb_sp_boot2_on_error("No valid PT\r\n");
            break;
        }
        ret = bflb_sp_do_ram_image_boot(active_id, &pt_table_stuff[active_id], &pt_entry[0]);
    } while (ret != 0);
#endif
    /* Deal with flash image */
    while (1) {
#ifdef BFLB_SP_BOOT2_ROLLBACK
        uint32_t count_val = 0;
        bflb_sp_get_anti_rollback_flag(&count_val);
        if (((count_val >> 16) & 0xff) > 0x2) {
            break;
        }
#endif
        do {
            // mfg_mode_flag = 0;
            active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

            if (PT_TABLE_ID_INVALID == active_id) {
                // bflb_sp_boot2_on_error("No valid PT\r\n");
                BOOT2_MSG_WAR("NO valid PT table,use default\r\n");
                /* no valid partition, boot from 0x10000 */
                active_id = 0;
                pt_table_init_default_fw(&pt_table_stuff[0], (BFLB_PT_TABLE1_ADDRESS + 0x1000));

            }
            /* check partition is valid then save age info else age flag be stable */
            if (pt_table_valid(&pt_table_stuff[!active_id]) == 1) {
                pt_table_entry_config pt_entry_tmp[1] = { 0 };
                /* get not active partition fw age info */
                pt_table_get_active_entries_by_id(&pt_table_stuff[!active_id], 0, &pt_entry_tmp[0]);
                g_no_active_fw_age = pt_entry_tmp[0].age;
            }

            BOOT2_MSG_DBG("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

            bflb_sp_boot2_customer_init_on_pt_found_pre(active_id, &pt_table_stuff[0], &pt_entry[0]);

            if (mfg_mode_flag == 0) {
                /* for mfg,we only try to start up once,otherwise,it will get deadloop */
                bflb_sp_boot2_get_mfg_startreq(active_id, &pt_table_stuff[active_id], &pt_entry[0], user_fw_name);
            }

            /* Get entry and boot */
            if (user_fw_name[0] != 0) {
                ret = bflb_sp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], user_fw_name, PT_ENTRY_FW_CPU0);
                if (!ret) {
                    /* boot user fw success */
                    g_user_hash_ignored = 1;
                    mfg_mode_flag = 1;

                } else {
                    ret = bflb_sp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], NULL, PT_ENTRY_FW_CPU0);
                    if (!ret) {
                        /* go on get partition type 1 (group1) info */
                        if (BFLB_SP_BOOT2_CPU_GROUP_MAX > 1) {
                            ret = bflb_sp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[1], NULL, PT_ENTRY_FW_CPU1);
                        }
                    }
                }
                arch_memset(user_fw_name, 0, 4);
            } else {
                /* get partition type 0 (group0) info */
                ret = bflb_sp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], NULL, PT_ENTRY_FW_CPU0);
                if (!ret) {
                    /* go on get partition type 1 (group1) info */
                    if (BFLB_SP_BOOT2_CPU_GROUP_MAX > 1) {
                        ret = bflb_sp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[1], NULL, PT_ENTRY_FW_CPU1);
                    }
                }
            }
            if (ret == BFLB_BOOT2_PRE_DEAL_ENTRY_NOT_FOUND) {
                /* partition valid but entry not found, boot from 0x10000 as dafault */
                active_id = 0;
                pt_table_init_default_fw(&pt_table_stuff[0], 0x10000);
                //bflb_sp_boot2_on_error("No valid entry\r\n");
            }
        } while (ret);

        /* Pass data to App*/
        bflb_sp_boot2_pass_parameter(NULL, 0);
        /* Pass active partition table ID */
        bflb_sp_boot2_pass_parameter(&active_id, 4);
        /* Pass active partition table content: table header+ entries +crc32 */
        bflb_sp_boot2_pass_parameter(&pt_table_stuff[active_id], sizeof(pt_table_config) + 4 +
                                                                  pt_table_stuff[active_id].pt_table.entryCnt * sizeof(pt_table_entry_config));

        /* Pass flash config */
        if (pt_entry[0].start_address[pt_entry[0].active_index] != 0) {
            /* Include magic and CRC32 */
            bflb_flash_get_cfg(&flash_cfg, &flash_cfg_len);
            arch_memcpy(flash_cfg_buf, "FCFG", 4);
            arch_memcpy(flash_cfg_buf + 4, flash_cfg, flash_cfg_len);
            crc = BFLB_Soft_CRC32(flash_cfg, flash_cfg_len);
            arch_memcpy(flash_cfg_buf + 4 + flash_cfg_len, &crc, sizeof(crc));
            bflb_sp_boot2_pass_parameter(flash_cfg_buf, sizeof(flash_cfg_buf));
        }

        BOOT2_MSG_DBG("Boot start\r\n");

#ifdef BFLB_SP_BOOT2_ROLLBACK
        for (i = 0; i < BFLB_SP_BOOT2_CPU_GROUP_MAX; i++) {
            boot_header_addr[i] = pt_entry[i].start_address[pt_entry[i].active_index];
            uint8_t retry_flag = (pt_entry[i].age >> 24);
            if ((retry_flag != 0x00) && (roll_backed == 0)) {
                if (retry_flag > 0xF0) {
                    retry_flag--;
                    pt_entry[i].age = ((pt_entry[i].age & 0x00FFFFFF) | (retry_flag << 24));
                    ret = pt_table_update_entry(active_id, &pt_table_stuff[active_id], &pt_entry[i]);
                } else {
                    BOOT2_MSG_WAR("Retry boot fail, Rollback %d\r\n", i);
                    boot_need_rollback[i] = 1;
                    goto ROLLBACK_CHECK;
                }
            }
        }
        /* mfg mode do not need roll back */
        if (roll_backed == 0 && mfg_mode_flag == 0) {
            ret = bflb_sp_mediaboot_main(boot_header_addr, boot_need_rollback, 1);
        } else {
            ret = bflb_sp_mediaboot_main(boot_header_addr, boot_need_rollback, 0);
        }
#else
        ret = bflb_sp_mediaboot_main(boot_header_addr, boot_need_rollback, 0);
#endif

        if (mfg_mode_flag == 1) {
            continue;
        }

#ifdef BFLB_SP_BOOT2_ROLLBACK
        /* If rollback is done, we still fail, break */
        if (roll_backed) {
            break;
        }

        BOOT2_MSG_WAR("Boot return 0x%x\r\nCheck Rollback\r\n", ret);

    ROLLBACK_CHECK:
        for (i = 0; i < BFLB_SP_BOOT2_CPU_GROUP_MAX; i++) {
            if (boot_need_rollback[i] != 0) {
                BOOT2_MSG_WAR("Rollback group %d\r\n", i);

                if (BFLB_BOOT2_SUCCESS == bflb_sp_boot2_rollback_ptentry(active_id, &pt_table_stuff[active_id], &pt_entry[i])) {
                    roll_backed = 1;
                }
            }
        }
        /* If need no rollback, boot fail due to other reseaon instead of imgae issue,break */
        if (roll_backed == 0) {
            break;
        }
#else
        break;
#endif
    }

    /* We should never get here unless boot fail */
    BOOT2_MSG_ERR("Media boot return %d\r\n", ret);
    BOOT2_MSG_ERR("Boot2 fail\r\n");
    while (1) {
#if BFLB_SP_BOOT2_SUPPORT_USB_IAP
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_USB);

        bflb_eflash_loader_if_init();
        if (0 == bflb_eflash_loader_if_handshake_poll(5000)) {
            bflb_eflash_loader_main();
        }
        /* read data finished,deinit and go on*/
        bflb_eflash_loader_if_deinit();

#endif
        bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_UART);

        if (0 == bflb_eflash_loader_if_handshake_poll(0)) {
            bflb_eflash_loader_main();
        }

        bflb_uart_set_console(uartx);
        bflb_boot2_release_dump_log();

        bflb_mtimer_delay_ms(500);
    }
}

void bflb_main()
{
    main();
}
