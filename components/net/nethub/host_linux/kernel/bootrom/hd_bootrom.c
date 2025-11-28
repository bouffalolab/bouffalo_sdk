/**
 * @file hd_bootrom.c
 * @brief Host Driver SDIO Bootrom Firmware Download Driver
 * @author Host Driver Development Team
 * @date 2024
 * @details This module implements the bootrom firmware download functionality
 *          for Host Driver wireless chipsets over SDIO interface. It handles
 *          secure firmware loading, signature verification, encryption support,
 *          and complete firmware execution management with comprehensive
 *          error handling and progress tracking.
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/firmware.h>

#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/host.h>

#include "hd_sdio_manage.h"
#include "hd_sdio2.h"
#include "hd_sdio3.h"
#include "hd_debugfs.h"
#include "hd_bootrom.h"

/**
 * @brief Clear bootrom command and acknowledgment buffers
 * @param[in] sdio_boot Pointer to SDIO bootrom structure
 * @details This function zeros out both command and acknowledgment buffers
 *          to ensure clean state before each bootrom communication operation.
 *          Provides null pointer safety check for robust operation.
 */
static void hd_bootrom_buff_clear(hd_sdio_bootrom_t *sdio_boot)
{
    if (!sdio_boot) {
        return;
    }

    memset(sdio_boot->sdio_cmd_buff, 0, HD_BOOTROM_SDIO_CMD_BUFF_MAX);
    memset(sdio_boot->sdio_ack_buff, 0, HD_BOOTROM_SDIO_ACK_BUFF_MAX);
}

/**
 * @brief Send bootrom command to SDIO device
 * @param[in] card Pointer to SDIO card structure
 * @param[in] cmd Pointer to bootrom command structure
 * @retval 0 Success
 * @retval <0 Error code from SDIO transmission
 * @details This function transmits bootrom commands to the device using
 *          SDIO download transfer with proper timeout handling and
 *          variable-length command payload support.
 */
static int hd_bootrom_send_cmd(struct hd_sdio_card *card, bootrom_cmd_t *cmd)
{
    int ret;

    struct sdio_trans_desc trans_desc = {
        .buff = (void *)cmd,
        .data_len = sizeof(bootrom_cmd_t) + cmd->len,
        .buff_len = HD_BOOTROM_SDIO_CMD_BUFF_MAX,
    };
    ret = card->sdio_dnld_transf(card, &trans_desc, HD_BOOTROM_SDIO_TRANS_TIMEOUT);

    return ret;
}

/**
 * @brief Receive bootrom acknowledgment from SDIO device
 * @param[in] card Pointer to SDIO card structure
 * @param[out] ack Pointer to bootrom acknowledgment structure
 * @retval 0 Success
 * @retval <0 Error code from SDIO reception
 * @details This function receives bootrom command acknowledgments from the
 *          device using SDIO upload transfer with proper timeout handling.
 *          The acknowledgment contains command execution status and results.
 */
static int hd_bootrom_receive_ack(struct hd_sdio_card *card, bootrom_ack_t *ack)
{
    int ret;

    struct sdio_trans_desc trans_desc = {
        .buff = (void *)ack,
        .data_len = 0,
        .buff_len = HD_BOOTROM_SDIO_ACK_BUFF_MAX,
    };
    ret = card->sdio_upld_transf(card, &trans_desc, HD_BOOTROM_SDIO_TRANS_TIMEOUT);

    return ret;
}

/**
 * @brief Get and parse firmware image from filesystem
 * @param[in] card Pointer to SDIO card structure
 * @param[out] sdio_boot Pointer to SDIO bootrom structure
 * @param[in] fw_name Firmware filename to load
 * @retval 0 Success
 * @retval <0 Error code from firmware loading
 * @details This function loads firmware from filesystem, validates the boot
 *          header structure, and extracts essential firmware metadata including
 *          signature type, encryption type, and segment information.
 */
