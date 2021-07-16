/**
 * @file hal_cam.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "bl702_cam.h"
#include "bl702_glb.h"
#include "hal_cam.h"

#ifdef BSP_USING_CAM
static intCallback_Type *camIntCbfArra[CAM_INT_ALL] = { NULL };

void CAMERA_IRQ(void);
#endif
/**
 * @brief
 *
 * @param cam_cfg
 */
void cam_init(cam_device_t *cam_cfg, uint16_t oflag)
{
    CAM_CFG_Type camera_cfg = {
        .swMode = cam_cfg->software_mode,
        .swIntCnt = 0,
        .frameMode = cam_cfg->frame_mode,
        .yuvMode = cam_cfg->yuv_format,
        .waitCount = 0x40,
        .linePol = CAM_LINE_ACTIVE_POLARITY_HIGH,
        .framePol = CAM_FRAME_ACTIVE_POLARITY_HIGH,
        .burstType = CAM_BURST_TYPE_INCR16,
        .camSensorMode = CAM_SENSOR_MODE_V_AND_H,
        .memStart0 = cam_cfg->cam_write_ram_addr,
        .memSize0 = cam_cfg->cam_write_ram_size,
        .frameSize0 = cam_cfg->cam_frame_size,
        /* planar mode*/
        .memStart1 = cam_cfg->cam_write_ram_addr1,
        .memSize1 = cam_cfg->cam_write_ram_size1,
        .frameSize1 = cam_cfg->cam_frame_size1,
    };
    CAM_Init(&camera_cfg);

    if (oflag == DEVICE_OFLAG_INT) {
#ifdef BSP_USING_CAM
        Interrupt_Handler_Register(CAM_IRQn, CAMERA_IRQ);
#endif
    }
}

/**
 * @brief
 *
 */
void cam_start(void)
{
    CAM_Enable();
}

/**
 * @brief
 *
 */
void cam_stop(void)
{
    CAM_Disable();
}

/**
 * @brief
 *
 * @param pic
 * @param len
 */
uint8_t cam_get_one_frame_interleave(uint8_t **pic, uint32_t *len)
{
    CAM_Interleave_Frame_Info info;
    arch_memset(&info, 0, sizeof(info));

    CAM_Interleave_Get_Frame_Info(&info);

    if (info.validFrames == 0) {
        return ERROR;
    }

    *pic = (uint8_t *)(info.curFrameAddr);
    *len = info.curFrameBytes;

    return SUCCESS;
}

uint8_t cam_get_one_frame_planar(CAM_YUV_Mode_Type yuv, uint8_t **picYY, uint32_t *lenYY, uint8_t **picUV, uint32_t *lenUV)
{
    CAM_Planar_Frame_Info info;
    arch_memset(&info, 0, sizeof(info));

    CAM_Planar_Get_Frame_Info(&info);

    if (yuv == CAM_YUV400_EVEN || yuv == CAM_YUV400_ODD) {
        if (info.validFrames0 == 0 && info.validFrames1 == 0) {
            return ERROR;
        }
    } else {
        if (info.validFrames0 == 0 || info.validFrames1 == 0) {
            return ERROR;
        }
    }

    *picYY = (uint8_t *)(info.curFrameAddr0);
    *lenYY = info.curFrameBytes0;
    *picUV = (uint8_t *)(info.curFrameAddr1);
    *lenUV = info.curFrameBytes1;

    return SUCCESS;
}

void cam_drop_one_frame_interleave(void)
{
    CAM_Interleave_Pop_Frame();
}

void cam_drop_one_frame_planar(void)
{
    CAM_Planar_Pop_Frame();
}

void cam_hsync_crop(uint16_t start, uint16_t end)
{
    CAM_Hsync_Crop(start, end);
}

void cam_vsync_crop(uint16_t start, uint16_t end)
{
    CAM_Vsync_Crop(start, end);
}

void cam_hw_mode_wrap(uint8_t enable)
{
    CAM_HW_Mode_Wrap(enable);
}

void cam_isr(cam_device_t *handle)
{
}

#ifdef BSP_USING_CAM

void CAM_Int_Callback_set(CAM_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_CAM_INT_TYPE(intType));

    camIntCbfArra[intType] = cbFun;
}

void CAMERA_IRQ(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(CAM_BASE, CAM_DVP_STATUS_AND_ERROR);
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_NORMAL_INT_0)) {
        CAM_IntClr(CAM_INT_NORMAL_0);
        if (camIntCbfArra[CAM_INT_NORMAL_0] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_NORMAL_0]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_NORMAL_INT_1)) {
        CAM_IntClr(CAM_INT_NORMAL_1);
        if (camIntCbfArra[CAM_INT_NORMAL_1] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_NORMAL_1]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_MEM_INT_0)) {
        CAM_IntClr(CAM_INT_MEMORY_OVERWRITE_0);
        if (camIntCbfArra[CAM_INT_MEMORY_OVERWRITE_0] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_MEMORY_OVERWRITE_0]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_MEM_INT_1)) {
        CAM_IntClr(CAM_INT_MEMORY_OVERWRITE_1);
        if (camIntCbfArra[CAM_INT_MEMORY_OVERWRITE_1] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_MEMORY_OVERWRITE_1]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FRAME_INT_0)) {
        CAM_IntClr(CAM_INT_FRAME_OVERWRITE_0);
        if (camIntCbfArra[CAM_INT_FRAME_OVERWRITE_0] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_FRAME_OVERWRITE_0]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FRAME_INT_1)) {
        CAM_IntClr(CAM_INT_FRAME_OVERWRITE_1);
        if (camIntCbfArra[CAM_INT_FRAME_OVERWRITE_1] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_FRAME_OVERWRITE_1]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FIFO_INT_0)) {
        CAM_IntClr(CAM_INT_FIFO_OVERWRITE_0);
        if (camIntCbfArra[CAM_INT_FIFO_OVERWRITE_0] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_FIFO_OVERWRITE_0]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_FIFO_INT_1)) {
        CAM_IntClr(CAM_INT_FIFO_OVERWRITE_1);
        if (camIntCbfArra[CAM_INT_FIFO_OVERWRITE_1] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_FIFO_OVERWRITE_1]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_HCNT_INT)) {
        CAM_IntClr(CAM_INT_HSYNC_CNT_ERROR);
        if (camIntCbfArra[CAM_INT_HSYNC_CNT_ERROR] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_HSYNC_CNT_ERROR]();
        }
    }
    if (BL_IS_REG_BIT_SET(tmpVal, CAM_STS_VCNT_INT)) {
        CAM_IntClr(CAM_INT_VSYNC_CNT_ERROR);
        if (camIntCbfArra[CAM_INT_VSYNC_CNT_ERROR] != NULL) {
            /* call the callback function */
            camIntCbfArra[CAM_INT_VSYNC_CNT_ERROR]();
        }
    }
}

#endif
