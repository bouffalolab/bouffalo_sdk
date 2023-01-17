/**
 * @file gt911_i2c.h
 * @brief
 *
 * Copyright (c) 2022 Bouffalolab team
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
#ifndef _GT911_I2C_H
#define _GT911_I2C_H

#include "bflb_core.h"
#include "touch.h"

#define GT911_CUSTOM_CFG           (1)
#define GT911_I2C_SLAVE_ADDR       (0xBA >> 1) // #define GT911_ADDR (0xBA)   //(0x28)   /* slave addr */
#define GT911_READ_XY_REG          (0x4E81)    //(0x814E) /* 坐标寄存器 */
#define GT911_CLEARBUF_REG         (0x4E81)    //(0x814E) /* 清除坐标寄存器 */
#define GT911_POINT1_REG           (0x4F81)    //(0x814f) /* point 1 reg */
#define GT911_POINT2_REG           (0x5781)    //(0x8157) /* point 2 reg */
#define GT911_POINT3_REG           (0x5F81)    //(0x815f) /* point 3 reg */
#define GT911_POINT4_REG           (0x6781)    //(0x8167) /* point 4 reg */
#define GT911_POINT5_REG           (0x6F81)    //(0x816f) /* point 5 reg */
#define GT911_CONFIG_REG           (0x4780)    //(0x8047) /* 配置参数寄存器 */
#define GT911_COMMAND_REG          (0x4080)    //(0x8040) /* 实时命令 */
#define GT911_PRODUCT_ID_REG       (0x4081)    //(0x8140) /* 芯片ID */
#define GT911_VENDOR_ID_REG        (0x4A81)    //(0x814A) /* 当前模组选项信息 */
#define GT911_CONFIG_VERSION_REG   (0x4780)    //(0x8047) /* 配置文件版本号 */
#define GT911_CONFIG_CHECKSUM_REG  (0xFF80)    //(0x80FF) /* 配置文件校验码 */
#define GT911_FIRMWARE_VERSION_REG (0x4481)    //(0x8144) /* 固件版本号 */
#define GT911_X_RESOLUTION         (0x4681)    //(0x8146) /* x 坐标分辨率 */
#define GT911_Y_RESOLUTION         (0x4881)    //(0x8148) /* y 坐标分辨率 */

#define GT911_PRODUCT_ID (0x00313139)

/* Maximum border values of the touchscreen pad that the chip can handle */
#if GT911_CUSTOM_CFG
#define GT911_MAX_WIDTH   ((uint16_t)320)
#define GT911_MAX_HEIGHT  ((uint16_t)480)
#define GT911_INT_TRIGGER (0)
#else
#define GT911_MAX_WIDTH   ((uint16_t)4096)
#define GT911_MAX_HEIGHT  ((uint16_t)4096)
#define GT911_INT_TRIGGER (1)
#endif

/* Max detectable simultaneous touch points */
#define GT911_I2C_MAX_POINT 5