static int hd_bootrom_get_firmware_image(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot, char *fw_name)
{
    int ret;
    const struct firmware *fw_helper = NULL;
    bootheader_t *boot_header;

    ret = request_firmware(&fw_helper, fw_name, &card->func->dev);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "request_firmware failed, error code %d", ret);
        return ret;
    }

    if (fw_helper->size < sizeof(bootheader_t)) {
        SDIO_DRV_ERR(card, "fw image size too small, size %d Byte", (int)fw_helper->size);
        return -1;
    }

    sdio_boot->hd_fw = fw_helper;
    sdio_boot->fw_curr_offset = 0;

    boot_header = (bootheader_t *)fw_helper->data;
    sdio_boot->fw_version = boot_header->version;
    sdio_boot->fw_sign_type = boot_header->basic_cfg.sign_type;
    sdio_boot->fw_encry_type = boot_header->basic_cfg.encrypt_type;
    sdio_boot->fw_segment_num = boot_header->basic_cfg.img_len_cnt;

    SDIO_DRV_INFO(card, "get firmware image OK, size: %d", (int)sdio_boot->hd_fw->size);
    SDIO_DRV_INFO(card, "    fw_version: 0x%08X, ", sdio_boot->fw_version);
    SDIO_DRV_INFO(card, "    fw_sign_type: 0x%02X, ", sdio_boot->fw_sign_type);
    SDIO_DRV_INFO(card, "    fw_encry_type: 0x%02X, ", sdio_boot->fw_encry_type);
    SDIO_DRV_INFO(card, "    fw_segment_num: %d, ", sdio_boot->fw_segment_num);

    return 0;
}

/**
 * @brief Get and parse boot information from device
 * @param[in] card Pointer to SDIO card structure
 * @param[out] sdio_boot Pointer to SDIO bootrom structure
 * @retval 0 Success
 * @retval <0 Error code from command execution
 * @details This function retrieves device boot information including firmware
 *          signature type, encryption type, and segment count. The information
 *          is validated and parsed for subsequent firmware download operations.
 */
static int hd_bootrom_get_boot_info(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot)
{
    int ret;
    bootrom_cmd_t *cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    bootrom_ack_t *ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;
    boot_info_data_t *boot_info;

    /* clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* cmd fill */
    cmd->cmd = HD_BOOTROM_CMD_BOOTINFO_GET;
    cmd->sum = 0;
    cmd->len = 0;
    /* send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "get_boot_info send cmd failed");
        return ret;
    }

    /* receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "get_boot_info receive ack failed");
        return ret;
    }

    /* bootinfo parse */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "get_boot_info ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    } else if (ack->len < sizeof(boot_info_data_t)) {
        SDIO_DRV_ERR(card, "get_boot_info ack len too small, %dbyte", ack->len);
        return -1;
    }
    boot_info = (boot_info_data_t *)ack->data;
    sdio_boot->chip_version = boot_info->version;
    sdio_boot->chip_sign_type = boot_info->sign_type[0];
    sdio_boot->chip_encry_type = boot_info->encry_type[0];
    sdio_boot->chip_id = boot_info->chip_id;

    SDIO_DRV_INFO(card, "get boot info OK, info data size: %d", ack->len);
    SDIO_DRV_INFO(card, "    chip_version: 0x%08X, ", sdio_boot->chip_version);
    SDIO_DRV_INFO(card, "    chip_sign_type: 0x%02X, ", sdio_boot->chip_sign_type);
    SDIO_DRV_INFO(card, "    chip_encry_type: 0x%02X, ", sdio_boot->chip_encry_type);
    SDIO_DRV_INFO(card, "    chip_id: 0x%016llX, ", sdio_boot->chip_id);

    if (sdio_boot->chip_sign_type != sdio_boot->fw_sign_type) {
        SDIO_DRV_ERR(card, "Sign mode of the firmware and chip is different!");
        return -1;
    }

    if (sdio_boot->chip_encry_type != sdio_boot->fw_encry_type) {
        SDIO_DRV_ERR(card, "Encrypt mode of the firmware and chip is different!");
        return -1;
    }

    return 0;
}

