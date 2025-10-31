#include "sdh_spec.h"
#include "sdh_host.h"
#include "sdh_sd.h"

/* log */
#define DBG_TAG "SDH_SD"
#ifdef CONFIG_BSP_SDH_LOG_LEVEL
#define CONFIG_LOG_LEVEL CONFIG_BSP_SDH_LOG_LEVEL
#endif
#include "log.h"

/* conver wide_width_data to little endian: ACMD13(SD Status), ACMD51(SCR) */
int sdh_sd_wide_width_data_conver(uint32_t *data, uint32_t size)
{
    uint8_t temp;
    uint8_t *p1 = (void *)data;
    uint8_t *p2 = p1 + size - 1;

    while (p2 > p1) {
        temp = *p1;
        *p1 = *p2;
        *p2 = temp;
        p1++;
        p2--;
    }
    return 0;
}

int sdh_sd_go_idle(struct sd_card_s *sd_card, uint32_t arg)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_GO_IDLE_STATE;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = arg;
    cmd_cfg.resp_type = SDH_RESP_TPYE_NONE;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    return ret;
}

/* send cond (CMD8) */
int sdh_sd_send_cond(struct sd_card_s *sd_card)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SD_SEND_IF_COND;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = 0x0100 | 0xAA;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R7;

    for (int i = 0; i < 8; i++) {
        sdh_sd_go_idle(sd_card, 0);

        ret = sdh_host_transfer(sd_card->host, &transfer);
        if (ret < 0) {
            continue;
        }
        ret = sdh_host_wait_done(sd_card->host, &transfer);
        if (ret < 0) {
            continue;
        } else {
            break;
        }
    }

    if (ret < 0) {
        return ret;
    }

    if ((cmd_cfg.resp[0] & 0xff) != 0xAA) {
        ret = -1;
    }

    return ret;
}

/* get rca (CMD2) */
int sdh_sd_send_rca(struct sd_card_s *sd_card)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SET_RELATIVE_ADDR;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = 0;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R6;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    sd_card->sd_rca = cmd_cfg.resp[0] >> 16;

    return 0;
}

/* select card (CMD7) */
int sdh_sd_select_card(struct sd_card_s *sd_card, uint32_t rca)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SELECT_CARD;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    if (rca) {
        cmd_cfg.argument = rca << 16;
        cmd_cfg.resp_type = SDH_RESP_TPYE_R1b;
    } else {
        cmd_cfg.argument = 0;
        cmd_cfg.resp_type = SDH_RESP_TPYE_NONE;
    }

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    return 0;
}

/* send app cmd (CMD55) */
int sdh_sd_send_app_cmd(struct sd_card_s *sd_card, uint32_t rca)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_APP_CMD;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = rca << 16;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    if ((cmd_cfg.resp[0] & R1_APP_CMD) == 0) {
        return -2;
    }

    return 0;
}

/* send cond (ACMD41) */
int sdh_sd_send_app_cond(struct sd_card_s *sd_card)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    uint32_t acmd41_arg = 0;

    /* acmd41 arg cfg */
    acmd41_arg |= SD_OCR_VDD_32 | SD_OCR_VDD_33; /* OCR 3.3v */
#if SDH_HOST_SUP_SIGNAL_1V8
    acmd41_arg |= SD_OCR_FLAG_S18R; /* req 1.8V */
#endif
    acmd41_arg |= SD_OCR_FLAG_XPC; /* maximum performance (support speed class) */
    if (sd_card->card_flag & SDH_SD_FLAG_SDHC) {
        acmd41_arg |= SD_OCR_FLAG_HCS; /* support sdhc/sdxc */
    }

    /* wait init complete */
    for (int i = 0; i < 1000; i++) {
        /* send ACMD */
        ret = sdh_sd_send_app_cmd(sd_card, 0);
        if (ret < 0) {
            LOG_E("send app cmd failed\r\n");
            break;
        }

        cmd_cfg.index = CMD_SD_APP_OP_COND;
        cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
        cmd_cfg.argument = acmd41_arg;
        cmd_cfg.resp_type = SDH_RESP_TPYE_R3;

        ret = sdh_host_transfer(sd_card->host, &transfer);
        if (ret < 0) {
            break;
        }
        ret = sdh_host_wait_done(sd_card->host, &transfer);
        if (ret < 0) {
            LOG_E("send app cmd failed\r\n");
            break;
        }
        /* Wait until card exit busy state. */
        if (cmd_cfg.resp[0] & SD_OCR_FLAG_BUSY) {
            /* high capacity check */
            if (cmd_cfg.resp[0] & SD_OCR_FLAG_HCS) {
                sd_card->card_flag |= SDH_SD_FLAG_SUP_HIGH_CAPAC;
            }
            /* signal 1.8V support */
            if (cmd_cfg.resp[0] & SD_OCR_FLAG_S18R) {
                sd_card->card_flag |= SDH_SD_FLAG_SUP_SIGNAL_1V8;
            }
            /* read ocr */
            sd_card->ocr |= cmd_cfg.resp[0];
            break;
        }
        arch_delay_ms(1);
        ret = -1;
    }

    return ret;
}

