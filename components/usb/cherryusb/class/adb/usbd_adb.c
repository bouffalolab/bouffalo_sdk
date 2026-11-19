/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "usbd_core.h"
#include "usbd_adb.h"

#define ADB_OUT_EP_IDX 0
#define ADB_IN_EP_IDX  1

/* Receive state machine: one host command is processed at a time. */
#define ADB_STATE_READ_MSG  0
#define ADB_STATE_READ_DATA 1

/* Transmit engine: every IN transfer is serialized through this stage. */
#define ADB_TX_NONE      0
#define ADB_TX_RESP_MSG  1
#define ADB_TX_RESP_DATA 2
#define ADB_TX_APP_MSG   3
#define ADB_TX_APP_DATA  4

#define ADB_RESP_QUEUE_LEN 4

#define MAX_PAYLOAD_V1 (4 * 1024)
#define MAX_PAYLOAD_V2 (256 * 1024)
#define MAX_PAYLOAD    MAX_PAYLOAD_V1
#define A_VERSION      0x01000000

#define A_SYNC 0x434e5953
#define A_CNXN 0x4e584e43
#define A_OPEN 0x4e45504f
#define A_OKAY 0x59414b4f
#define A_CLSE 0x45534c43
#define A_WRTE 0x45545257
#define A_AUTH 0x48545541

struct adb_msg {
    uint32_t command;     /* command identifier constant (A_CNXN, ...) */
    uint32_t arg0;        /* first argument                            */
    uint32_t arg1;        /* second argument                           */
    uint32_t data_length; /* length of payload (0 is allowed)          */
    uint32_t data_crc32;  /* crc32 of data payload                     */
    uint32_t magic;       /* command ^ 0xffffffff */
};

struct adb_packet {
    USB_MEM_ALIGNX struct adb_msg msg;
    USB_MEM_ALIGNX uint8_t payload[USB_ALIGN_UP(MAX_PAYLOAD, CONFIG_USB_ALIGN_SIZE)];
};

/** @brief One class generated response waiting for the IN endpoint. */
struct adb_resp {
    uint32_t command;
    uint32_t arg0;
    uint32_t arg1;
};

struct usbd_adb {
    uint8_t common_state;
    uint8_t tx_stage;
    uint8_t notify_channel; /* channel whose A_WRTE is being acknowledged */
    bool app_pending;
    bool writable;
    uint32_t localid;
    uint32_t shell_remoteid;
    uint32_t file_remoteid;
};

static struct usbd_adb adb_client;

static struct usbd_endpoint adb_ep_data[2];

static struct adb_resp adb_resp_queue[ADB_RESP_QUEUE_LEN];
static uint8_t adb_resp_head;
static uint8_t adb_resp_tail;
static uint32_t adb_last_resp_command;
static bool adb_tx_busy;

struct adb_open_request {
    uint32_t localid;
    uint32_t len;
    uint8_t service[ADB_MAX_SERVICE_LEN];
    bool pending;
};

static struct adb_open_request adb_open_request;

USB_NOCACHE_RAM_SECTION struct adb_packet tx_packet;
USB_NOCACHE_RAM_SECTION struct adb_packet rx_packet;

#ifdef CONFIG_CHERRYUSB_ADB_OSAL_LOCK
#define adb_enter_critical() usb_osal_enter_critical_section()
#define adb_leave_critical(flag) usb_osal_leave_critical_section(flag)
#else
#define adb_enter_critical() 0
#define adb_leave_critical(flag) (void)(flag)
#endif

static inline uint32_t adb_packet_checksum(struct adb_packet *packet)
{
    uint32_t sum = 0;
    uint32_t i;

    for (i = 0; i < packet->msg.data_length; ++i) {
        sum += (uint32_t)(packet->payload[i]);
    }

    return sum;
}

static uint32_t usbd_adb_get_remoteid(uint32_t localid)
{
    if (localid == ADB_SHELL_LOALID) {
        return adb_client.shell_remoteid;
    } else {
        return adb_client.file_remoteid;
    }
}

/* Remember the accepted service string; it is reported to the application
 * once the A_OKAY response has been sent, so that the application can start
 * writing data without racing with the class-managed IN transfer. */
static void adb_request_open_notify(uint32_t localid, const uint8_t *service, uint32_t len)
{
    if (len > sizeof(adb_open_request.service) - 1) {
        len = sizeof(adb_open_request.service) - 1;
    }

    adb_open_request.localid = localid;
    adb_open_request.len = len;
    if (len) {
        memcpy(adb_open_request.service, service, len);
    }
    adb_open_request.service[len] = '\0';
    adb_open_request.pending = true;
}