/**
 * @brief Download boot header to device
 * @param[in] card Pointer to SDIO card structure
 * @param[in] sdio_boot Pointer to SDIO bootrom structure
 * @retval 0 Success
 * @retval <0 Error code from download operation
 * @details This function downloads the firmware boot header to the device,
 *          validating size constraints and handling the command-acknowledgment
 *          protocol for secure bootrom communication.
 */
static int hd_bootrom_download_bootheader(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot)
{
    int ret;
    bootrom_cmd_t *cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    bootrom_ack_t *ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;

    /* check image size */
    if (sdio_boot->hd_fw->size - sdio_boot->fw_curr_offset < sizeof(bootheader_t)) {
        SDIO_DRV_ERR(card, "boot head size too small");
        return -1;
    }

    /* clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* cmd fill */
    cmd->cmd = HD_BOOTROM_CMD_BOOTHEADER_LOAD;
    cmd->sum = 0;
    cmd->len = sizeof(bootheader_t);
    memcpy(cmd->data, &(sdio_boot->hd_fw->data[sdio_boot->fw_curr_offset]), sizeof(bootheader_t));
    /* send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "boot_head send cmd failed");
        return ret;
    }

    /* receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "boot_head receive ack failed");
        return ret;
    }

    /* check ack */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "boot_head ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    }

    /* update offset */
    sdio_boot->fw_curr_offset += sizeof(bootheader_t);

    SDIO_DRV_INFO(card, "download boot_head OK, size: %d", (int)sizeof(bootheader_t));
    return 0;
}

/**
 * @brief Download SHA384 P2 signature data for sign type 2
 * @param[in] card Pointer to SDIO card structure
 * @param[in] sdio_boot Pointer to SDIO bootrom structure
 * @retval 0 Success
 * @retval <0 Error code from signature download
 * @details This function downloads SHA384 P2 signature data for firmware
 *          verification when both firmware and chip use signature type 2.
 *          The signature is essential for secure boot validation.
 */
static int hd_bootrom_download_sign_sha348_p2(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot)
{
    int ret;
    bootrom_cmd_t *cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    bootrom_ack_t *ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;

    if (sdio_boot->fw_sign_type != 2 || sdio_boot->chip_sign_type != 2) {
        SDIO_DRV_ERR(card, "No sign_sha348_p2 used");
    }

    /* Check image size */
    if (sdio_boot->hd_fw->size - sdio_boot->fw_curr_offset < 16) {
        SDIO_DRV_ERR(card, "sign_sha348_p2 size too small");
        return -1;
    }

    /* Clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* Command fill */
    cmd->cmd = HD_BOOTROM_CMD_LOAD_BOOTHEADER_SHA384_P2;
    cmd->sum = 0;
    cmd->len = 16;
    memcpy(cmd->data, &(sdio_boot->hd_fw->data[sdio_boot->fw_curr_offset]), 16);
    /* Send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sign_sha348_p2 send cmd failed");
        return ret;
    }

    /* Receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sign_sha348_p2 receive ack failed");
        return ret;
    }

    /* Check ack */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "sign_sha348_p2 ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    }

    /* Update offset */
    sdio_boot->fw_curr_offset += 16;

    SDIO_DRV_INFO(card, "download sign_sha348_p2 OK, size: %d", 16);
    return 0;
}

/**
 * @brief Download signature public key1 to device
 * @param[in] card Pointer to SDIO card structure
 * @param[in] sdio_boot Pointer to SDIO bootrom structure
 * @retval 0 Success
 * @retval <0 Error code from public key download
 * @details This function downloads the public key for firmware signature
 *          verification when signature verification is enabled. The key size
 *          varies based on signature type and is validated before transmission.
 */
