/**
  ******************************************************************************
  * @file    blsp_eflash_loader_cmds.c
  * @version V1.2
  * @date
  * @brief   This file is the peripheral case header file
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
#include "bflb_eflash_loader_cmds.h"
#include "bflb_eflash_loader.h"
#include "bflb_eflash_loader_uart.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "softcrc.h"
#include "bflb_platform.h"
#include "partition.h"
#include "bl702_sec_eng.h"
#include "hal_flash.h"
#include "bl702_hbn.h"
#include "bl702_glb.h"

/*for mass read comamnd(flash read and efuse read) is only valid for UART
  this is due to ack buffer is g_eflash_loader_readbuf */
#define eflash_loader_cmd_mass_ack_buf g_eflash_loader_readbuf[1]
#define BFLB_EFLASH_LOADER_CHECK_LEN   2048
#define BFLB_EFLASH_MAX_SIZE           2 * 1024 * 1024

/*add for verify using SHA-256*/
uint32_t g_sha_tmp_buf[16] = { 0 };
uint32_t g_padding[16] = { 0 };
uint32_t g_sha_in_buf[(BFLB_EFLASH_LOADER_READBUF_SIZE + 3) / 4] = { 0 };
static uint32_t g_eflash_loader_error = 0;
extern struct device *download_uart;