/* set block size (CMD16) */
int sdh_sd_set_block_size(struct sd_card_s *sd_card, uint32_t block_size)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SET_BLOCKLEN;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = block_size;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    return 0;
}

/* get cid and decode (CMD2) */
int sdh_sd_all_send_cid(struct sd_card_s *sd_card)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_ALL_SEND_CID;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = 0;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R2;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    /* decode cid */
    uint32_t raw_cid[4];
    struct sd_cid_s *cid = &sd_card->sd_cid;
    memcpy((uint8_t *)raw_cid + 1, cmd_cfg.resp, 15);

    cid->mid = (raw_cid[3] >> 16) & 0xff;   /*!< Manufacturer ID [127:120] */
    cid->oid = (raw_cid[3] >> 8) & 0xffff;  /*!< OEM/Application ID [119:104] */
    cid->pnm[0] = (raw_cid[3] >> 0) & 0xff; /*!< Product name [103:64] */
    cid->pnm[1] = (raw_cid[2] >> 24) & 0xff;
    cid->pnm[2] = (raw_cid[2] >> 16) & 0xff;
    cid->pnm[3] = (raw_cid[2] >> 8) & 0xff;
    cid->pnm[4] = (raw_cid[2] >> 0) & 0xff;
    cid->pnm[5] = 0;
    cid->prv = (raw_cid[1] >> 24) & 0xff;      /*!< Product revision [63:56] */
    cid->psn = (raw_cid[1] << 8) & 0xffffff00; /*!< Product serial number [55:24] */
    cid->psn |= (raw_cid[0] >> 24) & 0xff;
    cid->mdt = (raw_cid[0] >> 8) & 0xfff; /*!< Manufacturing date [19:8] */

    /* cid dump */
    LOG_D("********* CID DUMP ********\r\n");
    LOG_D("CID MID: 0x%02X\r\n", cid->mid);
    LOG_D("CID OID: 0x%04X\r\n", cid->oid);
    LOG_D("CID PNM: \"%s\"\r\n", cid->pnm);
    LOG_D("CID PRV: 0x%02X\r\n", cid->prv);
    LOG_D("CID PSN: 0x%08X\r\n", cid->psn);
    LOG_D("CID MDT: 0x%08X\r\n", cid->mdt);
    LOG_D("********* DUMP END ********\r\n");

    return 0;
}