static int hd_bootrom_download_sign_publickey1(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot)
{
    int ret;
    int sign_publickey_size;
    bootrom_cmd_t *cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    bootrom_ack_t *ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;

    if (!sdio_boot->fw_sign_type || !sdio_boot->chip_sign_type) {
        SDIO_DRV_ERR(card, "No signature used");
    }

    if (sdio_boot->fw_sign_type == 2) {
        sign_publickey_size = sizeof(sign384_public_key_t);
    } else {
        sign_publickey_size = sizeof(sign256_public_key_t);
    }

    /* check image size */
    if (sdio_boot->hd_fw->size - sdio_boot->fw_curr_offset < sign_publickey_size) {
        SDIO_DRV_ERR(card, "sign_public_key size too small");
        return -1;
    }

    /* clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* cmd fill */
    cmd->cmd = HD_BOOTROM_CMD_PK1_LOAD;
    cmd->sum = 0;
    cmd->len = sign_publickey_size;
    memcpy(cmd->data, &(sdio_boot->hd_fw->data[sdio_boot->fw_curr_offset]), sign_publickey_size);
    /* send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sign_public_key send cmd failed");
        return ret;
    }

    /* receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sign_public_key receive ack failed");
        return ret;
    }

    /* check ack */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "sign_public_key ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    }

    /* update offset */
    sdio_boot->fw_curr_offset += sign_publickey_size;

    SDIO_DRV_INFO(card, "download sign_public_key OK, size: %d", (int)sign_publickey_size);
    return 0;
}

/**
 * @brief Download signature data1 to device
 * @param[in] card Pointer to SDIO card structure
 * @param[in] sdio_boot Pointer to SDIO bootrom structure
 * @retval 0 Success
 * @retval <0 Error code from signature data download
 * @details This function downloads the signature data for firmware verification
 *          when signature verification is enabled. The signature data size is
 *          determined by the signature type and validated before transmission.
 */
static int hd_bootrom_download_sign_data1(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot)
{
    int ret;
    int sign_data1_size;
    bootrom_cmd_t *cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    bootrom_ack_t *ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;

    if (!sdio_boot->fw_sign_type || !sdio_boot->chip_sign_type) {
        SDIO_DRV_ERR(card, "No signature used");
    }

    if (sdio_boot->fw_sign_type == 2) {
        sign_data1_size = sizeof(sign384_data_t);
    } else {
        sign_data1_size = sizeof(sign256_data_t);
    }

    /* check image size */
    if (sdio_boot->hd_fw->size - sdio_boot->fw_curr_offset < sign_data1_size) {
        SDIO_DRV_ERR(card, "sign_data size too small");
        return -1;
    }

    /* clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* cmd fill */
    cmd->cmd = HD_BOOTROM_CMD_SIGNATURE1_LOAD;
    cmd->sum = 0;
    cmd->len = sign_data1_size;
    memcpy(cmd->data, &(sdio_boot->hd_fw->data[sdio_boot->fw_curr_offset]), sign_data1_size);
    /* send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sign_data send cmd failed");
        return ret;
    }

    /* receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "sign_data receive ack failed");
        return ret;
    }

    /* check ack */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "sign_data ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    }

    /* update offset */
    sdio_boot->fw_curr_offset += sign_data1_size;

    SDIO_DRV_INFO(card, "download sign_data OK, size: %d", (int)sign_data1_size);
    return 0;
}

/**
 * @brief Download encryption IV (Initialization Vector) to device
 * @param[in] card Pointer to SDIO card structure
 * @param[in] sdio_boot Pointer to SDIO bootrom structure
 * @retval 0 Success
 * @retval <0 Error code from IV download
 * @details This function downloads the encryption initialization vector when
 *          firmware encryption is enabled. The IV is required for decrypting
 *          encrypted firmware segments during the boot process.
 */