static void adb_tx_start_app(void)
{
    adb_client.app_pending = false;
    adb_client.tx_stage = ADB_TX_APP_MSG;
    adb_tx_busy = true;
    usbd_ep_start_write(0, adb_ep_data[ADB_IN_EP_IDX].ep_addr, (uint8_t *)&tx_packet.msg, sizeof(struct adb_msg));
}

static void adb_tx_start_resp(void)
{
    static const char support_feature[] = "device::"
                                          "ro.product.name=cherryadb;"
                                          "ro.product.model=cherrysh;"
                                          "ro.product.device=cherryadb;"
                                          "features=cmd,shell_v1";
    struct adb_resp *resp = &adb_resp_queue[adb_resp_head];

    adb_resp_head = (adb_resp_head + 1) % ADB_RESP_QUEUE_LEN;

    tx_packet.msg.command = resp->command;
    tx_packet.msg.arg0 = resp->arg0;
    tx_packet.msg.arg1 = resp->arg1;
    tx_packet.msg.data_length = 0;

    if (resp->command == A_CNXN) {
        tx_packet.msg.data_length = sizeof(support_feature) - 1;
        memcpy(tx_packet.payload, support_feature, sizeof(support_feature) - 1);
    }

    tx_packet.msg.data_crc32 = adb_packet_checksum(&tx_packet);
    tx_packet.msg.magic = tx_packet.msg.command ^ 0xffffffff;

    adb_last_resp_command = resp->command;
    adb_client.tx_stage = ADB_TX_RESP_MSG;
    adb_tx_busy = true;
    usbd_ep_start_write(0, adb_ep_data[ADB_IN_EP_IDX].ep_addr, (uint8_t *)&tx_packet.msg, sizeof(struct adb_msg));
}

/* Pick the next IN transfer. Application data has priority, class responses
 * are kept queued until the application is done with the shared tx packet. */
static void adb_tx_kick(void)
{
    if (adb_tx_busy) {
        return;
    }

    if (adb_client.app_pending) {
        adb_tx_start_app();
    } else if (adb_resp_head != adb_resp_tail) {
        adb_tx_start_resp();
    }
}

static bool adb_queue_resp(uint32_t command, uint32_t arg0, uint32_t arg1)
{
    uint8_t next = (adb_resp_tail + 1) % ADB_RESP_QUEUE_LEN;

    if (next == adb_resp_head) {
        USB_LOG_ERR("ADB response queue full\r\n");
        return false;
    }

    adb_resp_queue[adb_resp_tail].command = command;
    adb_resp_queue[adb_resp_tail].arg0 = arg0;
    adb_resp_queue[adb_resp_tail].arg1 = arg1;
    adb_resp_tail = next;

    adb_tx_kick();
    return true;
}

static void adb_rearm_read(uint8_t busid)
{
    adb_client.common_state = ADB_STATE_READ_MSG;
    usbd_ep_start_read(busid, adb_ep_data[ADB_OUT_EP_IDX].ep_addr, (uint8_t *)&rx_packet.msg, sizeof(struct adb_msg));
}

static void adb_tx_finish_resp(uint8_t busid)
{
    uint32_t command = adb_last_resp_command;

    adb_client.tx_stage = ADB_TX_NONE;
    adb_tx_busy = false;

    if (command == A_OKAY) {
        if (adb_open_request.pending) {
            adb_open_request.pending = false;
            usbd_adb_notify_open(adb_open_request.localid, adb_open_request.service, adb_open_request.len);
        } else if (adb_client.notify_channel) {
            adb_client.writable = true;
            if (adb_client.notify_channel == ADB_SHELL_LOALID) {
                usbd_adb_notify_shell_read(rx_packet.payload, rx_packet.msg.data_length);
            } else {
                usbd_adb_notify_file_read(rx_packet.payload, rx_packet.msg.data_length);
            }
            adb_client.notify_channel = 0;
        }
    }

    adb_rearm_read(busid);
    adb_tx_kick();
}

static void adb_reset_state(void)
{
    adb_client.common_state = ADB_STATE_READ_MSG;
    adb_client.tx_stage = ADB_TX_NONE;
    adb_client.notify_channel = 0;
    adb_client.app_pending = false;
    adb_client.writable = false;
    adb_tx_busy = false;
    adb_resp_head = 0;
    adb_resp_tail = 0;
    adb_last_resp_command = 0;
    adb_open_request.pending = false;
}

