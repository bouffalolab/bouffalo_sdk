/**
  ******************************************************************************
  * @file    bflb_sp_port.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 Bouffalo Lab</center></h2>
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

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdarg.h"
#include "bflb_sp_port.h"
#include "bflb_sp_bootinfo.h"
#include "bflb_sp_common.h"
#include "bflb_sp_media_boot.h"
#include "bflb_uart.h"
#include "bflb_sec_aes.h"
#include "bflb_sf_ctrl.h"
#include "bflb_wdg.h"

extern uint32_t __boot2_pass_param_addr;
static uint32_t start_time = 0;
struct bflb_device_s *boot2_wdg;
int g_cur_pos = 0;
#if defined(BL702)
uint8_t g_log_out_buf[BFLB_BOOT2_LOG_OUT_BUF_SIZE] __attribute__((section(".noinit_hbn_ram_data")));
#else
uint8_t g_log_out_buf[BFLB_BOOT2_LOG_OUT_BUF_SIZE];
#endif

#define MFG_VER_INFO_BASE (BFLB_SP_BOOT2_XIP_BASE + 0xc00)

#define COMPILE_TIME __DATE__ " " __TIME__
char ver_name[5] __attribute__ ((section(".verinfo"))) = "boot2";
char git_commit[41] __attribute__ ((section(".verinfo"))) = GIT_COMMIT;
char time_info[30] __attribute__ ((section(".verinfo"))) = COMPILE_TIME;
const bflb_verinf_t boot2_ver __attribute__ ((section(".bflb_verinf"))) = {
#if BOOT2_ANTI_ROLLBACK_VER
    .anti_rollback = BOOT2_ANTI_ROLLBACK_VER,
#else
    .anti_rollback = 0,
#endif
    .x = BFLB_BOOT2_VER[0] - 0x30,
    .y = BFLB_BOOT2_VER[2] - 0x30,
    .z = BFLB_BOOT2_VER[4] - 0x30,
    .name = (uint32_t)ver_name,
    .build_time = (uint32_t)time_info,
    .commit_id = (uint32_t)git_commit,
    .rsvd0 = 0,
    .rsvd1 = 0,
};

#define TIMER_CLK_DIV (31)
void boot2_wdt_init(void)
{
    struct bflb_wdg_config_s wdg_cfg;
    wdg_cfg.clock_source = WDG_CLKSRC_32K;
    wdg_cfg.clock_div = TIMER_CLK_DIV;
    wdg_cfg.comp_val = 30000;
    wdg_cfg.mode = WDG_MODE_RESET;
    boot2_wdg = bflb_device_get_by_name("watchdog0");

    bflb_wdg_stop(boot2_wdg);
    bflb_wdg_init(boot2_wdg, &wdg_cfg);

    bflb_wdg_start(boot2_wdg);
}

void boot2_wdt_set_value(uint16_t wdt_timeout)
{
    bflb_wdg_set_countervalue(boot2_wdg, wdt_timeout);
}

void boot2_wdt_feed(void)
{
    bflb_wdg_reset_countervalue(boot2_wdg);
}

void boot2_wdt_disable(void)
{
    bflb_wdg_stop(boot2_wdg);
}

/****************************************************************************/ /**
* @brief  Boot2 show timer for cal boot time
*
* @param  None
*
* @return None
*
*******************************************************************************/
void bflb_sp_boot2_start_timer(void)
{
    start_time = (uint32_t)bflb_mtimer_get_time_ms();
}

/****************************************************************************/ /**
* @brief  Boot2 show timer for cal boot time
*
* @param  None
*
* @return None
*
*******************************************************************************/
void bflb_sp_boot2_show_timer(void)
{
    BOOT2_MSG("Counter value=%d\n", (unsigned int)bflb_mtimer_get_time_ms() - start_time);
}

/****************************************************************************/ /**
 * @brief  Read power save mode
 *
 * @param  None
 *
 * @return BFLB_PSM_ACTIVE or BFLB_PSM_HBN
 *
*******************************************************************************/
uint8_t bflb_sp_read_power_save_mode(void)
{
    if (hal_boot2_get_psmode_status() == HBN_STATUS_WAKEUP_FLAG) {
        return BFLB_PSM_HBN;
    } else {
        return BFLB_PSM_ACTIVE;
    }
}

/****************************************************************************/ /**
 * @brief  get anti-rollback flag from HBN register which is saved by cpu before jump
 *
 * @param  None
 *
 * @return 1 for HBN register store anti-rollback flag,0 for not
 *
*******************************************************************************/
uint8_t bflb_sp_get_anti_rollback_flag(uint32_t *value)
{
    if (HBN_Get_Status_Flag() == HBN_STATUS_ANTIROLLBACK_FLAG) {
        /* use RSV1 register to pass anti-rollback value */
        *value = HBN_Get_Wakeup_Addr();
        return 1;
    }
    return 0;
}

