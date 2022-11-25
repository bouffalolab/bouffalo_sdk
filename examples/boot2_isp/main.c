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
#include "blsp_port.h"
#include "blsp_bootinfo.h"
#include "blsp_media_boot.h"
#include "blsp_boot_decompress.h"
#include "blsp_common.h"
#include "blsp_version.h"
#include "partition.h"
#include "softcrc.h"
#include "bflb_flash.h"
#include "log.h"
#include "bflb_sec_sha.h"

enum blsp_boot2_pre_deal_ret_t {

    BFLB_BOOT2_PRE_DEAL_SUSS = 0x0,
    BFLB_BOOT2_PRE_DEAL_NEED_RETRY,
    BFLB_BOOT2_PRE_DEAL_ENTRY_NOT_FOUND,
};

uint8_t *g_boot2_read_buf;
boot2_image_config g_boot_img_cfg[BLSP_BOOT2_CPU_GROUP_MAX];
boot2_efuse_hw_config g_efuse_cfg;
uint8_t g_ps_mode = BFLB_PSM_ACTIVE;
uint32_t g_user_hash_ignored = 0;
uint8_t g_usb_init_flag = 0;
struct bflb_device_s *sha;
ATTR_NOCACHE_NOINIT_RAM_SECTION struct bflb_sha256_ctx_s ctx_sha256;
int blsp_do_ram_image_boot(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff,
                           pt_table_entry_config *pt_entry);
