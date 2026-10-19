/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <xutils/types.h>
#include <xutils/debug.h>
#include <xutils/list.h>
#include <xutils/device.h>
#include <xutils/driver.h>
// #include <xutils/xringbuffer.h>
#include <msp/kernel.h>

#include <alsa/snd.h>
#include <alsa/pcm.h>

#define TAG "card"

// int snd_card_new(struct device *parent)
int snd_card_new(int *parent, int idx, const char *xid, struct module *module, int size, snd_card_drv_t **card_ret)
{

    return 0;
}

int snd_card_free(snd_card_drv_t *card)
{

    return 0;
}

int msp_card_attach(const char *name, card_dev_t **card)
{
    *card = (card_dev_t *)msp_device_open(name);
    return *card == NULL? 0 : -1;
}

int msp_card_lpm(const char *name ,int state)
{
    char *dev_name  = strdup(name);
    int len         = strlen(name);
    int id          = dev_name[len-1] - 0x30;
    dev_name[len-1] = 0;

    card_dev_t *card = (card_dev_t *)msp_device_find(dev_name, id);

    msp_device_lpm((msp_dev_t*)card, state);

    msp_free(dev_name);

    return 0;
}