/****************************************************************************/ /**
 * @brief  set anti-rollback flag to HBN register which is saved by cpu before jump
 *
 * @param  None
 *
 * @return 1 for HBN register store anti-rollback flag,0 for not
 *
*******************************************************************************/
ATTR_TCM_SECTION uint8_t bflb_sp_set_anti_rollback_flag(uint32_t value)
{
    HBN_Set_Status_Flag(HBN_STATUS_ANTIROLLBACK_FLAG);
    if (HBN_Get_Wakeup_Addr() == 0xffffffff) {
        HBN_Set_Wakeup_Addr((value + (1 << 16)));
    } else {
        HBN_Set_Wakeup_Addr((value | ((HBN_Get_Wakeup_Addr() & 0xffff00ff) + (1 << 16))));
    }
    return 0;
}

/****************************************************************************/ /**
 * @brief  clean anti-rollback flag
 *
 * @param  None
 *
 * @return 1 for HBN register store anti-rollback flag,0 for not
 *
*******************************************************************************/
ATTR_TCM_SECTION uint8_t bflb_sp_clean_anti_rollback_flag()
{
    HBN_Set_Status_Flag(0);
    if (((HBN_Get_Wakeup_Addr() >> 16) & 0xff) > 0x02) {
        HBN_Set_Wakeup_Addr(0);
    } else {
        HBN_Set_Wakeup_Addr((HBN_Get_Wakeup_Addr() & 0x00ff0000));
    }
    return 0;
}

/****************************************************************************/ /**
 * @brief  Boot2 Pass parameter to FW
 *
 * @param  data: Data pointer to pass
 * @param  len: Data length
 *
 * @return None
 *
*******************************************************************************/
void bflb_sp_boot2_pass_parameter(void *data, uint32_t len)
{
    static uint8_t *p_parameter = NULL;

    if (len == 0) {
        //GLB_Set_EM_Sel(0); //system init has done
        //p_parameter = (uint8_t *)(0x42020000 + 60 * 1024);
        //p_parameter = (uint8_t *)(0x42030000+103*1024);
        p_parameter = (uint8_t *)&__boot2_pass_param_addr;
        return;
    }

    if(p_parameter!=NULL){
        BOOT2_MSG_DBG("pass param addr %x,len %d\r\n", p_parameter, len);
        arch_memcpy_fast(p_parameter, data, len);
        p_parameter += len;
    }
}

/****************************************************************************/ /**
 * @brief  Release other CPU
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION bflb_sp_boot2_releae_other_cpu(void)
{
#if BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX > 0
    uint32_t i = 0;
    pentry_t pentry;
#endif
#if BFLB_SP_BOOT2_CPU_MAX > 1
    uint32_t group, core;
    /* deal Other CPUs' entry point and release*/
    for (group = 0; group < BFLB_SP_BOOT2_CPU_GROUP_MAX; group++) {
        for (core = 1; core < BFLB_SP_BOOT2_CPU_MAX; core++) {
            if (g_boot_img_cfg[group].cpu_cfg[core].config_enable) {
                if (g_boot_img_cfg[group].cpu_cfg[core].halt_cpu == 0) {
                    hal_boot2_release_cpu(core, g_boot_img_cfg[group].cpu_cfg[core].boot_entry);
                }
            }
        }
    }
#endif
#if BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX > 0
    /* deal with ram image */
    for (i = 0; i < BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX; i++) {
        if (ram_img_config[i].valid && ram_img_config[i].core != 0 &&
            ram_img_config[i].core != 0xf) {
            hal_boot2_release_cpu(ram_img_config[i].core, ram_img_config[i].boot_addr);
        }
    }
    for (i = 0; i < BFLB_SP_BOOT2_RAM_IMG_COUNT_MAX; i++) {
        if (ram_img_config[i].valid && ram_img_config[i].core == 0) {
            pentry = (pentry_t)ram_img_config[i].boot_addr;
            //cpu_global_irq_disable();
            __disable_irq();
            hal_boot2_clean_cache();
            pentry();
        }
    }
#endif
}