static int hd_bootrom_download_encry_iv(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot)
{
    int ret;
    bootrom_cmd_t *cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    bootrom_ack_t *ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;

    if (!sdio_boot->chip_encry_type || !sdio_boot->chip_encry_type) {
        SDIO_DRV_ERR(card, "No Encryption used");
    }

    /* check image size */
    if (sdio_boot->hd_fw->size - sdio_boot->fw_curr_offset < sizeof(encry_iv_t)) {
        SDIO_DRV_ERR(card, "encry_iv size too small");
        return -1;
    }

    /* clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* cmd fill */
    cmd->cmd = HD_BOOTROM_CMD_AESIV_LOAD;
    cmd->sum = 0;
    cmd->len = sizeof(encry_iv_t);
    memcpy(cmd->data, &(sdio_boot->hd_fw->data[sdio_boot->fw_curr_offset]), sizeof(encry_iv_t));
    /* send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "encry_iv send cmd failed");
        return ret;
    }

    /* receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "encry_iv receive ack failed");
        return ret;
    }

    /* check ack */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "encry_iv ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    }

    /* update offset */
    sdio_boot->fw_curr_offset += sizeof(encry_iv_t);

    SDIO_DRV_INFO(card, "download encry_iv OK, size: %d", (int)sizeof(encry_iv_t));
    return 0;
}

/**
 * @brief Download firmware segment header and data to device
 * @param[in] card Pointer to SDIO card structure
 * @param[in] sdio_boot Pointer to SDIO bootrom structure
 * @retval 0 Success
 * @retval <0 Error code from segment download
 * @details This function downloads firmware segments including header validation
 *          and chunked data transfer. Each segment is processed with proper
 *          acknowledgment handling and progress tracking for reliable transfer.
 */
static int hd_bootrom_download_segment(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot)
{
    int ret;
    int send_len, send_cnt;
    bootrom_cmd_t *cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    bootrom_ack_t *ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;
    segment_header_t segm_head_ack;

    /* check segment_header size */
    if (sdio_boot->hd_fw->size - sdio_boot->fw_curr_offset < sizeof(segment_header_t)) {
        SDIO_DRV_ERR(card, "segment_header size too small");
        return -1;
    }

    /* clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* segment header cmd fill */
    cmd->cmd = HD_BOOTROM_CMD_SECTIONHEADER_LOAD;
    cmd->sum = 0;
    cmd->len = sizeof(segment_header_t);
    memcpy(cmd->data, &(sdio_boot->hd_fw->data[sdio_boot->fw_curr_offset]), sizeof(segment_header_t));
    /* send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "segment_header send cmd failed");
        return ret;
    }

    /* segment header receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "segment_header receive ack failed");
        return ret;
    }
    /* segment header parse ack */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "segment_header ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    } else if (ack->len < sizeof(segment_header_t)) {
        SDIO_DRV_ERR(card, "segment_header ack len too small, %dbyte", ack->len);
        return -1;
    }
    segm_head_ack = *(segment_header_t *)ack->data;
    SDIO_DRV_INFO(card, "download segment_header OK, dest_addr: 0x%08X, size: %d", segm_head_ack.dest_addr, segm_head_ack.len);
    /* update offset */
    sdio_boot->fw_curr_offset += sizeof(segment_header_t);

    /* check segment_data size */
    if (sdio_boot->hd_fw->size - sdio_boot->fw_curr_offset < segm_head_ack.len) {
        SDIO_DRV_ERR(card, "segment_data size too small");
        return -1;
    }

    for (send_cnt = 0, send_len = 0; send_len < segm_head_ack.len; send_cnt++) {
        /* clear cmd/ack buff */
        hd_bootrom_buff_clear(sdio_boot);

        /* segment data cmd fill */
        cmd->cmd = HD_BOOTROM_CMD_SECTIONDATA_LOAD;
        cmd->sum = 0;
        if (segm_head_ack.len - send_len > HD_BOOTROM_CMD_PAYLOAD_MAX) {
            cmd->len = HD_BOOTROM_CMD_PAYLOAD_MAX;
        } else {
            cmd->len = segm_head_ack.len - send_len;
        }
        memcpy(cmd->data, &(sdio_boot->hd_fw->data[sdio_boot->fw_curr_offset]), cmd->len);
        /* send cmd */
        ret = hd_bootrom_send_cmd(card, cmd);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "segment_data piece[%d] send cmd failed", send_cnt);
            return ret;
        }

        /* segment data receive ack */
        ret = hd_bootrom_receive_ack(card, ack);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "segment_data piece[%d] receive ack failed", send_cnt);
            return ret;
        }
        /* check ack */
        if (ack->status != HD_BOOTOM_ACK_STA_OK) {
            SDIO_DRV_ERR(card, "segment_data piece[%d] ack status err: 0x%04X, len: 0x%04X", send_cnt, ack->status, ack->len);
            return -1;
        }

        /* update offset */
        sdio_boot->fw_curr_offset += cmd->len;
        send_len += cmd->len;

        SDIO_DRV_INFO(card, "download segment_data piece[%d] OK, size:%d, progress:%d/%d", send_cnt, cmd->len, send_len, segm_head_ack.len);
    }

    SDIO_DRV_INFO(card, "download segment_data OK, piece_cnt: %d, total_size: %d", send_cnt, send_len);
    return 0;
}

