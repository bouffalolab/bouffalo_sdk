/**
  ******************************************************************************
  * @file    blsp_port.c
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
#include "blsp_port.h"
#include "bflb_platform.h"
#include "blsp_bootinfo.h"
#include "blsp_common.h"
#include "blsp_media_boot.h"
#include "tzc_sec_reg.h"
#include "hal_boot2.h"

extern uint32_t __boot2_pass_param_addr;

/****************************************************************************/ /**
* @brief  Boot2 show timer for cal boot time
*
* @param  None
*
* @return None
*
*******************************************************************************/
void blsp_boot2_show_timer(void)
{
    MSG("Counter value=%d\n", (unsigned int)bflb_platform_get_time_ms());
}

/****************************************************************************/ /**
* @brief  Boot2 get cpu count info
*
* @param  None
*
* @return None
*
*******************************************************************************/
uint32_t blsp_boot2_get_cpu_count(void)
{
    return 1;
}

/****************************************************************************/ /**
 * @brief  Read power save mode
 *
 * @param  None
 *
 * @return BFLB_PSM_ACTIVE or BFLB_PSM_HBN
 *
*******************************************************************************/
uint8_t blsp_read_power_save_mode(void)
{
    if (hal_boot2_get_psmode_status() == HBN_STATUS_WAKEUP_FLAG) {
        return BFLB_PSM_HBN;
    } else {
        return BFLB_PSM_ACTIVE;
    }
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
void blsp_boot2_pass_parameter(void *data, uint32_t len)
{
    static uint8_t *p_parameter = NULL;
    
    if (len == 0) {
        //GLB_Set_EM_Sel(0); //system init has done
        //p_parameter = (uint8_t *)(0x42020000 + 60 * 1024);
        //p_parameter = (uint8_t *)(0x42030000+103*1024);
        p_parameter = (uint8_t *)&__boot2_pass_param_addr;
        return;
    }

    MSG("pass param addr %08x,len %d\r\n", p_parameter,len);
    ARCH_MemCpy_Fast(p_parameter, data, len);
    p_parameter += len;
}

/****************************************************************************/ /**
 * @brief  Release other CPU
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void ATTR_TCM_SECTION blsp_boot2_releae_other_cpu(void)
{
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
int32_t ATTR_TCM_SECTION blsp_boot2_set_encrypt(uint8_t index, boot2_image_config *g_boot_img_cfg)
{
    uint32_t aes_enabled = 0;
    uint32_t len = 0;

    /* FIXME:,1:lock, should be 1??*/
    if (g_boot_img_cfg->encrypt_type != 0) {
        len = g_boot_img_cfg->img_segment_info.img_len;

        if (len != 0) {
            SF_Ctrl_AES_Set_Key_BE(index, NULL, (SF_Ctrl_AES_Key_Type)(g_boot_img_cfg->encrypt_type - 1));
            SF_Ctrl_AES_Set_IV_BE(index, g_boot_img_cfg->aes_iv, g_boot_img_cfg->img_start.flash_offset);

            SF_Ctrl_AES_Set_Region(index, 1 /*enable this region*/, 1 /*hardware key*/,
                                   g_boot_img_cfg->img_start.flash_offset,
                                   g_boot_img_cfg->img_start.flash_offset + len - 1,
                                   g_boot_img_cfg->aes_region_lock /*lock*/);
            aes_enabled = 1;
        }
    }

    if (aes_enabled) {
        SF_Ctrl_AES_Enable_BE();
        SF_Ctrl_AES_Enable();
    }

    return BFLB_BOOT2_SUCCESS;
}

typedef void (*pFunc)(void);
extern pFunc __Vectors[];
/****************************************************************************/ /**
 * @brief  Boot2 Get log tx GPIO
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_get_tx_gpio(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[9] + 1);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get UART Port
 *
 * @param  None
 *
 * @return 0(UART0)/1(UART1)
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_get_uart_port(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[9] + 2);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get Feature Flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_get_feature_flag(void)
{
    static uint8_t boot2_flag = 0xff;
    uint8_t *p = ((uint8_t *)&__Vectors[10] + 0);

    if (boot2_flag == 0xff) {
        boot2_flag = *p;
    }

    return boot2_flag;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get log disable Flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_get_log_disable_flag(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[10] + 1);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get 8M Flash support Flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_8m_support_flag(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[10] + 2);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get dump critical data flag
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint8_t ATTR_TCM_SECTION blsp_boot2_dump_critical_flag(void)
{
    uint8_t *p = ((uint8_t *)&__Vectors[10] + 3);

    return *p;
}

/****************************************************************************/ /**
 * @brief  Boot2 Get Baudrate
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
uint32_t ATTR_TCM_SECTION blsp_boot2_get_baudrate(void)
{
    uint32_t *p = ((uint32_t *)&__Vectors[13]);

    return *p;
}