/* get csd and decode (CMD9) */
int sdh_sd_send_csd(struct sd_card_s *sd_card)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SEND_CSD;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = sd_card->sd_rca << 16;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R2;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    /* decode csd */
    uint32_t raw_csd[4];
    struct sd_csd_s *csd = &sd_card->sd_csd;
    memcpy((uint8_t *)raw_csd + 1, cmd_cfg.resp, 15);

    csd->csd_structure = (raw_csd[3] >> 30) & 0x03; /*!< CSD structure [127:126] */
    csd->taac = (raw_csd[3] >> 16) & 0xff;          /*!< Data read access-time-1 [119:112] */
    csd->nsac = (raw_csd[3] >> 8) & 0xff;           /*!< Data read access-time-2 in clock cycles (NSAC*100) [111:104] */
    csd->tran_speed = (raw_csd[3] >> 0) & 0xff;     /*!< Maximum data transfer rate [103:96] */

    csd->ccc = (raw_csd[2] >> 20) & 0x0fff;           /*!< Card command classes [95:84] */
    csd->read_bl_len = (raw_csd[2] >> 16) & 0x0f;     /*!< Maximum read data block length [83:80] */
    csd->read_bl_partial = (raw_csd[2] >> 15) & 0x01; /*!< Partial blocks for read allowed [79:79] */
    csd->wr_blk_misalign = (raw_csd[2] >> 14) & 0x01; /*!< Write block misalignment [78:78] */
    csd->rd_blk_misalign = (raw_csd[2] >> 13) & 0x01; /*!< Read block misalignment [77:77] */
    csd->dsr_imp = (raw_csd[2] >> 12) & 0x01;         /*!< DSR implemented [76:76] */

    if (csd->csd_structure == 0) {
        /* csd ver 1.0, SDSC */
        csd->c_size = (raw_csd[1] >> 30) & 0x03; /*!< Device size [73:62], 12-bit */
        csd->c_size |= (raw_csd[2] << 2) & 0x0FFC;

        csd->vdd_r_curr_min = (raw_csd[1] >> 27) & 0x07; /*!< Maximum read current at VDD min [61:59] */
        csd->vdd_r_curr_max = (raw_csd[1] >> 24) & 0x07; /*!< Maximum read current at VDD max [58:56] */
        csd->vdd_w_curr_min = (raw_csd[1] >> 21) & 0x07; /*!< Maximum write current at VDD min [55:53] */
        csd->vdd_w_curr_max = (raw_csd[1] >> 18) & 0x07; /*!< Maximum write current at VDD max [52:50] */
        csd->c_size_mult = (raw_csd[1] >> 15) & 0x07;    /*!< Device size multiplier [49:47] */

        /* Get card total block count and block size. */
        sd_card->block_count = (csd->c_size + 1) << (csd->c_size_mult + 2);
        sd_card->block_size = 1 << csd->read_bl_len;
        if (sd_card->block_size != SD_DEFAULT_BLOCK_SIZE) {
            sd_card->block_count = (uint64_t)sd_card->block_count * sd_card->block_size / SD_DEFAULT_BLOCK_SIZE;
            sd_card->block_size = SD_DEFAULT_BLOCK_SIZE;
        }

    } else if (csd->csd_structure == 1) {
        /* csd ver 2.0, SDHC or SDXC */
        csd->c_size = (raw_csd[2] << 16) & 0x3F0000; /*!< Device size [69:48], 22-bit */
        csd->c_size |= (raw_csd[1] >> 16) & 0xFFFF;
        if (csd->c_size < 0xFFFF) {
            /* SDHC */
            sd_card->card_flag |= SDH_SD_FLAG_SDHC;
            sd_card->card_flag &= ~SDH_SD_FLAG_SDXC;
        } else {
            /* SDXC */
            sd_card->card_flag &= ~SDH_SD_FLAG_SDHC;
            sd_card->card_flag |= SDH_SD_FLAG_SDXC;
        }
        sd_card->block_count = (csd->c_size + 1) * (512 * 1024 / SD_DEFAULT_BLOCK_SIZE);
        sd_card->block_size = SD_DEFAULT_BLOCK_SIZE;
    }

    csd->erase_blk_en = (raw_csd[1] >> 14) & 0x01;       /*!< Erase single block enabled [46:46] */
    csd->sector_size = (raw_csd[1] >> 7) & 0x7F;         /*!< Erase sector size [45:39] */
    csd->wp_grp_size = (raw_csd[1] >> 0) & 0x7F;         /*!< Write protect group size [38:32] */
    csd->wp_grp_enable = (raw_csd[0] >> 31) & 0x01;      /*!< Write protect group enabled [31:31] */
    csd->r2w_factor = (raw_csd[0] >> 26) & 0x07;         /*!< Write speed factor [28:26] */
    csd->write_bl_len = (raw_csd[0] >> 22) & 0x0F;       /*!< Maximum write data block length [25:22] */
    csd->write_bl_partial = (raw_csd[0] >> 21) & 0x01;   /*!< Partial blocks for write allowed [21:21] */
    csd->file_format_grp = (raw_csd[0] >> 15) & 0x01;    /*!< File format group [15:15] */
    csd->copy = (raw_csd[0] >> 14) & 0x01;               /*!< Copy flag [14:14] */
    csd->perm_write_protect = (raw_csd[0] >> 13) & 0x01; /*!< Permanent write protection [13:13] */
    csd->tmp_write_protect = (raw_csd[0] >> 12) & 0x01;  /*!< Temporary write protection [12:12] */
    csd->file_format = (raw_csd[0] >> 10) & 0x03;        /*!< File format [11:10] */

    /* csd dump */
    LOG_D("********* CSD DUMP ********\r\n");
    LOG_D("CSD STRUCT: v%d.0\r\n", csd->csd_structure + 1);
    LOG_D("CSD TAAC: %d\r\n", csd->taac);
    LOG_D("CSD NSAC: %d\r\n", csd->nsac);
    LOG_D("CSD CCC: 0x%04X\r\n", csd->ccc);
    LOG_D("CSD READ_BL_LEN: %d Byte\r\n", 1 << csd->read_bl_len);
    LOG_D("CSD READ_BL_PARTIAL: %s\r\n", csd->read_bl_partial ? "ture" : "false");
    LOG_D("CSD WRITE_BLK_MISALIGN: %s\r\n", csd->wr_blk_misalign ? "ture" : "false");
    LOG_D("CSD READ_BLK_MISALIGN: %s\r\n", csd->rd_blk_misalign ? "ture" : "false");
    LOG_D("CSD DSR_IMP: %s\r\n", csd->dsr_imp ? "ture" : "false");

    LOG_D("CSD C_SIZE: %d\r\n", csd->c_size);
    if (csd->csd_structure == 0) {
        LOG_D("CSD SD_TYPE: SDSC\r\n");
        LOG_D("CSD C_SIZE_MULT: %d\r\n", csd->c_size_mult);
    } else {
        LOG_D("CSD SD_TYPE: %s\r\n", (csd->c_size < 0xFFFF) ? "SDHC" : "SDXC");
    }
    LOG_D("********* DUMP END ********\r\n");

    return 0;
}