/**
 * @brief Check firmware image integrity on device
 * @param[in] card Pointer to SDIO card structure
 * @param[in] sdio_boot Pointer to SDIO bootrom structure
 * @retval 0 Success
 * @retval <0 Error code from image verification
 * @details This function triggers device-side firmware image integrity
 *          verification after complete download. The device validates
 *          checksums, signatures, and other security parameters.
 */
static int hd_bootrom_check_image(struct hd_sdio_card *card, hd_sdio_bootrom_t *sdio_boot)
{
    int ret;
    bootrom_cmd_t *cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    bootrom_ack_t *ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;

    /* clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* cmd fill */
    cmd->cmd = HD_BOOTROM_CMD_CHECK_IMAGE;
    cmd->sum = 0;
    cmd->len = 0;
    /* send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "check_image send cmd failed");
        return ret;
    }

    /* receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "check_image receive ack failed");
        return ret;
    }
    /* check ack */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "check_image ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    }

    SDIO_DRV_INFO(card, "check_image: OK");
    return 0;
}

/**
 * @brief Download complete firmware to device via SDIO
 * @param[in] card Pointer to SDIO card structure
 * @param[in] fw_name Firmware filename to download (NULL for default)
 * @retval 0 Success - firmware downloaded and verified successfully
 * @retval -ENOMEM Memory allocation failure
 * @retval <0 Other error codes from download process
 * @details This is the main function that orchestrates the entire firmware
 *          download process including boot info retrieval, header download,
 *          signature/encryption handling, segment transfer, and verification.
 *          It manages the complete secure boot sequence for the device.
 */
int hd_bootrom_download_firmware(struct hd_sdio_card *card, char *fw_name)
{
    int ret = 0;
    int segment_num;
    hd_sdio_bootrom_t *sdio_boot;

    sdio_boot = kzalloc(sizeof(hd_sdio_bootrom_t), GFP_KERNEL);
    if (!sdio_boot) {
        ret = -ENOMEM;
        goto exit;
    }

    if (!fw_name) {
        fw_name = HD_BOOTROM_FW_NAME;
    }

    /* 1. get firmware image */
    SDIO_DRV_INFO(card, "request_firmware %s", fw_name);
    ret = hd_bootrom_get_firmware_image(card, sdio_boot, fw_name);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "get firmware_image failed");
        goto exit;
    }

    /* 2. get bootrom info */
    SDIO_DRV_INFO(card, "get boot_info");
    ret = hd_bootrom_get_boot_info(card, sdio_boot);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "get boot_info failed");
        goto exit;
    }

    /* 3. download bootheader */
    SDIO_DRV_INFO(card, "download boot_head");
    ret = hd_bootrom_download_bootheader(card, sdio_boot);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "download boot_head failed");
        goto exit;
    }

    /* 4. download sign_public_key and sign_data */
    if (sdio_boot->chip_sign_type && sdio_boot->fw_sign_type) {
        /* sign384, load sha384 p2 */
        if (sdio_boot->chip_sign_type == 2) {
            ret = hd_bootrom_download_sign_sha348_p2(card, sdio_boot);
            if (ret < 0) {
                SDIO_DRV_ERR(card, "download sign_sha348_p2 failed");
                goto exit;
            }
        }

        /* download signature public key1 */
        SDIO_DRV_INFO(card, "download sign_publickey1");
        ret = hd_bootrom_download_sign_publickey1(card, sdio_boot);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "download sign_publickey1 failed");
            goto exit;
        }

        /* download signature data1 */
        SDIO_DRV_INFO(card, "download sign_data1");
        ret = hd_bootrom_download_sign_data1(card, sdio_boot);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "download sign_data1 failed");
            goto exit;
        }
    }

    /* 5. download encry_iv */
    if (sdio_boot->chip_encry_type && sdio_boot->fw_encry_type) {
        SDIO_DRV_INFO(card, "download encry_iv");
        ret = hd_bootrom_download_encry_iv(card, sdio_boot);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "download encry_iv failed");
            goto exit;
        }
    }

    /* 6. download segment header and data  */
    for (segment_num = 0; segment_num < sdio_boot->fw_segment_num; segment_num++) {
        SDIO_DRV_INFO(card, "download segment [%d]", segment_num);
        ret = hd_bootrom_download_segment(card, sdio_boot);
        if (ret < 0) {
            SDIO_DRV_ERR(card, "download segment [%d] failed", segment_num);
            goto exit;
        }
    }

    /* 7. get image check */
    SDIO_DRV_INFO(card, "image check");
    ret = hd_bootrom_check_image(card, sdio_boot);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "check_image failed");
        goto exit;
    }

    SDIO_DRV_INFO(card, "Firmware download all successful, fw data used: %d", sdio_boot->fw_curr_offset);
    ret = 0;