/****************************************************************************/ /**
 * @brief  Boot2 runs error call back function
 *
 * @param  log: Log to print
 *
 * @return None
 *
*******************************************************************************/
static void blsp_boot2_on_error(void *log)
{
    while (1) {
#if BLSP_BOOT2_SUPPORT_USB_IAP
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
        LOG_E("%s\r\n", (char *)log);
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
void blsp_dump_pt_entry(pt_table_entry_config *pt_entry)
{
    LOG_F("Name=%s\r\n", pt_entry->name);
    LOG_F("Age=%d\r\n", (unsigned int)pt_entry->age);
    LOG_F("active Index=%d\r\n", (unsigned int)pt_entry->active_index);
    LOG_F("active start_address=%08x\r\n", (unsigned int)pt_entry->start_address[pt_entry->active_index & 0x01]);
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
#if BLSP_BOOT2_SUPPORT_DECOMPRESS
static int blsp_boot2_check_xz_fw(pt_table_id_type activeID, pt_table_stuff_config *ptStuff, pt_table_entry_config *ptEntry)
{
    uint8_t buf[6];

    if (BFLB_BOOT2_SUCCESS != blsp_mediaboot_read(ptEntry->start_address[ptEntry->active_index], buf, sizeof(buf))) {
        LOG_E("Read fw fail\r\n");
        return 0;
    }

    if (blsp_boot2_verify_xz_header(buf) == 1) {
        LOG_F("XZ image\r\n");

        if (BFLB_BOOT2_SUCCESS == blsp_boot2_update_fw(activeID, ptStuff, ptEntry)) {
            return 1;
        } else {
            LOG_E("Img decompress fail\r\n");
            /* Set flag to make it not boot */
            ptEntry->active_index = 0;
            ptEntry->start_address[0] = 0;
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
static int blsp_boot2_do_fw_copy(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry)
{
    uint8_t active_index = pt_entry->active_index;
    uint32_t src_address = pt_entry->start_address[active_index & 0x01];
    uint32_t dest_address = pt_entry->start_address[!(active_index & 0x01)];
    uint32_t dest_max_size = pt_entry->max_len[!(active_index & 0x01)];
    uint32_t total_len = pt_entry->len;
    uint32_t deal_len = 0;
    uint32_t cur_len = 0;

    LOG_F("OTA copy src address %08x, dest address %08x, total len %d\r\n", src_address, dest_address, total_len);

    if (SUCCESS != bflb_flash_erase(dest_address, dest_max_size)) {
        LOG_E("Erase flash fail");
        return BFLB_BOOT2_FLASH_ERASE_ERROR;
    }

    while (deal_len < total_len) {
        cur_len = total_len - deal_len;

        if (cur_len > BFLB_BOOT2_READBUF_SIZE) {
            cur_len = BFLB_BOOT2_READBUF_SIZE;
        }

        if (BFLB_BOOT2_SUCCESS != blsp_mediaboot_read(src_address, g_boot2_read_buf, cur_len)) {
            LOG_E("Read FW fail when copy\r\n");
            return BFLB_BOOT2_FLASH_READ_ERROR;
        }

        if (SUCCESS != bflb_flash_write(dest_address, g_boot2_read_buf, cur_len)) {
            LOG_E("Write flash fail");
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
static uint32_t blsp_boot2_deal_one_fw(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff,
                                       pt_table_entry_config *pt_entry, uint8_t *fw_name,
                                       pt_table_entry_type type)
{
    uint32_t ret;

    if (fw_name != NULL) {
        ret = pt_table_get_active_entries_by_name(pt_stuff, fw_name, pt_entry);
        if (PT_ERROR_SUCCESS != ret) {
            ret = BFLB_BOOT2_PRE_DEAL_ENTRY_NOT_FOUND;
            LOG_F("entry %s not found\r\n", fw_name);
        } else {
            LOG_F("entry %s found\r\n", fw_name);
        }
    } else {
        ret = pt_table_get_active_entries_by_id(pt_stuff, type, pt_entry);
        if (PT_ERROR_SUCCESS != ret) {
            ret = BFLB_BOOT2_PRE_DEAL_ENTRY_NOT_FOUND;
            LOG_F("entry ID %d not found\r\n", type);
        } else {
            LOG_F("entry ID %d found\r\n", type);
        }
    }

    if (PT_ERROR_SUCCESS == ret) {
#if BLSP_BOOT2_SUPPORT_DECOMPRESS
        if (blsp_boot2_check_xz_fw(active_id, pt_stuff, pt_entry) == 1) {
            /* xz image found, retry to get newest partition info */
            return BFLB_BOOT2_PRE_DEAL_NEED_RETRY;
        }
#endif
        /* Check if this partition need copy */
        if (pt_entry->active_index >= 2) {
            LOG_F("Do image copy\r\n");
            if (BFLB_BOOT2_SUCCESS == blsp_boot2_do_fw_copy(active_id, pt_stuff, pt_entry)) {
                pt_entry->active_index = !(pt_entry->active_index & 0x01);
                pt_entry->age++;
                ret = pt_table_update_entry((pt_table_id_type)(!active_id), pt_stuff, pt_entry);
                if (ret == PT_ERROR_SUCCESS) {
                    /* copy done, retry to get newest partition info */
                    LOG_F("Done\r\n");
                    return BFLB_BOOT2_PRE_DEAL_NEED_RETRY;
                } else {
                    LOG_E("Update Partition table entry fail\r\n");
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
#ifdef BLSP_BOOT2_ROLLBACK
int32_t blsp_boot2_rollback_ptentry(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry)
{
    int32_t ret;

    pt_entry->active_index = !(pt_entry->active_index & 0x01);
    pt_entry->age++;
    ret = pt_table_update_entry((pt_table_id_type)(!active_id), pt_stuff, pt_entry);

    if (ret != PT_ERROR_SUCCESS) {
        LOG_E("Update PT entry fail\r\n");
        return BFLB_BOOT2_FAIL;
    }

    return BFLB_BOOT2_SUCCESS;
}
#endif

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
static void blsp_boot2_get_mfg_startreq(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry, uint8_t *user_fw_name)
{
    uint32_t ret;
    uint32_t len = 0;
    uint8_t tmp[16 + 1] = { 0 };

    /* case 1 0mfg and 1 mfg, should return and boot */
    if ((arch_memcmp(user_fw_name, "0mfg", 4) == 0) || (arch_memcmp(user_fw_name, "1mfg", 4) == 0)) {
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
        LOG_F("read mfg flag addr:%08x\r\n", pt_entry->start_address[0] + MFG_START_REQUEST_OFFSET);
        bflb_flash_read(pt_entry->start_address[0] + MFG_START_REQUEST_OFFSET, tmp, sizeof(tmp) - 1);
        //LOG_F("%s\r\n", tmp);
        if (tmp[0] == '0' || tmp[0] == '1') {
            len = strlen((char *)tmp) - 1;
            if (len < 9) {
                arch_memcpy(user_fw_name, &tmp[1], len);
                LOG_F("flag:%s\r\n", tmp);
            }
        }
    } else {
        LOG_F("MFG not found\r\n");
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
    pt_table_entry_config pt_entry[BLSP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint32_t boot_header_addr[BLSP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint8_t boot_need_rollback[BLSP_BOOT2_CPU_GROUP_MAX] = { 0 };
    uint8_t user_fw_name[9] = { 0 };
    uint32_t user_fw;
#ifdef BLSP_BOOT2_ROLLBACK
    uint8_t roll_backed = 0;
#endif
    uint8_t mfg_mode_flag = 0;
    //boot_clk_config clk_cfg;
    uint8_t flash_cfg_buf[4 + sizeof(SPI_Flash_Cfg_Type) + 4] = { 0 };
    uint32_t crc;
    uint8_t *flash_cfg = NULL;
    uint32_t flash_cfg_len = 0;

    hal_boot2_init_clock();
    blsp_boot2_start_timer();
    hal_boot2_get_efuse_cfg(&g_efuse_cfg);

    bflb_eflash_loader_if_set(BFLB_EFLASH_LOADER_IF_UART);
    bflb_eflash_loader_if_init();

#if (BLSP_BOOT2_MODE == BOOT2_MODE_RELEASE)
    bflb_uart_set_console(NULL);
#endif

#if (BLSP_BOOT2_MODE == BOOT2_MODE_DEBUG)
    bflb_uart_set_console(uartx);
#endif

    simple_malloc_init(g_malloc_buf, sizeof(g_malloc_buf));
    g_boot2_read_buf = vmalloc(BFLB_BOOT2_READBUF_SIZE);
    ret = hal_boot2_custom(NULL);
    LOG_F("custom 0x%04x\r\n", ret);

    ret = bflb_flash_init();
    if (ret) {
        while (1) {
            LOG_E("flash cfg err\r\n");
            bflb_mtimer_delay_ms(500);
        }
    }
    LOG_F("flash init %d\r\n", (int)ret);

    LOG_F("BLSP Boot2 start:%s,%s\r\n", __DATE__, __TIME__);
    LOG_F("Group=%d,CPU Count=%d\r\n", BLSP_BOOT2_CPU_GROUP_MAX, BLSP_BOOT2_CPU_MAX);

    LOG_F("ver:%s\r\n", BL_SDK_VER);

    /* Reset Sec_Eng */
    hal_boot2_reset_sec_eng();
    sha = bflb_device_get_by_name("sha");

    bflb_group0_request_sha_access(sha);
    /* Get power save mode */
    g_ps_mode = blsp_read_power_save_mode();

    /* Get User specified FW */
    user_fw = hal_boot2_get_user_fw();
    arch_memcpy(user_fw_name, &user_fw, 4);
    LOG_F("user_fw %s\r\n", user_fw_name);

    /* Set flash operation function, read via xip */
    pt_table_set_flash_operation(bflb_flash_erase, bflb_flash_write, bflb_flash_read);

#if BLSP_BOOT2_SUPPORT_USB_IAP
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

#if BLSP_BOOT2_RAM_IMG_COUNT_MAX > 0
    /* Deal with ram image: copy from flash to ram like */
    do {
        active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

        if (PT_TABLE_ID_INVALID == active_id) {
            blsp_boot2_on_error("No valid PT\r\n");
        }
        ret = blsp_do_ram_image_boot(active_id, &pt_table_stuff[active_id], &pt_entry[0]);
    } while (ret != 0);
#endif
    /* Deal with flash image */
    while (1) {
        do {
            mfg_mode_flag = 0;
            active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

            if (PT_TABLE_ID_INVALID == active_id) {
                blsp_boot2_on_error("No valid PT\r\n");
            }

            LOG_F("Active PT:%d,Age %d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

            blsp_boot2_get_mfg_startreq(active_id, &pt_table_stuff[active_id], &pt_entry[0], user_fw_name);

            /* Get entry and boot */
            if (user_fw_name[0] != 0) {
                ret = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], user_fw_name, PT_ENTRY_FW_CPU0);
                if (!ret) {
                    g_user_hash_ignored = 1;
                    mfg_mode_flag = 1;

                } else {
                    ret = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], NULL, PT_ENTRY_FW_CPU0);
                    if (!ret) {
                        /* go on get partition type 1 (group1) info */
                        if (BLSP_BOOT2_CPU_GROUP_MAX > 1) {
                            ret = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[1], NULL, PT_ENTRY_FW_CPU1);
                        }
                    }
                }
                hal_boot2_clr_user_fw();
                arch_memset(user_fw_name, 0, 4);

            } else {
                /* get partition type 0 (group0) info */
                ret = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], NULL, PT_ENTRY_FW_CPU0);
                if (!ret) {
                    /* go on get partition type 1 (group1) info */
                    if (BLSP_BOOT2_CPU_GROUP_MAX > 1) {
                        ret = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[1], NULL, PT_ENTRY_FW_CPU1);
                    }
                }
            }
            if (ret == BFLB_BOOT2_PRE_DEAL_ENTRY_NOT_FOUND) {
                blsp_boot2_on_error("No valid entry\r\n");
            }
        } while (ret);

        /* Pass data to App*/
        blsp_boot2_pass_parameter(NULL, 0);
        /* Pass active partition table ID */
        blsp_boot2_pass_parameter(&active_id, 4);
        /* Pass active partition table content: table header+ entries +crc32 */
        blsp_boot2_pass_parameter(&pt_table_stuff[active_id], sizeof(pt_table_config) + 4 +
                                                                  pt_table_stuff[active_id].pt_table.entryCnt * sizeof(pt_table_entry_config));

        /* Pass flash config */
        if (pt_entry[0].start_address[pt_entry[0].active_index] != 0) {
            /* Include magic and CRC32 */
            bflb_flash_get_cfg(&flash_cfg, &flash_cfg_len);
            arch_memcpy(flash_cfg_buf, "FCFG", 4);
            arch_memcpy(flash_cfg_buf + 4, flash_cfg, flash_cfg_len);
            crc = BFLB_Soft_CRC32(flash_cfg, flash_cfg_len);
            arch_memcpy(flash_cfg_buf + 4 + flash_cfg_len, &crc, sizeof(crc));
            blsp_boot2_pass_parameter(flash_cfg_buf, sizeof(flash_cfg_buf));
        }

        LOG_F("Boot start\r\n");
        for (i = 0; i < BLSP_BOOT2_CPU_GROUP_MAX; i++) {
            boot_header_addr[i] = pt_entry[i].start_address[pt_entry[i].active_index];
        }
#ifdef BLSP_BOOT2_ROLLBACK
        /* mfg mode do not need roll back */
        if (roll_backed == 0 && mfg_mode_flag == 0) {
            ret = blsp_mediaboot_main(boot_header_addr, boot_need_rollback, 1);
        } else {
            ret = blsp_mediaboot_main(boot_header_addr, boot_need_rollback, 0);
        }
#else
        ret = blsp_mediaboot_main(boot_header_addr, boot_need_rollback, 0);
#endif

        if (mfg_mode_flag == 1) {
            continue;
        }

#ifdef BLSP_BOOT2_ROLLBACK
        /* If rollback is done, we still fail, break */
        if (roll_backed) {
            break;
        }

        LOG_F("Boot return 0x%04x\r\nCheck Rollback\r\n", ret);

        for (i = 0; i < BLSP_BOOT2_CPU_GROUP_MAX; i++) {
            if (boot_need_rollback[i] != 0) {
                LOG_F("Rollback group %d\r\n", i);

                if (BFLB_BOOT2_SUCCESS == blsp_boot2_rollback_ptentry(active_id, &pt_table_stuff[active_id], &pt_entry[i])) {
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
    LOG_E("Media boot return %d\r\n", ret);

    while (1) {
#if BLSP_BOOT2_SUPPORT_USB_IAP
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
        LOG_E("BLSP boot2 fail\r\n");
        bflb_mtimer_delay_ms(500);
    }
}

void bfl_main()
{
    main();
}
