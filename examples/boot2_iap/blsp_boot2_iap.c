/**
  ******************************************************************************
  * @file    blsp_boot2.c
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

#include "bflb_platform.h"
#include "blsp_port.h"
#include "blsp_bootinfo.h"
#include "blsp_media_boot.h"
#include "partition.h"
#include "blsp_boot_decompress.h"
#include "blsp_common.h"
#include "softcrc.h"
#include "bflb_eflash_loader_uart.h"
#include "hal_uart.h"
#include "hal_flash.h"
#include "blsp_version.h"

/** @addtogroup  BL606_BLSP_Boot2
 *  @{
 */

/** @addtogroup  BLSP_BOOT2
 *  @{
 */

/** @defgroup  BLSP_BOOT2_Private_Macros
 *  @{
 */

/*@} end of group BLSP_BOOT2_Private_Macros */

/** @defgroup  BLSP_BOOT2_Private_Types
 *  @{
 */

/*@} end of group BLSP_BOOT2_Private_Types */

/** @defgroup  BLSP_BOOT2_Private_Variables
 *  @{
 */

/*@} end of group BLSP_BOOT2_Private_Variables */

/** @defgroup  BLSP_BOOT2_Global_Variables
 *  @{
 */
uint8_t g_boot2_read_buf[BFLB_BOOT2_READBUF_SIZE] __attribute__((section(".noinit_data")));
boot_image_config g_boot_img_cfg[2];
boot_cpu_config g_boot_cpu_cfg[2] = {
    /*CPU0 boot cfg*/
    {
        .msp_store_addr = 0,
        .pc_store_addr = 0,
        .default_xip_addr = 0x23000000,
    },
    /*CPU1 boot cfg*/
    {
        .msp_store_addr = BFLB_BOOT2_CPU1_APP_MSP_ADDR,
        .pc_store_addr = BFLB_BOOT2_CPU1_APP_PC_ADDR,
        .default_xip_addr = 0x23000000,
    }
};
boot_efuse_hw_config g_efuse_cfg;
uint8_t g_ps_mode = BFLB_PSM_ACTIVE;
uint8_t g_cpu_count;
uint32_t g_user_hash_ignored = 0;
int32_t blsp_boot2_get_clk_cfg(boot_clk_config *cfg);
void blsp_boot2_get_efuse_cfg(boot_efuse_hw_config *g_efuse_cfg);

/*@} end of group BLSP_BOOT2_Global_Variables */

/** @defgroup  BLSP_BOOT2_Private_Fun_Declaration
 *  @{
 */

/*@} end of group BLSP_BOOT2_Private_Fun_Declaration */

/** @defgroup  BLSP_BOOT2_Private_Functions_User_Define
 *  @{
 */

/*@} end of group BLSP_BOOT2_Private_Functions_User_Define */

