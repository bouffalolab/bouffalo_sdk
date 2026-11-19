/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include <stdlib.h>
#include <string.h>
#include "bflb_i2c.h"
#include "xcodec_dev_defaults.h"

typedef struct {
    xcodec_ctrl_if_t base;
    xcodec_i2c_ctrl_cfg_t cfg;
    bool opened;
} i2c_ctrl_t;

static int ctrl_open(const xcodec_ctrl_if_t *h, void *cfg, int cfg_size)
{
    if (!h || !cfg || cfg_size != sizeof(xcodec_i2c_ctrl_cfg_t)) {
        return XCODEC_DEV_INVALID_ARG;
    }
    i2c_ctrl_t *ctrl = (i2c_ctrl_t *)h;
    xcodec_i2c_ctrl_cfg_t *config = cfg;
    if (!config->i2c || config->addr > 0x7f) {
        return XCODEC_DEV_INVALID_ARG;
    }
    ctrl->cfg = *config;
    ctrl->opened = true;
    return XCODEC_DEV_OK;
}

static bool ctrl_is_open(const xcodec_ctrl_if_t *h)
{
    return h && ((const i2c_ctrl_t *)h)->opened;
}

static int validate(const xcodec_ctrl_if_t *h, int reg, int reg_len, void *data, int len)
{
    if (!h || !data || reg < 0 || reg_len < 1 || reg_len > 4 || len < 1 ||
        len > UINT16_MAX - reg_len || (reg_len < 4 && (unsigned)reg >> (reg_len * 8))) {
        return XCODEC_DEV_INVALID_ARG;
    }
    return ctrl_is_open(h) ? XCODEC_DEV_OK : XCODEC_DEV_WRONG_STATE;
}

static void encode_reg(uint8_t *bytes, int reg, int reg_len)
{
    for (int i = 0; i < reg_len; i++) {
        bytes[i] = (unsigned)reg >> (8 * (reg_len - i - 1));
    }
}

static int ctrl_read(const xcodec_ctrl_if_t *h, int reg, int reg_len, void *data, int len)
{
    int ret = validate(h, reg, reg_len, data, len);
    if (ret) {
        return ret;
    }
    const i2c_ctrl_t *ctrl = (const i2c_ctrl_t *)h;
    uint8_t address[4];
    encode_reg(address, reg, reg_len);
    struct bflb_i2c_msg_s msgs[2] = {
        {.addr = ctrl->cfg.addr, .flags = I2C_M_NOSTOP, .buffer = address, .length = reg_len},
        {.addr = ctrl->cfg.addr, .flags = I2C_M_READ, .buffer = data, .length = len},
    };
    return bflb_i2c_transfer(ctrl->cfg.i2c, msgs, 2) ? XCODEC_DEV_READ_FAIL : XCODEC_DEV_OK;
}

static int ctrl_write(const xcodec_ctrl_if_t *h, int reg, int reg_len, void *data, int len)
{
    int ret = validate(h, reg, reg_len, data, len);
    if (ret) {
        return ret;
    }
    const i2c_ctrl_t *ctrl = (const i2c_ctrl_t *)h;
    uint8_t address[4];
    encode_reg(address, reg, reg_len);
    struct bflb_i2c_msg_s msgs[2] = {
        {.addr = ctrl->cfg.addr, .flags = I2C_M_NOSTOP, .buffer = address, .length = reg_len},
        {.addr = ctrl->cfg.addr, .flags = I2C_M_WRITE, .buffer = data, .length = len},
    };
    ret = bflb_i2c_transfer(ctrl->cfg.i2c, msgs, 2);
    return ret ? XCODEC_DEV_WRITE_FAIL : XCODEC_DEV_OK;
}

static int ctrl_close(const xcodec_ctrl_if_t *h)
{
    if (!h) {
        return XCODEC_DEV_INVALID_ARG;
    }
    ((i2c_ctrl_t *)h)->opened = false;
    return XCODEC_DEV_OK;
}

const xcodec_ctrl_if_t *xcodec_i2c_ctrl_new(const xcodec_i2c_ctrl_cfg_t *cfg)
{
    i2c_ctrl_t *ctrl = calloc(1, sizeof(*ctrl));
    if (!ctrl) {
        return NULL;
    }
    ctrl->base = (xcodec_ctrl_if_t){
        .open = ctrl_open, .is_open = ctrl_is_open, .read_reg = ctrl_read,
        .write_reg = ctrl_write, .close = ctrl_close,
    };
    if (ctrl_open(&ctrl->base, (void *)cfg, sizeof(*cfg))) {
        free(ctrl);
        return NULL;
    }
    return &ctrl->base;
}