/* get csd and decode (ACMD51) */
int sdh_sd_send_scr(struct sd_card_s *sd_card)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct bflb_sdh_data_cfg_s data_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, &data_cfg };

    /* send ACMD */
    ret = sdh_sd_send_app_cmd(sd_card, sd_card->sd_rca);
    if (ret < 0) {
        LOG_E("send app cmd failed\r\n");
        return ret;
    }

    /* cmd cfg */
    cmd_cfg.index = CMD_SD_APP_SEND_SCR;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = 0;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;
    /* data cfg */
    struct bflb_sdh_data_tranfer_s data_tranfer = {
        .address = sd_card->host->internal_buff,
        .int_en = 0,
        .length = 8,
    };
    data_cfg.adma_tranfer = &data_tranfer;
    data_cfg.data_dir = SDH_TRANSFER_DIR_RD;
    data_cfg.adma_tranfer_cnt = 1;
    data_cfg.adma2_hw_desc_raw_mode = false;
    data_cfg.auto_cmd_mode = SDH_AUTO_CMD_DISABLE;
    data_cfg.block_size = 8;
    data_cfg.block_count = 1;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    /* decode scr */
    uint32_t raw_scr[2];
    struct sd_scr_s *scr = &sd_card->sd_scr;
    memcpy(raw_scr, sd_card->host->internal_buff, 8);
    sdh_sd_wide_width_data_conver(raw_scr, 8);

    scr->scr_structure = (raw_scr[1] >> 28) & 0x0F;         /*!< SCR Structure [63:60] */
    scr->sd_spec = (raw_scr[1] >> 24) & 0x0F;               /*!< SD memory card specification version [59:56] */
    scr->data_stat_after_erase = (raw_scr[1] >> 23) & 0x01; /*!< Data status after erases [55:55] */
    scr->sd_security = (raw_scr[1] >> 20) & 0x03;           /*!< Security specification supported [54:52] */
    scr->sd_bus_widths = (raw_scr[1] >> 16) & 0x0F;         /*!< Data bus widths supported [51:48] */
    scr->sd_spec3 = (raw_scr[1] >> 15) & 0x01;              /*!< Specification version 3.00 or higher [47:47]*/
    scr->cmd_support = (raw_scr[1] >> 0) & 0x3;             /*!< Command support bits [33:32] 33-support CMD23, 32-support cmd20*/

    /* check 4-bit bus */
    if (scr->sd_bus_widths & (1 << 2)) {
        sd_card->card_flag |= SDH_SD_FLAG_SUP_4BIT;
    }

    /* get specification version */
    if (scr->sd_spec == 0) {
        sd_card->card_spec_version = 100; /* version 1.00/1.01 */
    } else if (scr->sd_spec == 1) {
        sd_card->card_spec_version = 110; /* version 1.10 */
    } else if (scr->sd_spec == 2) {
        if (scr->sd_spec3 == 0) {
            sd_card->card_spec_version = 200; /* version 2.00 */
        } else {
            sd_card->card_spec_version = 300; /* version 3.00 */
        }
    }

    /* check support block count (cmd23) */
    if (scr->cmd_support & 0x02) {
        sd_card->card_flag |= SDH_SD_FLAG_SUP_CMD23;
    }
    /* check support speed class control (cmd20) */
    if (scr->cmd_support & 0x01) {
        sd_card->card_flag |= SDH_SD_FLAG_SUP_SPEED_CLASS;
    }

    /* scr dump */
    LOG_D("********* SCR DUMP ********\r\n");
    LOG_D("SCR STRUCT: v%d.0\r\n", scr->scr_structure + 1);
    LOG_D("SCR SD_SPEC: v%d.%02d\r\n", sd_card->card_spec_version / 100, sd_card->card_spec_version % 100);
    LOG_D("SCR SUP_4BIT: %s\r\n", (scr->sd_bus_widths & (1 << 2)) ? "ture" : "false");
    LOG_D("SCR SUP_CMD20: %s\r\n", (scr->cmd_support & 0x01) ? "ture" : "false");
    LOG_D("SCR SUP_CMD23: %s\r\n", (scr->cmd_support & 0x02) ? "ture" : "false");
    LOG_D("SCR ERASE_STA: %d\r\n", scr->data_stat_after_erase);
    LOG_D("********* DUMP END ********\r\n");

    return 0;
}

/* get R1 status (CMD13) */
int sdh_sd_send_status(struct sd_card_s *sd_card, uint32_t *r1_data)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    cmd_cfg.index = CMD_SEND_STATUS;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;
    cmd_cfg.argument = sd_card->sd_rca << 16;
    transfer.data_cfg = NULL;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    *r1_data = cmd_cfg.resp[0];

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    return 0;
}