exit:
    if (sdio_boot->hd_fw) {
        release_firmware(sdio_boot->hd_fw);
    }
    if (sdio_boot) {
        kfree(sdio_boot);
    }
    if (ret < 0) {
        SDIO_DRV_ERR(card, "firmware download failed");
    }
    return ret;
}

/**
 * @brief Execute downloaded firmware on device
 * @param[in] card Pointer to SDIO card structure
 * @retval 0 Success - firmware execution started
 * @retval -ENOMEM Memory allocation failure
 * @retval <0 Other error codes from run command
 * @details This function sends the run command to start firmware execution
 *          after successful download and verification. It triggers the device
 *          to begin executing the downloaded firmware from bootrom state.
 */
int hd_bootrom_firmware_run(struct hd_sdio_card *card)
{
    int ret = 0;
    hd_sdio_bootrom_t *sdio_boot;
    bootrom_cmd_t *cmd;
    bootrom_ack_t *ack;

    sdio_boot = kzalloc(sizeof(hd_sdio_bootrom_t), GFP_KERNEL);
    if (!sdio_boot) {
        ret = -ENOMEM;
        goto exit;
    }

    cmd = (bootrom_cmd_t *)sdio_boot->sdio_cmd_buff;
    ack = (bootrom_ack_t *)sdio_boot->sdio_ack_buff;

    /* Clear cmd/ack buff */
    hd_bootrom_buff_clear(sdio_boot);

    /* Command fill */
    cmd->cmd = HD_BOOTROM_CMD_RUN;
    cmd->sum = 0;
    cmd->len = 0;
    /* Send cmd */
    ret = hd_bootrom_send_cmd(card, cmd);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "firmware_run send cmd failed");
        return ret;
    }

    /* Receive ack */
    ret = hd_bootrom_receive_ack(card, ack);
    if (ret < 0) {
        SDIO_DRV_ERR(card, "firmware_run receive ack failed");
        return ret;
    }
    /* Check ack */
    if (ack->status != HD_BOOTOM_ACK_STA_OK) {
        SDIO_DRV_ERR(card, "firmware_run ack status err: 0x%04X, len: 0x%04X", ack->status, ack->len);
        return -1;
    }

    SDIO_DRV_INFO(card, "firmware_run: OK");
    SDIO_DRV_INFO(card, "Good luck to you");
    ret = 0;

exit:
    if (sdio_boot) {
        kfree(sdio_boot);
    }
    if (ret < 0) {
        SDIO_DRV_ERR(card, "firmware run failed");
    }
    return ret;
}