void usbd_adb_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)ep;

    if (adb_client.common_state == ADB_STATE_READ_MSG) {
        if (nbytes != sizeof(struct adb_msg)) {
            /* A zero length OUT packet carries no ADB message; ignore it. */
            if (nbytes) {
                USB_LOG_ERR("ADB bad message size:%u\r\n", (unsigned int)nbytes);
            }
            adb_rearm_read(busid);
            return;
        }

        USB_LOG_DBG("command:%x arg0:%x arg1:%x len:%d\r\n",
                    rx_packet.msg.command,
                    rx_packet.msg.arg0,
                    rx_packet.msg.arg1,
                    rx_packet.msg.data_length);

        if (rx_packet.msg.data_length) {
            if (rx_packet.msg.data_length > sizeof(rx_packet.payload)) {
                USB_LOG_ERR("ADB payload too large:%u\r\n", (unsigned int)rx_packet.msg.data_length);
                adb_rearm_read(busid);
                return;
            }
            adb_client.common_state = ADB_STATE_READ_DATA;
            usbd_ep_start_read(busid, adb_ep_data[ADB_OUT_EP_IDX].ep_addr, rx_packet.payload, rx_packet.msg.data_length);
        } else {
            if (rx_packet.msg.command == A_CLSE) {
                adb_client.writable = false;
                USB_LOG_INFO("Close remoteid:%x\r\n", rx_packet.msg.arg0);
            }
            adb_rearm_read(busid);
        }
    } else if (adb_client.common_state == ADB_STATE_READ_DATA) {
        bool response_queued = false;

        if (nbytes != rx_packet.msg.data_length) {
            USB_LOG_ERR("ADB bad payload size:%u/%u\r\n",
                        (unsigned int)nbytes, (unsigned int)rx_packet.msg.data_length);
            adb_rearm_read(busid);
            return;
        }

        switch (rx_packet.msg.command) {
            case A_CNXN: /* CONNECT(version, maxdata, "system-id-string") */
                adb_client.writable = false;
                response_queued = adb_queue_resp(A_CNXN, A_VERSION, MAX_PAYLOAD);
                break;

            case A_OPEN: /* OPEN(local-id, 0, "destination") */
            {
                uint32_t service_len = rx_packet.msg.data_length;

                /* The ADB host includes the NUL terminator of the service
                 * string in data_length; strip it before parsing. */
                while (service_len && (rx_packet.payload[service_len - 1] == '\0')) {
                    service_len--;
                }

                if (service_len >= sizeof(rx_packet.payload)) {
                    response_queued = adb_queue_resp(A_CLSE, 0, rx_packet.msg.arg0);
                    break;
                }

                rx_packet.payload[service_len] = '\0';

                if (strncmp((const char *)rx_packet.payload, "shell:", 6) == 0) {
                    adb_client.localid = ADB_SHELL_LOALID;
                    adb_client.shell_remoteid = rx_packet.msg.arg0;
                    adb_request_open_notify(ADB_SHELL_LOALID, &rx_packet.payload[6], service_len - 6);
                    response_queued = adb_queue_resp(A_OKAY, ADB_SHELL_LOALID, adb_client.shell_remoteid);

                    USB_LOG_INFO("Open shell service, remoteid:%x\r\n", rx_packet.msg.arg0);
                } else if (strncmp((const char *)rx_packet.payload, "sync:", 5) == 0) {
                    adb_client.localid = ADB_FILE_LOALID;
                    adb_client.file_remoteid = rx_packet.msg.arg0;
                    adb_request_open_notify(ADB_FILE_LOALID, &rx_packet.payload[5], service_len - 5);
                    response_queued = adb_queue_resp(A_OKAY, ADB_FILE_LOALID, adb_client.file_remoteid);
                    USB_LOG_INFO("Open file service, remoteid:%x\r\n", rx_packet.msg.arg0);
                } else {
                    USB_LOG_INFO("Reject unsupported service:%s\r\n", rx_packet.payload);
                    response_queued = adb_queue_resp(A_CLSE, 0, rx_packet.msg.arg0);
                }
            } break;

            case A_WRTE: /* WRITE(local-id, remote-id, "data") */
                if ((rx_packet.msg.arg0 == adb_client.shell_remoteid) && (rx_packet.msg.arg1 == ADB_SHELL_LOALID)) {
                    adb_client.notify_channel = ADB_SHELL_LOALID;
                    response_queued = adb_queue_resp(A_OKAY, ADB_SHELL_LOALID, adb_client.shell_remoteid);
                } else if ((rx_packet.msg.arg0 == adb_client.file_remoteid) && (rx_packet.msg.arg1 == ADB_FILE_LOALID)) {
                    adb_client.notify_channel = ADB_FILE_LOALID;
                    response_queued = adb_queue_resp(A_OKAY, ADB_FILE_LOALID, adb_client.file_remoteid);
                } else {
                    response_queued = adb_queue_resp(A_CLSE, 0, rx_packet.msg.arg0);
                }
                break;

            case A_SYNC:
            case A_OKAY:
            case A_CLSE:
            case A_AUTH:
            default:
                break;
        }

        if (!response_queued) {
            adb_rearm_read(busid);
        }
    }
}