/* get 512-bit status (ACMD13) */
int sdh_sd_read_status(struct sd_card_s *sd_card)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct bflb_sdh_data_cfg_s data_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, &data_cfg };

    /* send ACMD */
    ret = sdh_sd_send_app_cmd(sd_card, sd_card->sd_rca);
    if (ret < 0) {
        LOG_E("send app cmd failed\r\n");
        return ret;
    }

    /* cmd cfg */
    cmd_cfg.index = CMD_SD_APP_SD_STATUS;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = 0;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;
    /* data cfg */
    struct bflb_sdh_data_tranfer_s data_tranfer = {
        .address = sd_card->host->internal_buff,
        .int_en = 0,
        .length = 64,
    };
    data_cfg.adma_tranfer = &data_tranfer;
    data_cfg.data_dir = SDH_TRANSFER_DIR_RD;
    data_cfg.adma_tranfer_cnt = 1;
    data_cfg.adma2_hw_desc_raw_mode = false;
    data_cfg.auto_cmd_mode = SDH_AUTO_CMD_DISABLE;
    data_cfg.block_size = 64;
    data_cfg.block_count = 1;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    /* decode sd_status */
    uint32_t raw_sd_status[16];
    struct sd_status_s *status = &sd_card->sd_status;
    memcpy(raw_sd_status, sd_card->host->internal_buff, 64);
    sdh_sd_wide_width_data_conver(raw_sd_status, 64);

    status->dat_bus_width = (raw_sd_status[15] >> 30) & 0x03;    /*!< current buswidth */
    status->secure_mode = (raw_sd_status[15] >> 29) & 0x01;      /*!< secured mode */
    status->sd_card_type = (raw_sd_status[15] >> 0) & 0xFFFF;    /*!< sdcard type */
    status->size_of_protected_area = raw_sd_status[14];          /*!< size of protected area */
    status->speed_class = (raw_sd_status[13] >> 24) & 0xFF;      /*!< speed class of card */
    status->performance_move = (raw_sd_status[13] >> 16) & 0xFF; /*!< Performance of move indicated by 1[MB/S]step */
    status->au_size = (raw_sd_status[13] >> 12) & 0x0F;          /*!< size of AU */

    status->erase_size = (raw_sd_status[13] << 8) & 0xFF00; /*!< number of AUs to be erased at a time */
    status->erase_size |= (raw_sd_status[12] >> 24) & 0xFF;

    status->erase_timeout = (raw_sd_status[12] >> 18) & 0x3F; /*!< timeout value for erasing areas specified by UNIT OF ERASE AU */
    status->erase_offset = (raw_sd_status[12] >> 16) & 0x03;  /*!< fixed offset value added to erase time */

    /* status dump */
    LOG_D("********* STATUS DUMP ********\r\n");
    LOG_D("STATUS BUS_WIDTH: %d bit\r\n", 1 << status->dat_bus_width);
    LOG_D("STATUS SECURE_MODE: %s\r\n", (status->secure_mode) ? "ture" : "false");
    LOG_D("STATUS CARD_TYPE: %d\r\n", status->sd_card_type);
    LOG_D("STATUS PROTECT_SIZE: %d\r\n", status->size_of_protected_area);
    LOG_D("STATUS SPEED_CLASS: Class %d\r\n", (status->speed_class * 2) > 6 ? (status->speed_class * 2 + 2) : (status->speed_class * 2));
    LOG_D("STATUS PERFORMANCE: %d MB/s\r\n", status->performance_move);
    LOG_D("STATUS AU_SIZE: %d\r\n", status->au_size);
    LOG_D("STATUS ERASE_SIZE: %d\r\n", status->erase_size);
    LOG_D("STATUS ERASE_TIMEOUT: %d\r\n", status->erase_timeout);
    LOG_D("STATUS ERASE_OFFSET: %d\r\n", status->erase_offset);
    LOG_D("********** DUMP END *********\r\n");

    return 0;
}

/* set bus width (ACMD6) */
int sdh_sd_set_bus_width(struct sd_card_s *sd_card, uint8_t bus_width)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    if (bus_width == 1) {
        bus_width = 0x00;
    } else if (bus_width == 4) {
        bus_width = 0x02;
    } else {
        return -1;
    }

    /* send ACMD */
    ret = sdh_sd_send_app_cmd(sd_card, sd_card->sd_rca);
    if (ret < 0) {
        LOG_E("send app cmd failed\r\n");
        return ret;
    }

    cmd_cfg.index = CMD_SD_APP_SET_BUS_WIDTH;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = bus_width;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    /* sdh set bus width */
    ret = sdh_set_bus_width(sd_card->host, 4);

    return ret;
}

/* sd switch (CMD6) */
int sdh_sd_switch_func(struct sd_card_s *sd_card, bool set_mode, uint32_t func_bit, void *status)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct bflb_sdh_data_cfg_s data_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, &data_cfg };

    /* cmd cfg */
    cmd_cfg.index = CMD_SD_SWITCH;
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.argument = func_bit | (set_mode << 31);
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;
    /* data cfg */
    struct bflb_sdh_data_tranfer_s data_tranfer = {
        .address = sd_card->host->internal_buff,
        .int_en = 0,
        .length = 64,
    };
    data_cfg.adma_tranfer = &data_tranfer;
    data_cfg.data_dir = SDH_TRANSFER_DIR_RD;
    data_cfg.adma_tranfer_cnt = 1;
    data_cfg.adma2_hw_desc_raw_mode = false;
    data_cfg.auto_cmd_mode = SDH_AUTO_CMD_DISABLE;
    data_cfg.block_size = 64;
    data_cfg.block_count = 1;

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        return ret;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        return -1;
    }

    memcpy(status, sd_card->host->internal_buff, 64);
    sdh_sd_wide_width_data_conver(status, 64);

    return 0;
}