/** @defgroup  BLSP_BOOT2_Private_Functions
 *  @{
 */

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
        if (BFLB_EFLASH_LOADER_HANDSHAKE_SUSS == bflb_eflash_loader_uart_handshake_poll()) {
            bflb_eflash_loader_main();
        }

        MSG_ERR("%s\r\n", (char *)log);
        ARCH_Delay_MS(500);
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
static void blsp_dump_pt_entry(pt_table_entry_config *pt_entry)
{
    MSG("Name=%s\r\n", pt_entry->name);
    MSG("Age=%d\r\n", (unsigned int)pt_entry->age);
    MSG("active Index=%d\r\n", (unsigned int)pt_entry->active_index);
    MSG("active start_address=%08x\r\n", (unsigned int)pt_entry->start_address[pt_entry->active_index]);
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
#if 0
static int BLSP_Boot2_Check_XZ_FW(pt_table_id_type activeID, pt_table_stuff_config *ptStuff, pt_table_entry_config *ptEntry)
{
    uint8_t buf[6];

    if(BFLB_BOOT2_SUCCESS != blsp_mediaboot_read(ptEntry->start_address[ptEntry->active_index], buf, sizeof(buf)))
    {
        MSG_ERR("Read fw fail\r\n");
        return 0;
    }

    if(blsp_boot2_dump_critical_flag())
    {
        blsp_dump_data(buf, sizeof(buf));
    }

    if(blsp_boot2_verify_xz_header(buf) == 1)
    {
        MSG_DBG("XZ image\r\n");

        if(BFLB_BOOT2_SUCCESS == blsp_boot2_update_fw(activeID, ptStuff, ptEntry))
        {
            return 1;
        }
        else
        {
            MSG_ERR("Img decompress fail\r\n");
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

    if (SUCCESS != flash_erase_xip(dest_address, dest_address + dest_max_size - 1)) {
        MSG_ERR("Erase flash fail");
        return BFLB_BOOT2_FLASH_ERASE_ERROR;
    }

    while (deal_len < total_len) {
        cur_len = total_len - deal_len;

        if (cur_len > sizeof(g_boot2_read_buf)) {
            cur_len = sizeof(g_boot2_read_buf);
        }

        if (BFLB_BOOT2_SUCCESS != blsp_mediaboot_read(src_address, g_boot2_read_buf, cur_len)) {
            MSG_ERR("Read FW fail when copy\r\n");
            return BFLB_BOOT2_FLASH_READ_ERROR;
        }

        if (SUCCESS != flash_write_xip(dest_address, g_boot2_read_buf, cur_len)) {
            MSG_ERR("Write flash fail");
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
 * @return 0 for partition table changed,need re-parse,1 for partition table or entry parsed successfully
 *
*******************************************************************************/
static int blsp_boot2_deal_one_fw(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff,
                                  pt_table_entry_config *pt_entry, uint8_t *fw_name,
                                  pt_table_entry_type type)
{
    uint32_t ret;

    if (fw_name != NULL) {
        MSG_DBG("Get FW:%s\r\n", fw_name);
        ret = pt_table_get_active_entries_by_name(pt_stuff, fw_name, pt_entry);
    } else {
        MSG_DBG("Get FW ID:%d\r\n", type);
        ret = pt_table_get_active_entries_by_id(pt_stuff, type, pt_entry);
    }

    if (PT_ERROR_SUCCESS != ret) {
        MSG_ERR("Entry not found\r\n");
    } else {
        blsp_dump_pt_entry(pt_entry);
        MSG_DBG("Check Img\r\n");

        //if(BLSP_Boot2_Check_XZ_FW(activeID,ptStuff,ptEntry)==1){
        //return 0;
        //}
        /* Check if this partition need copy */
        if (pt_entry->active_index >= 2) {
            if (BFLB_BOOT2_SUCCESS == blsp_boot2_do_fw_copy(active_id, pt_stuff, pt_entry)) {
                return 0;
            }
        }
    }

    return 1;
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
static int32_t blsp_boot2_rollback_ptentry(pt_table_id_type active_id, pt_table_stuff_config *pt_stuff, pt_table_entry_config *pt_entry)
{
    int32_t ret;

    pt_entry->active_index = !(pt_entry->active_index & 0x01);
    pt_entry->age++;
    ret = pt_table_update_entry((pt_table_id_type)(!active_id), pt_stuff, pt_entry);

    if (ret != PT_ERROR_SUCCESS) {
        MSG_ERR("Update PT entry fail\r\n");
        return BFLB_BOOT2_FAIL;
    }

    return BFLB_BOOT2_SUCCESS;
}
#endif

/*@} end of group BLSP_BOOT2_Private_Functions */

/** @defgroup  BLSP_BOOT2_Public_Functions
 *  @{
 */

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
    pt_table_entry_config pt_entry[BFLB_BOOT2_CPU_MAX] = { 0 };
    uint32_t boot_header_addr[BFLB_BOOT2_CPU_MAX] = { 0 };
    uint8_t boot_rollback[BFLB_BOOT2_CPU_MAX] = { 0 };
    uint8_t pt_parsed = 1;
#ifdef BLSP_BOOT2_ROLLBACK
    uint8_t roll_backed = 0;
#endif
    uint8_t temp_mode = 0;
    //boot_clk_config clk_cfg;
    uint8_t flash_cfg_buf[4 + sizeof(SPI_Flash_Cfg_Type) + 4] = { 0 };

    bflb_eflash_loader_uart_init();
    bflb_platform_init(0);
    flash_init();

    bflb_platform_print_set(blsp_boot2_get_log_disable_flag());
    bflb_platform_deinit_time();

    if (blsp_boot2_get_feature_flag() == BLSP_BOOT2_CP_FLAG) {
        MSG_DBG("BLSP_Boot2_CP:%s,%s\r\n", __DATE__, __TIME__);
    } else if (blsp_boot2_get_feature_flag() == BLSP_BOOT2_MP_FLAG) {
        MSG_DBG("BLSP_Boot2_MC:%s,%s\r\n", __DATE__, __TIME__);
    } else {
        MSG_DBG("BLSP_Boot2_SP:%s,%s\r\n", __DATE__, __TIME__);
    }

#ifdef BL_SDK_VER
    MSG_DBG("SDK:%s\r\n", BL_SDK_VER);
#else
    MSG_DBG("MCU SDK:%s\r\n", MCU_SDK_VERSION);
    MSG_DBG("BSP Driver:%s\r\n", BSP_DRIVER_VERSION);
    MSG_DBG("BSP Common:%s\r\n", BSP_COMMON_VERSION);
#endif

    if (blsp_boot2_dump_critical_flag()) {
        //blsp_dump_data(&clk_cfg,16);
    }

    MSG_DBG("Get efuse config\r\n");
    blsp_boot2_get_efuse_cfg(&g_efuse_cfg);

    /* Reset Sec_Eng for using */
    blsp_boot2_reset_sec_eng();

    if (blsp_boot2_get_feature_flag() != BLSP_BOOT2_SP_FLAG) {
        /* Get cpu count info */
        g_cpu_count = blsp_boot2_get_cpu_count();
    } else {
        g_cpu_count = 1;
    }

    /* Get power save mode */
    g_ps_mode = blsp_read_power_save_mode();

    /* Get User specified FW */
    //ARCH_MemCpy_Fast(userFwName,blsp_get_user_specified_fw(),4);
    if (blsp_boot2_8m_support_flag()) {
        /* Set flash operation function, read via sbus */
        pt_table_set_flash_operation(flash_erase_xip, flash_write_xip, flash_read_xip);
    } else {
        /* Set flash operation function, read via xip */
        pt_table_set_flash_operation(flash_erase_xip, flash_write_xip, flash_read_xip);
    }

    while (1) {
        temp_mode = 0;

        do {
            active_id = pt_table_get_active_partition_need_lock(pt_table_stuff);

            if (PT_TABLE_ID_INVALID == active_id) {
                blsp_boot2_on_error("No valid PT\r\n");
            }

            MSG_DBG("Active PT:%d,%d\r\n", active_id, pt_table_stuff[active_id].pt_table.age);

            pt_parsed = blsp_boot2_deal_one_fw(active_id, &pt_table_stuff[active_id], &pt_entry[0], NULL, PT_ENTRY_FW_CPU0);

            if (pt_parsed == 0) {
                continue;
            }

            pt_parsed = 1;
        } while (pt_parsed == 0);

        /* Pass data to App*/
        blsp_boot2_pass_parameter(NULL, 0);
        /* Pass active partition table ID */
        blsp_boot2_pass_parameter(&active_id, 4);
        /* Pass active partition table content: table header+ entries +crc32 */
        blsp_boot2_pass_parameter(&pt_table_stuff[active_id], sizeof(pt_table_config) + 4 +
                                                                  pt_table_stuff[active_id].pt_table.entryCnt * sizeof(pt_table_entry_config));

        /* Pass flash config */
        if (pt_entry[0].start_address[pt_entry[0].active_index] != 0) {
            XIP_SFlash_Read_Via_Cache_Need_Lock(BLSP_BOOT2_XIP_BASE + pt_entry[0].start_address[pt_entry[0].active_index] + 8, flash_cfg_buf, sizeof(flash_cfg_buf));
            /* Include magic and CRC32 */
            blsp_boot2_pass_parameter(flash_cfg_buf, sizeof(flash_cfg_buf));
        }

        MSG_DBG("Boot start\r\n");

        for (i = 0; i < g_cpu_count; i++) {
            boot_header_addr[i] = pt_entry[i].start_address[pt_entry[i].active_index];
        }

#ifdef BLSP_BOOT2_ROLLBACK

        /* Test mode is not need roll back */
        if (roll_backed == 0 && temp_mode == 0) {
            ret = blsp_mediaboot_main(boot_header_addr, boot_rollback, 1);
        } else {
            ret = blsp_mediaboot_main(boot_header_addr, boot_rollback, 0);
        }

#else
        ret = blsp_mediaboot_main(boot_header_addr, boot_rollback, 0);
#endif

        /* Fail in temp mode,continue to boot normal image */
        if (temp_mode == 1) {
            continue;
        }

#ifdef BLSP_BOOT2_ROLLBACK

        /* If rollback is done, we still fail, break */
        if (roll_backed) {
            break;
        }

        MSG_DBG("Boot return %d\r\n", ret);
        MSG_DBG("Check Rollback\r\n");

        for (i = 0; i < g_cpu_count; i++) {
            if (boot_rollback[i] != 0) {
                MSG_DBG("Rollback %d\r\n", i);

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
    MSG_ERR("Media boot return %d\r\n", ret);

    while (1) {
        if (BFLB_EFLASH_LOADER_HANDSHAKE_SUSS == bflb_eflash_loader_uart_handshake_poll()) {
            bflb_eflash_loader_main();
        }

        MSG_ERR("BLSP boot2 fail\r\n");
        ARCH_Delay_MS(500);
    }
}

void bfl_main()
{
    main();
}

/*@} end of group BLSP_BOOT2_Public_Functions */

/*@} end of group BLSP_BOOT2 */

/*@} end of group BL606_BLSP_Boot2 */