static int32_t bflb_eflash_loader_cmd_read_jedec_id(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_reset(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_erase_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_write_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_read_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_readSha_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_xip_readSha_flash(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_write_flash_check(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_set_flash_para(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_xip_read_flash_start(uint16_t cmd, uint8_t *data, uint16_t len);
static int32_t bflb_eflash_loader_cmd_xip_read_flash_finish(uint16_t cmd, uint8_t *data, uint16_t len);

static const struct eflash_loader_cmd_cfg_t eflash_loader_cmds[] = {
    { BFLB_EFLASH_LOADER_CMD_RESET, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_reset },
    { BFLB_EFLASH_LOADER_CMD_FLASH_ERASE, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_erase_flash },
    { BFLB_EFLASH_LOADER_CMD_FLASH_WRITE, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_write_flash },
    { BFLB_EFLASH_LOADER_CMD_FLASH_READ, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_read_flash },
    { BFLB_EFLASH_LOADER_CMD_FLASH_WRITE_CHECK, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_write_flash_check },
    { BFLB_EFLASH_LOADER_CMD_FLASH_SET_PARA, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_set_flash_para },
    { BFLB_EFLASH_LOADER_CMD_FLASH_READSHA, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_readSha_flash },
    { BFLB_EFLASH_LOADER_CMD_FLASH_XIP_READSHA, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_xip_readSha_flash },
    { BFLB_EFLASH_LOADER_CMD_XIP_READ_START, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_xip_read_flash_start },
    { BFLB_EFLASH_LOADER_CMD_XIP_READ_FINISH, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_xip_read_flash_finish },
    { BFLB_EFLASH_LOADER_CMD_FLASH_READ_JEDECID, EFLASH_LOADER_CMD_ENABLE, bflb_eflash_loader_cmd_read_jedec_id },
};

/* int boot command control */
void bflb_eflash_loader_cmd_init()
{
    g_eflash_loader_error = BFLB_EFLASH_LOADER_SUCCESS;
    //download_uart = device_find("download_uart");
}

/* ack host with command process result */
static void bflb_eflash_loader_cmd_ack(uint32_t result)
{
    if (result == 0) {
        /*OK*/
        g_eflash_loader_cmd_ack_buf[0] = BFLB_EFLASH_LOADER_CMD_ACK;
        bflb_eflash_loader_uart_send((uint32_t *)g_eflash_loader_cmd_ack_buf, 2);
        return;
    } else {
        /* FL+Error code(2bytes) */
        g_eflash_loader_cmd_ack_buf[0] = BFLB_EFLASH_LOADER_CMD_NACK | ((result << 16) & 0xffff0000);
        bflb_eflash_loader_uart_send(g_eflash_loader_cmd_ack_buf, 4);
    }
}

static int32_t bflb_eflash_loader_cmd_read_jedec_id(uint16_t cmd, uint8_t *data, uint16_t len)
{
    uint8_t *ackdata = (uint8_t *)eflash_loader_cmd_mass_ack_buf;

    bflb_eflash_loader_printf("JID\n");
    eflash_loader_cmd_mass_ack_buf[0] = BFLB_EFLASH_LOADER_CMD_ACK;

    /*ack read jedec ID */
    ackdata[2] = 4;
    ackdata[3] = 0;
    flash_read_jedec_id((uint8_t *)&eflash_loader_cmd_mass_ack_buf[1]);
    bflb_eflash_loader_uart_send((uint32_t *)ackdata, 4 + 4);
    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_eflash_loader_cmd_reset(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    pt_table_set_iap_para(&p_iap_param);
    pt_table_dump();
    bflb_eflash_loader_printf("RST\n");

    bflb_eflash_loader_cmd_ack(ret);
    bflb_eflash_loader_usart_wait_tx_idle(BFLB_EFLASH_LOADER_IF_TX_IDLE_TIMEOUT);

    /* add for bl702, will impact on boot pin read */
    HBN_Set_Status_Flag(0x594c440B);

    /* FPGA POR RST NOT work,so add system reset */
    bflb_platform_delay_us(10);
    GLB_SW_System_Reset();

    return ret;
}

static int32_t bflb_eflash_loader_cmd_erase_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;
    uint32_t startaddr, endaddr;

    bflb_eflash_loader_printf("E\n");

    if (len != 8) {
        ret = BFLB_EFLASH_LOADER_FLASH_ERASE_PARA_ERROR;
    } else {
        /*clean write error, since write usually behand erase*/
        g_eflash_loader_error = BFLB_EFLASH_LOADER_SUCCESS;

        memcpy(&startaddr, data, 4);
        memcpy(&endaddr, data + 4, 4);

        p_iap_param.iap_img_len = endaddr - startaddr + 1;

        bflb_eflash_loader_printd("from%08xto%08x\n", p_iap_param.iap_start_addr, p_iap_param.iap_start_addr + p_iap_param.iap_img_len - 1);

        if (SUCCESS != flash_erase_xip(p_iap_param.iap_start_addr, p_iap_param.iap_start_addr + p_iap_param.iap_img_len - 1)) {
            bflb_eflash_loader_printe("fail\n");
            ret = BFLB_EFLASH_LOADER_FLASH_ERASE_ERROR;
        }
    }

    bflb_eflash_loader_cmd_ack(ret);
    return ret;
}

static int32_t ATTR_TCM_SECTION bflb_eflash_loader_cmd_write_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;
    uint32_t write_len;

    //bflb_eflash_loader_printf("W\n");

    if (len <= 4) {
        ret = BFLB_EFLASH_LOADER_FLASH_WRITE_PARA_ERROR;
    } else {
        //memcpy(&startaddr,data,4);
        write_len = len - 4;
        //MSG("to%08x,%d\n",p_iap_param.iap_write_addr,write_len);
        bflb_platform_clear_time();

        if (p_iap_param.iap_write_addr < 0xffffffff) {
            bflb_eflash_loader_cmd_ack(ret);

            if (SUCCESS != flash_write_xip(p_iap_param.iap_write_addr, data + 4, write_len)) {
                /*error , response again with error */
                bflb_eflash_loader_printe("fail\r\n");
                ret = BFLB_EFLASH_LOADER_FLASH_WRITE_ERROR;
                g_eflash_loader_error = ret;
            } else {
                p_iap_param.iap_write_addr += write_len;
                //bflb_eflash_loader_printe("Write suss\r\n");
                return BFLB_EFLASH_LOADER_SUCCESS;
            }
        } else {
            ret = BFLB_EFLASH_LOADER_FLASH_WRITE_ADDR_ERROR;
        }

        bflb_eflash_loader_printd("%d\n", bflb_platform_get_time_us());
    }

    bflb_eflash_loader_cmd_ack(ret);
    return ret;
}

static int32_t bflb_eflash_loader_cmd_read_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_eflash_loader_cmd_xip_read_flash_start(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    bflb_eflash_loader_cmd_ack(ret);
    return ret;
}

static int32_t bflb_eflash_loader_cmd_xip_read_flash_finish(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    //bflb_eflash_loader_printf("exit\n");
    bflb_eflash_loader_cmd_ack(ret);
    return ret;
}

static int32_t bflb_eflash_loader_cmd_readSha_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_eflash_loader_cmd_xip_readSha_flash(uint16_t cmd, uint8_t *data, uint16_t len)
{
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;
    uint32_t startaddr, read_len;
    SEC_Eng_SHA256_Ctx sha_ctx;
    SEC_ENG_SHA_ID_Type shaId = SEC_ENG_SHA_ID0;
    uint16_t sha_len = 32;
    uint8_t *ackdata = (uint8_t *)eflash_loader_cmd_mass_ack_buf;
    bflb_eflash_loader_printf("XRSha\n");

    if (len != 8) {
        ret = BFLB_EFLASH_LOADER_FLASH_WRITE_PARA_ERROR;
        bflb_eflash_loader_cmd_ack(ret);
    } else {
        startaddr = p_iap_param.iap_start_addr;
        read_len = p_iap_param.iap_img_len;
        bflb_eflash_loader_printd("from%08x,%d\n", startaddr, read_len);
        //MSG("!!!Be careful that SHA input data should locate at OCRAM \n");
        /* Init SHA and input SHA temp buffer for scattered data and g_padding buffer */
        Sec_Eng_SHA256_Init(&sha_ctx, shaId, SEC_ENG_SHA256, g_sha_tmp_buf, g_padding);
        Sec_Eng_SHA_Start(shaId);

        while (read_len > 0) {
            if (read_len > BFLB_EFLASH_LOADER_READBUF_SIZE) {
                flash_read_xip(startaddr, (uint8_t *)g_sha_in_buf, BFLB_EFLASH_LOADER_READBUF_SIZE);
                /*cal sha here*/
                Sec_Eng_SHA256_Update(&sha_ctx, shaId, (uint8_t *)g_sha_in_buf, BFLB_EFLASH_LOADER_READBUF_SIZE);
                read_len -= BFLB_EFLASH_LOADER_READBUF_SIZE;
                startaddr += BFLB_EFLASH_LOADER_READBUF_SIZE;
            } else {
                flash_read_xip(startaddr, (uint8_t *)g_sha_in_buf, read_len);
                /*cal sha here*/
                Sec_Eng_SHA256_Update(&sha_ctx, shaId, (uint8_t *)g_sha_in_buf, read_len);
                read_len -= read_len;
                startaddr += read_len;
            }
        }

        Sec_Eng_SHA256_Finish(&sha_ctx, shaId, &ackdata[4]);

        for (sha_len = 0; sha_len < 32; sha_len++) {
            bflb_eflash_loader_printf("\r\n");
            bflb_eflash_loader_printx(ackdata[4 + sha_len]);
        }

        sha_len = 32;
        /*ack read data */
        ackdata[0] = BFLB_EFLASH_LOADER_CMD_ACK & 0xff;
        ackdata[1] = (BFLB_EFLASH_LOADER_CMD_ACK >> 8) & 0xff;
        ackdata[2] = sha_len & 0xff;
        ackdata[3] = (sha_len >> 8) & 0xff;
        bflb_eflash_loader_uart_send((uint32_t *)ackdata, sha_len + 4);
    }

    return ret;
}

static int32_t bflb_eflash_loader_cmd_write_flash_check(uint16_t cmd, uint8_t *data, uint16_t len)
{
    bflb_eflash_loader_printf("WC\n");

    bflb_eflash_loader_cmd_ack(g_eflash_loader_error);

    return BFLB_EFLASH_LOADER_SUCCESS;
}

static int32_t bflb_eflash_loader_cmd_set_flash_para(uint16_t cmd, uint8_t *data, uint16_t len)
{
    bflb_eflash_loader_cmd_ack(BFLB_EFLASH_LOADER_SUCCESS);
    return BFLB_EFLASH_LOADER_SUCCESS;
}

int32_t bflb_eflash_loader_cmd_process(uint8_t cmdid, uint8_t *data, uint16_t len)
{
    int i = 0;
    int32_t ret = BFLB_EFLASH_LOADER_SUCCESS;

    for (i = 0; i < sizeof(eflash_loader_cmds) / sizeof(eflash_loader_cmds[0]); i++) {
        if (eflash_loader_cmds[i].cmd == cmdid) {
            if (EFLASH_LOADER_CMD_ENABLE == eflash_loader_cmds[i].enabled && NULL != eflash_loader_cmds[i].cmd_process) {
                ret = eflash_loader_cmds[i].cmd_process(cmdid, data, len);
            } else {
                return BFLB_EFLASH_LOADER_CMD_ID_ERROR;
            }

            break;
        }
    }

    return ret;
}