int sdh_sd_select_func(struct sd_card_s *sd_card, uint8_t func_group, uint8_t func)
{
    int ret;
    uint32_t func_bit;
    uint32_t status[16];
    uint16_t func_support_bit[6];
    uint8_t curr_func_bit;

    if (sd_card->card_spec_version < 110) {
        /* not support */
        LOG_E("not support cmd6\r\n");
        return -1;
    }

    func_bit = 0x00FFFFFF;
    func_bit &= ~(0xf << (func_group * 4));
    func_bit |= func << (func_group * 4);

    /* check switch_func */
    ret = sdh_sd_switch_func(sd_card, 0, func_bit, status);
    if (ret < 0) {
        return ret;
    }

    curr_func_bit = (status[11] >> 24) & 0xFF;
    curr_func_bit |= (status[12] << 8) & 0xFFFF00;

    func_support_bit[0] = (status[12] >> 16) & 0xFFFF; /* bit400~bit415 */
    func_support_bit[1] = (status[13] >> 0) & 0xFFFF;  /* bit416~bit431 */
    func_support_bit[2] = (status[13] >> 16) & 0xFFFF; /* bit432~bit447 */
    func_support_bit[3] = (status[14] >> 0) & 0xFFFF;  /* bit448~bit463 */
    func_support_bit[4] = (status[14] >> 16) & 0xFFFF; /* bit464~bit479 */
    func_support_bit[5] = (status[15] >> 0) & 0xFFFF;  /* bit480~bit495 */

    /* check if function is support */
    if ((func_support_bit[func_group] & (1 << func)) == 0) {
        LOG_E("switch_func not support: group:%d, func:%d\r\n", func_group, func);
        return -1;
    }
    if ((curr_func_bit >> (func_group * 4) & 0x0F) != func) {
        LOG_E("switch_func check error: group:%d, func:%d\r\n", func_group, func);
        return -1;
    }
    LOG_D("switch_func check success: group:%d, func:%d\r\n", func_group, func);

    /* set switch_func */
    ret = sdh_sd_switch_func(sd_card, 1, func_bit, status);
    if (ret < 0) {
        return ret;
    }

    curr_func_bit = (status[11] >> 24) & 0xFF;
    curr_func_bit |= (status[12] << 8) & 0xFFFF00;

    if ((curr_func_bit >> (func_group * 4) & 0x0F) != func) {
        LOG_E("switch_func set error: group:%d, func:%d\r\n", func_group, func);
        return -1;
    }
    LOG_D("switch_func set success: group:%d, func:%d\r\n", func_group, func);

    return 0;
}

int sdh_sd_deinit(struct sd_card_s *sd_card)
{
    sdh_host_deinit(sd_card->host);
    memset(sd_card->host, 0, sizeof(struct sdh_host_s));
    memset(sd_card, 0, sizeof(struct sd_card_s));

    return 0;
}

int sdh_sd_card_init(struct sd_card_s *sd_card, struct sdh_host_s *sdh_host)
{
    int ret;

    memset(sd_card, 0, sizeof(struct sd_card_s));
    memset(sdh_host, 0, sizeof(struct sdh_host_s));

    sdh_host_init(sdh_host);
    sd_card->host = sdh_host;

    sdh_set_bus_width(sd_card->host, 1);
    sdh_set_bus_clock(sd_card->host, 400 * 1000, &sd_card->bus_clock_hz);
    sdh_set_data_timeout_ms(sd_card->host, SD_DATA_TIMEOUT_MS, NULL);

    /* send card active clock (80 clk) */
    sdh_send_active_clk(sd_card->host);

    sdh_sd_go_idle(sd_card, 0);

    ret = sdh_sd_send_cond(sd_card);
    if (ret >= 0) {
        /* v2.00 or laster */
        LOG_D("send_cond success\r\n");
        sd_card->card_flag |= SDH_SD_FLAG_SDHC;
    } else {
        /* v1.xx */
        LOG_I("send_cond failed\r\n");
        sdh_sd_go_idle(sd_card, 0);
    }

    ret = sdh_sd_send_app_cond(sd_card);
    if (ret < 0) {
        LOG_E("send_app_cond failed\r\n");
        goto init_exit;
    }
    LOG_D("send_app_cond success\r\n");

#if SDH_HOST_SUP_SIGNAL_1V8
    /* TODO: switch 1.8v signal (CMD11) */
#endif

    /* get cid */
    ret = sdh_sd_all_send_cid(sd_card);
    if (ret < 0) {
        LOG_E("all_send_cid failed\r\n");
        goto init_exit;
    }
    LOG_D("all_send_cid success\r\n");

    /* get rca */
    ret = sdh_sd_send_rca(sd_card);
    if (ret < 0) {
        LOG_E("send_rca failed\r\n");
        goto init_exit;
    }
    LOG_D("send_rca success, rca:0x%04X\r\n", sd_card->sd_rca);

    /* get csd */
    ret = sdh_sd_send_csd(sd_card);
    if (ret < 0) {
        LOG_E("send_csd failed\r\n");
        goto init_exit;
    }
    LOG_D("send_csd success\r\n");

    /* select card */
    ret = sdh_sd_select_card(sd_card, sd_card->sd_rca);
    if (ret < 0) {
        LOG_E("select_card failed\r\n");
        goto init_exit;
    }
    LOG_D("select_card success\r\n");

    /* default speed mode */
    sdh_set_bus_clock(sd_card->host, 25000000, &sd_card->bus_clock_hz);
    sdh_set_data_timeout_ms(sd_card->host, SD_DATA_TIMEOUT_MS, NULL);

    /* get scr */
    ret = sdh_sd_send_scr(sd_card);
    if (ret < 0) {
        LOG_E("send_scr failed\r\n");
        goto init_exit;
    }
    LOG_D("send_scr success\r\n");

    /* 4-bit bus mode */
#if SDH_HOST_SUP_BUS_WIDTH_4
    if (sd_card->card_flag & SDH_SD_FLAG_SUP_4BIT) {
        sdh_sd_set_bus_width(sd_card, 4);
    }
#endif

    /* get card current status */
    ret = sdh_sd_read_status(sd_card);
    if (ret < 0) {
        LOG_E("read_status failed\r\n");
        goto init_exit;
    }
    LOG_D("read_status success\r\n");

    /* set block size */
    ret = sdh_sd_set_block_size(sd_card, SD_DEFAULT_BLOCK_SIZE);
    if (ret < 0) {
        LOG_E("set_block_size failed\r\n");
        goto init_exit;
    }
    LOG_D("set_block_size %d success\r\n", SD_DEFAULT_BLOCK_SIZE);

    /* select bus speed */
#if SDH_HOST_SUP_SIGNAL_1V8 && SDH_HOST_SUP_UHS_I

    if (sd_card->card_flag & SDH_SD_FLAG_SUP_SIGNAL_1V8) {
        /* set SDR50/SDR104 and timing */
    } else
#endif
        if (sd_card->card_spec_version >= 110) {
        /* set SDR25 */
        ret = sdh_sd_select_func(sd_card, SD_SWITCH_FUNC_GRUP_ACCESS_MODE, SD_SWITCH_FUNC_ACCESS_MODE_SDR25);
        if (ret < 0) {
            LOG_E("set SDR25 failed\r\n");
        } else {
            sdh_set_bus_clock(sd_card->host, 50000000, &sd_card->bus_clock_hz);
            sdh_set_data_timeout_ms(sd_card->host, SD_DATA_TIMEOUT_MS, NULL);
        }
    }

init_exit:
    if (ret < 0) {
        LOG_E("sd_card init failed\r\n");
        return ret;
    }

    LOG_I("card_init success! Block_size %dByte, Block_cnt %d, Total size %lld MByte\r\n",
          sd_card->block_size, sd_card->block_count, (int64_t)sd_card->block_size * sd_card->block_count / 1024 / 1024);

    return 0;
}