/* GT911 配置参数，一次性写入 */
static const uint8_t g_gt911_cfg_params[] = {

#if 1
    /*KDC CFG*/
    0x00, 0x40, 0x01, 0xE0, 0x01, 0x0A, 0x05, 0x00,
    0x01, 0x08, 0x28, 0x05, 0x50, 0x32, 0x03, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x86, 0x26, 0x07, 0x17, 0x15,
    0x31, 0x0D, 0x00, 0x00, 0x01, 0xBA, 0x03, 0x1D,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x64, 0x32,
    0x00, 0x00, 0x00, 0x0F, 0x4B, 0x94, 0xC5, 0x02,
    0x07, 0x00, 0x00, 0x04, 0x9B, 0x11, 0x00, 0x72,
    0x18, 0x00, 0x57, 0x21, 0x00, 0x42, 0x2E, 0x00,
    0x35, 0x40, 0x00, 0x35, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x12, 0x10, 0x0E, 0x0C, 0x0A, 0x08, 0x06, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22, 0x21,
    0x20, 0x1F, 0x1E, 0x1D, 0x00, 0x02, 0x04, 0x06,
    0x08, 0x0A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xDC, 0x01
#else
    0x00,                               //0x8047 版本号 发送 0x00 初始化为 A
    0x40, 0x01,                         //0x8048/8049 X坐标输出最大值 320
    0xE0, 0x01,                         //0x804a/804b Y坐标输出最大值 480
    0x0A,                               //0x804c 输出触点个数上限
    0x05,                               //0x804d x2y 坐标交换，软件降噪，下降沿触发
    0x00,                               //0x804e reserved
    0x01,                               //0x804f 手指按下去抖动次数
    0x08,                               //0x8050 原始坐标窗口滤波值
    0x28,                               //0x8051 大面积触点个数
    0x05,                               //0x8052 噪声消除值
    0x50,                               //0x8053 屏上触摸点从无到有的阈值
    0x32,                               //0x8054 屏上触摸点从有到无的阈值
    0x03,                               //0x8055 进低功耗时间 s
    0x05,                               //0x8056 坐标上报率
    0x00,                               //0x8057 X坐标输出门上限
    0x00,                               //0x8058 Y坐标输出门上限
    0x00, 0X00,                         //0x8059-0x805a reserved
    0x00,                               //0x805b reserved
    0x00,                               //0x805c reserved
    0x00,                               //0x805d 划线过程中小filter设置
    0x00,                               //0x805e 拉伸区间 1 系数
    0x00,                               //0x805f 拉伸区间 2 系数
    0x00,                               //0x8060 拉伸区间 3 系数
    0x00,                               //0x8061 各拉伸区间基数
    0x00,                               //0x8062 Driver Group A number
    0x00,                               //0x8063 Driver Group B number
    0x00,                               //0x8064 Sensor Group B | A number
    0x00,                               //0x8065 驱动组A的驱动频率倍频系数
    0x00,                               //0x8066 驱动组B的驱动频率倍频系数
    0x00,                               //0x8067 驱动组A、B的基频
    0x04,                               //0x8068
    0x00,                               //0x8069 相邻两次驱动信号输出时间间隔
    0x00,                               //0x806a
    0x00,                               //0x806b 、、
    0x02,                               //0x806c 、、
    0x03,                               //0x806d 原始值放大系数
    0x1D,                               //0x806e 、、
    0x00,                               //0x806f reserved
    0x01,                               //0x8070 、、
    0x00, 0x00,                         //reserved
    0x00,                               //0x8073 、、
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0x8071 - 0x8079 reserved
    0x50,                               //0x807a 跳频范围的起点频率
    0xA0,                               //0x807b 跳频范围的终点频率
    0x94,                               //0x807c 多次噪声检测后确定噪声量，1-63有效
    0xD5,                               //0x807d 噪声检测超时时间
    0x02,                               //0x807e 、、
    0x07,                               //0x807f 判别有干扰的门限
    0x00, 0x00,                         //0x8081 reserved
    0x04,                               //0x8082 跳频检测区间频段1中心点基频（适用于驱动A、B）
    0xA4,                               //0x8083
    0x55,                               //0x8084 跳频检测区间频段1中心点倍频系数
    0x00,                               //0x8085 跳频检测区间频段2中心点基频(驱动A、B在此基础上换算)
    0x91,                               //0x8086
    0x62,                               //0x8087 跳频检测区间频段2中心点倍频系数
    0x00,                               //0x8088 跳频检测区间频段3中心点基频（适用于驱动A、B）
    0x80,                               //0x8089
    0x71,                               //0x808a 跳频检测区间频段3中心点倍频系数
    0x00,                               //0x808b 跳频检测区间频段4中心点基频（适用于驱动A、B）
    0x71,                               //0x808c
    0x82,                               //0x808d 跳频检测区间频段4中心点倍频系数
    0x00,                               //0x808e 跳频检测区间频段5中心点基频（适用于驱动A、B）
    0x65,                               //0x808f
    0x95,                               //0x8090 跳频检测区间频段5中心点倍频系数
    0x00, 0x65,                         //reserved
    0x00,                               //0x8093 key1位置 0：无按键
    0x00,                               //0x8094 key2位置 0：无按键
    0x00,                               //0x8095 key3位置 0：无按键
    0x00,                               //0x8096 key4位置 0：无按键
    0x00,                               //0x8097 reserved
    0x00,                               //0x8098 reserved
    0x00,                               //0x8099 reserved
    0x00,                               //0x809a reserved
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0x809b-0x80b2 reserved
    0x00,                               //0x80b3 合框距离
    0x00,                               //0x80b4
    0x00, 0x00,                         //0x80b6 reserved
    0x06,                               //0x80b7
    0x08,                               //0x80b8
    0x0A,                               //0x80b9
    0x0C,                               //0x80ba
    0x0E,                               //0x80bb
    0x10,                               //0x80bc
    0x12,                               //0x80bd
    0x14,                               //0x80be
    0x16,                               //0x80bf
    0x18,                               //0x80c0
    0x1A,                               //0x80c1
    0x1C,                               //0x80c2
    0xFF,                               //0x80c3
    0xFF,                               //0x80c4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0x80c5-0x80d4 reserved
    0x00,                               //0x80d5
    0x02,                               //0x80d6
    0x04,                               //0x80d7
    0x06,                               //0x80d8
    0x08,                               //0x80d9
    0x0A,                               //0x80da
    0x0C,                               //0x80db
    0x0F,                               //0x80dc
    0x10,                               //0x80dd
    0x12,                               //0x80de
    0x13,                               //0x80df
    0x14,                               //0x80e0
    0x16,                               //0x80e1
    0x18,                               //0x80e2
    0x1C,                               //0x80e3
    0x1D,                               //0x80e4
    0x1E,                               //0x80e5
    0x1F,                               //0x80e6
    0x20,                               //0x80e7
    0x21,                               //0x80e8
    0xFF,                               //0x80e9
    0xFF,                               //0x80ea
    0xFF,                               //0x80eb
    0xFF,                               //0x80ec
    0xFF,                               //0x80ed
    0xFF,                               //0x80ee
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //0x80ef-0x80fe reserved
    0x0B,                               //0x80ff 配置信息校验
    0x01                                //0x8100 配置以更新标记
#endif
};

typedef struct
{
    uint8_t touch_point_flag;
    uint8_t touch_key_sts;

    uint16_t x0;
    uint16_t y0;
    uint16_t p0;

    uint16_t x1;
    uint16_t y1;
    uint16_t p1;

    uint16_t x2;
    uint16_t y2;
    uint16_t p2;

    uint16_t x3;
    uint16_t y3;
    uint16_t p3;

    uint16_t x4;
    uint16_t y4;
    uint16_t p4;
} gt911_point_t;

int gt911_i2c_init(touch_coord_t *max_value);
int gt911_i2c_get_gesture_id();
int gt911_i2c_read(uint8_t *point_num, touch_coord_t *touch_coord, uint8_t max_num);

#endif /* __gt911_H */