void usbd_adb_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)ep;
    (void)nbytes;

    switch (adb_client.tx_stage) {
        case ADB_TX_RESP_MSG:
            if (tx_packet.msg.data_length) {
                adb_client.tx_stage = ADB_TX_RESP_DATA;
                usbd_ep_start_write(busid, adb_ep_data[ADB_IN_EP_IDX].ep_addr, tx_packet.payload, tx_packet.msg.data_length);
            } else {
                adb_tx_finish_resp(busid);
            }
            break;

        case ADB_TX_RESP_DATA:
            adb_tx_finish_resp(busid);
            break;

        case ADB_TX_APP_MSG:
            adb_client.tx_stage = ADB_TX_APP_DATA;
            usbd_ep_start_write(busid, adb_ep_data[ADB_IN_EP_IDX].ep_addr, tx_packet.payload, tx_packet.msg.data_length);
            break;

        case ADB_TX_APP_DATA:
            adb_client.tx_stage = ADB_TX_NONE;
            adb_tx_busy = false;
            usbd_adb_notify_write_done();
            adb_tx_kick();
            break;

        default:
            break;
    }
}

void adb_notify_handler(uint8_t busid, uint8_t event, void *arg)
{
    (void)arg;

    switch (event) {
        case USBD_EVENT_INIT:
        case USBD_EVENT_DEINIT:
        case USBD_EVENT_RESET:
            adb_reset_state();
            break;
        case USBD_EVENT_CONFIGURED:
            adb_reset_state();
            /* setup first out ep read transfer */
            usbd_ep_start_read(busid, adb_ep_data[ADB_OUT_EP_IDX].ep_addr, (uint8_t *)&rx_packet.msg, sizeof(struct adb_msg));
            break;

        default:
            break;
    }
}

struct usbd_interface *usbd_adb_init_intf(uint8_t busid, struct usbd_interface *intf, uint8_t in_ep, uint8_t out_ep)
{
    (void)busid;

    intf->class_interface_handler = NULL;
    intf->class_endpoint_handler = NULL;
    intf->vendor_handler = NULL;
    intf->notify_handler = adb_notify_handler;

    adb_ep_data[ADB_OUT_EP_IDX].ep_addr = out_ep;
    adb_ep_data[ADB_OUT_EP_IDX].ep_cb = usbd_adb_bulk_out;
    adb_ep_data[ADB_IN_EP_IDX].ep_addr = in_ep;
    adb_ep_data[ADB_IN_EP_IDX].ep_cb = usbd_adb_bulk_in;

    usbd_add_endpoint(busid, &adb_ep_data[ADB_OUT_EP_IDX]);
    usbd_add_endpoint(busid, &adb_ep_data[ADB_IN_EP_IDX]);

    return intf;
}

bool usbd_adb_can_write(void)
{
    return adb_client.writable;
}

int usbd_abd_write(uint32_t localid, const uint8_t *data, uint32_t len)
{
    struct adb_packet *packet;
    size_t flag;

    if (!data || !len || len > MAX_PAYLOAD) {
        return -1;
    }

    flag = adb_enter_critical();

    if (adb_tx_busy || adb_client.app_pending) {
        adb_leave_critical(flag);
        return -1;
    }

    packet = &tx_packet;
    packet->msg.command = A_WRTE;
    packet->msg.arg0 = localid;
    packet->msg.arg1 = usbd_adb_get_remoteid(localid);
    packet->msg.data_length = len;
    memcpy(packet->payload, data, len);

    packet->msg.data_crc32 = adb_packet_checksum(packet);
    packet->msg.magic = packet->msg.command ^ 0xffffffff;

    adb_client.app_pending = true;
    adb_tx_kick();

    adb_leave_critical(flag);
    return 0;
}

void usbd_adb_close(uint32_t localid)
{
    size_t flag = adb_enter_critical();

    (void)adb_queue_resp(A_CLSE, localid, usbd_adb_get_remoteid(localid));

    adb_leave_critical(flag);
}
