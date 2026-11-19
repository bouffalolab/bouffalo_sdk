/* Copyright (C) 2026 Bouffalolab Group Holding Limited */
#include <stdlib.h>
#include "xcodec_dev.h"
#include "xcodec_ctrl_if.h"

int xcodec_delete_data_if(const xcodec_data_if_t *interface)
{
    if (!interface) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = interface->close ? interface->close(interface) : XCODEC_DEV_OK;
    if (!ret) {
        free((void *)interface);
    }
    return ret;
}

int xcodec_delete_codec_if(const xcodec_if_t *interface)
{
    if (!interface) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = interface->close ? interface->close(interface) : XCODEC_DEV_OK;
    if (!ret) {
        free((void *)interface);
    }
    return ret;
}

int xcodec_delete_ctrl_if(const xcodec_ctrl_if_t *interface)
{
    if (!interface) {
        return XCODEC_DEV_INVALID_ARG;
    }
    int ret = interface->close ? interface->close(interface) : XCODEC_DEV_OK;
    if (!ret) {
        free((void *)interface);
    }
    return ret;
}
