/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_EVENT_ID_H
#define YOC_EVENT_ID_H

#include "event.h"

#ifdef __cplusplus
extern "C" {
#endif


/* net driver event
*  from 0x100 to 0x1FF
*  netdrv.h
*/

/* Netmgr */
#define EVENT_NETMGR_GOT_IP          0x01011
#define EVENT_NETMGR_NET_DISCON      0x01012
#define EVENT_NETMGR_NET_CONFIG      0x01013

/* IOT */
#define EVENT_IOT_CONNECT_SUCCESS    0x01030
#define EVENT_IOT_CONNECT_FAILED     0x01031
#define EVENT_IOT_DISCONNECTED       0x01032
#define EVENT_IOT_PUSH_SUCCESS       0x01034
#define EVENT_IOT_PUSH_FAILED        0x01035

/* OS */
#define EVENT_OS_REBOOT              0x01050
#define EVENT_CLOCK_ALARM            0x01051
#define EVENT_CLOCK_TIMEOUT          0x01052

/* FOTA */
#define EVENT_FOTA_START             0x01060
#define EVENT_FOTA_UPDATE            0x01061

/* BLE */
#define EVENT_BLE                    0x05000000

#define EVENT_BLE_MESH               0x05100000

/* USER */
#define EVENT_USER                   0x10000000

#ifdef __cplusplus
}
#endif

#endif