int sdh_sd_read_blocks(struct sd_card_s *sd_card, void *rd_buff, uint32_t blk_addr, uint32_t blk_cnt)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct bflb_sdh_data_cfg_s data_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, &data_cfg };

    LOG_D("read block start: buff:%p, blk_addr:%u, blk_cnt:%u\r\n", rd_buff, blk_addr, blk_cnt);

    /* cmd cfg */
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;
    /* block address */
    if (sd_card->card_flag & SDH_SD_FLAG_SUP_HIGH_CAPAC) {
        cmd_cfg.argument = blk_addr;
    } else {
        cmd_cfg.argument = blk_addr * SD_DEFAULT_BLOCK_SIZE;
    }
    /* single/multi block */
    if (blk_cnt == 1) {
        cmd_cfg.index = CMD_READ_SINGLE_BLOCK;
    } else {
        cmd_cfg.index = CMD_READ_MULTIPLE_BLOCK;
    }

    /* data cfg */
    struct bflb_sdh_data_tranfer_s data_tranfer = {
        .address = rd_buff,
        .int_en = 0,
        .length = blk_cnt * SD_DEFAULT_BLOCK_SIZE,
    };
    data_cfg.adma_tranfer = &data_tranfer;
    data_cfg.adma_tranfer_cnt = 1;
    data_cfg.adma2_hw_desc_raw_mode = false;
    data_cfg.data_dir = SDH_TRANSFER_DIR_RD;
    data_cfg.block_size = SD_DEFAULT_BLOCK_SIZE;
    data_cfg.block_count = blk_cnt;
    /* auto cmd */
    if (blk_cnt > 1 && (sd_card->card_flag & SDH_SD_FLAG_SUP_CMD23)) {
        data_cfg.auto_cmd_mode = SDH_AUTO_CMD_CMD23;
    } else if (blk_cnt > 1) {
        data_cfg.auto_cmd_mode = SDH_AUTO_CMD_CMD12;
    } else {
        data_cfg.auto_cmd_mode = SDH_AUTO_CMD_DISABLE;
    }

    bflb_l1c_dcache_clean_range(rd_buff, 1);
    bflb_l1c_dcache_clean_range(((uint8_t *)rd_buff + SD_DEFAULT_BLOCK_SIZE * blk_cnt), 1);
    bflb_l1c_dcache_invalidate_range((uint8_t *)rd_buff, SD_DEFAULT_BLOCK_SIZE * blk_cnt);

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        goto read_exit;
    }

    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        goto read_exit;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        ret = -1;
        goto read_exit;
    }

read_exit:
    if (ret < 0) {
        LOG_E("read block failed: buff:%p, blk_addr:%u, blk_cnt:%u\r\n", rd_buff, blk_addr, blk_cnt);
        return ret;
    }

    bflb_l1c_dcache_invalidate_range(rd_buff, SD_DEFAULT_BLOCK_SIZE * blk_cnt);

    LOG_D("read block success: buff:%p, blk_addr:%u, blk_cnt:%u\r\n", rd_buff, blk_addr, blk_cnt);
    return ret;
}