/****************************************************************************/ /**
 * @brief  Set encryption config
 *
 * @param  index: Region index
 * @param  g_boot_img_cfg: Boot image config pointer to hold parsed information
 *
 * @return BL_Err_Type
 *
*******************************************************************************/
int32_t ATTR_TCM_SECTION bflb_sp_boot2_set_encrypt(uint8_t index, boot2_image_config *g_boot_img_cfg)
{
    uint32_t aes_enabled = 0;
    uint32_t len = 0;

    if (g_boot_img_cfg->img_valid == 0) {
        return BFLB_BOOT2_SUCCESS;
    }
    /* FIXME:,1:lock, should be 1??*/
    if (g_boot_img_cfg->basic_cfg.encrypt_type != 0) {
        len = g_boot_img_cfg->basic_cfg.img_len_cnt;

        if (len != 0) {
            aes_enabled = 1;
            if (g_boot_img_cfg->basic_cfg.xts_mode) {
#if defined(BL616) || defined(BL606P) || defined(BL808)
                bflb_sf_ctrl_disable_wrap_access(0);
                bflb_sf_ctrl_aes_set_mode(SF_CTRL_AES_XTS_MODE);
                bflb_sf_ctrl_aes_xts_set_key_be(index, NULL, (g_boot_img_cfg->basic_cfg.encrypt_type - 1));
                bflb_sf_ctrl_aes_xts_set_iv_be(index, g_boot_img_cfg->aes_iv, g_boot_img_cfg->basic_cfg.group_image_offset);

                bflb_sf_ctrl_aes_set_region(index, 1 /*enable this region*/, 1 /*hardware key*/,
                                       g_boot_img_cfg->basic_cfg.group_image_offset,
                                       g_boot_img_cfg->basic_cfg.group_image_offset + len - 1,
                                       g_boot_img_cfg->basic_cfg.aes_region_lock /*lock*/);
#endif
            } else {
#if defined(BL616) || defined(BL606P) || defined(BL808)
                bflb_sf_ctrl_disable_wrap_access(1);
                bflb_sf_ctrl_aes_set_mode(SF_CTRL_AES_CTR_MODE);
#endif
                bflb_sf_ctrl_aes_set_key_be(index, NULL, (g_boot_img_cfg->basic_cfg.encrypt_type - 1));
                bflb_sf_ctrl_aes_set_iv_be(index, g_boot_img_cfg->aes_iv, g_boot_img_cfg->basic_cfg.group_image_offset);

                bflb_sf_ctrl_aes_set_region(index, 1 /*enable this region*/, 1 /*hardware key*/,
                                       g_boot_img_cfg->basic_cfg.group_image_offset,
                                       g_boot_img_cfg->basic_cfg.group_image_offset + len - 1,
                                       g_boot_img_cfg->basic_cfg.aes_region_lock /*lock*/);
            }
        }
    }

    if (aes_enabled) {
        bflb_sf_ctrl_aes_enable_be();
        bflb_sf_ctrl_aes_enable();
    }

    return BFLB_BOOT2_SUCCESS;
}

/****************************************************************************/ /**
 * @brief  boot2 print,support %x,%d,%s
 *
 * @param
 * @param
 *
 * @return
 *
*******************************************************************************/
void bflb_sp_boot2_printf(const char *format, ...)
{
    va_list arg_list;
    char output[128];
    int pos = 0;

    va_start(arg_list, format);

    while (*format != '\0') {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 's': {
                    // handle string
                    char *str = va_arg(arg_list, char *);
                    while (*str != '\0') {
                        output[pos++] = *str++;
                    }
                    break;
                }
                case 'c': {
                    // handle character
                    char c = (char)va_arg(arg_list, int);
                    output[pos++] = c;
                    break;
                }
                case 'd': {
                    // handle decimal integer
                    int num = va_arg(arg_list, int);
                    if (num == 0) {
                        output[pos++] = '0';
                    } else {
                        char buf[32];
                        int i = 0;
                        if (num < 0) {
                            output[pos++] = '-';
                            num = -num;
                        }
                        while (num > 0) {
                            buf[i++] = '0' + num % 10;
                            num /= 10;
                        }
                        while (i > 0) {
                            output[pos++] = buf[--i];
                        }
                    }
                    break;
                }
                case 'x': {
                    // handle hexadecimal integer
                    unsigned int num = va_arg(arg_list, unsigned int);
                    if (num == 0) {
                        output[pos++] = '0';
                    } else {
                        char buf[32];
                        int i = 0;
                        while (num > 0) {
                            unsigned int digit = num % 16;
                            if (digit < 10) {
                                buf[i++] = '0' + digit;
                            } else {
                                buf[i++] = 'a' + digit - 10;
                            }
                            num /= 16;
                        }
                        while (i > 0) {
                            output[pos++] = buf[--i];
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        } else {
            output[pos++] = *format;
        }
        format++;
    }
    output[pos] = '\0';

    va_end(arg_list);

    if ((g_cur_pos + pos) < BFLB_BOOT2_LOG_OUT_BUF_SIZE) {
        memcpy(&g_log_out_buf[g_cur_pos], output, pos);
        g_cur_pos += pos;
    }

    if(NULL == console){
        return;
    }

    bflb_uart_put_block(uartx, (uint8_t *)output, strlen(output));

}

void bflb_boot2_release_dump_log(void)
{
    char *str_log = "Dump log...\r\n";
    bflb_uart_put_block(uartx, (uint8_t *)str_log, strlen(str_log));
    if (g_cur_pos > 0) {
        bflb_uart_put_block(uartx, g_log_out_buf, g_cur_pos);
    }
    // g_cur_pos = 0;
}