int sdh_sd_write_blocks(struct sd_card_s *sd_card, void *wr_buff, uint32_t blk_addr, uint32_t blk_cnt)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct bflb_sdh_data_cfg_s data_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, &data_cfg };

    LOG_D("write block start: buff:%p, blk_addr:%u, blk_cnt:%u\r\n", wr_buff, blk_addr, blk_cnt);

    /* cmd cfg */
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;
    /* block address */
    if (sd_card->card_flag & SDH_SD_FLAG_SUP_HIGH_CAPAC) {
        cmd_cfg.argument = blk_addr;
    } else {
        cmd_cfg.argument = blk_addr * SD_DEFAULT_BLOCK_SIZE;
    }
    /* single/multi block */
    if (blk_cnt == 1) {
        cmd_cfg.index = CMD_WRITE_BLOCK;
    } else {
        cmd_cfg.index = CMD_WRITE_MULTIPLE_BLOCK;
    }

    /* data cfg */
    struct bflb_sdh_data_tranfer_s data_tranfer = {
        .address = wr_buff,
        .int_en = 0,
        .length = blk_cnt * SD_DEFAULT_BLOCK_SIZE,
    };
    data_cfg.adma_tranfer = &data_tranfer;
    data_cfg.adma_tranfer_cnt = 1;
    data_cfg.adma2_hw_desc_raw_mode = false;
    data_cfg.data_dir = SDH_TRANSFER_DIR_WR;
    data_cfg.block_size = SD_DEFAULT_BLOCK_SIZE;
    data_cfg.block_count = blk_cnt;
    /* auto cmd */
    if (blk_cnt > 1 && (sd_card->card_flag & SDH_SD_FLAG_SUP_CMD23)) {
        data_cfg.auto_cmd_mode = SDH_AUTO_CMD_CMD23;
    } else if (blk_cnt > 1) {
        data_cfg.auto_cmd_mode = SDH_AUTO_CMD_CMD12;
    } else {
        data_cfg.auto_cmd_mode = SDH_AUTO_CMD_DISABLE;
    }

    bflb_l1c_dcache_clean_range(wr_buff, SD_DEFAULT_BLOCK_SIZE * blk_cnt);

    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        goto write_exit;
    }

    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        goto write_exit;
    }

    if (R1_ERR_STATUS(cmd_cfg.resp[0])) {
        ret = -1;
        goto write_exit;
    }

    /* send cmd13 to check done */
    uint32_t r1_status = 0;
    do {
        ret = sdh_sd_send_status(sd_card, &r1_status);
        if (ret < 0) {
            goto write_exit;
        }
    } while (R1_CURRENT_STATE(r1_status) == R1_STATE_RECE_PROGRAM);

write_exit:
    if (ret < 0) {
        LOG_E("write block failed: buff:%p, blk_addr:%u, blk_cnt:%u\r\n", wr_buff, blk_addr, blk_cnt);
        return ret;
    }

    LOG_D("write block success: buff:%p, blk_addr:%u, blk_cnt:%u\r\n", wr_buff, blk_addr, blk_cnt);
    return ret;
}

int sdh_sd_erase_blocks(struct sd_card_s *sd_card, uint32_t blk_addr, uint32_t blk_cnt)
{
    int ret;
    struct bflb_sdh_cmd_cfg_s cmd_cfg = { 0 };
    struct sdh_host_transfer_s transfer = { &cmd_cfg, NULL };

    LOG_D("erase block start: blk_addr:%u, blk_cnt:%u\r\n", blk_addr, blk_cnt);

    /* start address */
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;
    cmd_cfg.index = CMD_SD_ERASE_WR_BLK_START;
    /* block address */
    if (sd_card->card_flag & SDH_SD_FLAG_SUP_HIGH_CAPAC) {
        cmd_cfg.argument = blk_addr;
    } else {
        cmd_cfg.argument = blk_addr * SD_DEFAULT_BLOCK_SIZE;
    }
    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        goto erase_exit;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        goto erase_exit;
    }

    /* end address */
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1;
    cmd_cfg.index = CMD_SD_ERASE_WR_BLK_END;
    /* block address */
    if (sd_card->card_flag & SDH_SD_FLAG_SUP_HIGH_CAPAC) {
        cmd_cfg.argument = blk_addr + blk_cnt - 1;
    } else {
        cmd_cfg.argument = (blk_addr + blk_cnt - 1) * SD_DEFAULT_BLOCK_SIZE;
    }
    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        goto erase_exit;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        goto erase_exit;
    }

    /* erase */
    cmd_cfg.cmd_type = SDH_CMD_TPYE_NORMAL;
    cmd_cfg.resp_type = SDH_RESP_TPYE_R1b;
    cmd_cfg.index = CMD_SD_ERASE;
    cmd_cfg.argument = 0;
    ret = sdh_host_transfer(sd_card->host, &transfer);
    if (ret < 0) {
        goto erase_exit;
    }
    ret = sdh_host_wait_done(sd_card->host, &transfer);
    if (ret < 0) {
        goto erase_exit;
    }

erase_exit:
    if (ret < 0) {
        LOG_W("erase block failed: blk_addr:%u, blk_cnt:%u\r\n", blk_addr, blk_cnt);
        return ret;
    }

    LOG_D("erase block success: blk_addr:%u, blk_cnt:%u\r\n", blk_addr, blk_cnt);
    return ret;
}
