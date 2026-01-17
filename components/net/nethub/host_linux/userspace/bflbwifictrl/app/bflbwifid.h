/**
 * @file bflbwifid.h
 * @brief BFLB WiFi daemon communication protocol definition (daemon and CLI tool communication)
 */

#ifndef BFLBWIFID_H
#define BFLBWIFID_H

/* ========== Constant Definitions ========== */

#define UNIX_SOCKET_PATH    "/tmp/bflbwifi.sock"
#define PID_FILE            "/var/run/bflbwifid.pid"
#define LOG_FILE            "/var/log/bflbwifi.log"
#define MAX_BUFFER_SIZE     4096
#define MAX_CMD_LEN         256
#define MAX_RESPONSE_LEN    4096

/* Communication protocol commands */
typedef enum {
    IPC_CMD_CONNECT_AP = 0,      /* Connect to AP: CONNECT_AP <ssid> <password> */
    IPC_CMD_DISCONNECT,          /* Disconnect: DISCONNECT */
    IPC_CMD_SCAN,                /* Scan APs: SCAN */
    IPC_CMD_STATUS,              /* Query status: STATUS */
    IPC_CMD_VERSION,             /* Query version: VERSION */
    IPC_CMD_SET_MODE,            /* Set mode: SET_MODE <mode> */
    IPC_CMD_SET_STATIC_IP,       /* Set static IP: SET_STATIC_IP <ip> <gateway> <netmask> */
    IPC_CMD_SET_DHCP,            /* Set DHCP: SET_DHCP <enable> */
    IPC_CMD_AP_CONFIG,           /* Configure AP: AP_CONFIG <ssid> <pwd> <channel> <enc> */
    IPC_CMD_AP_START,            /* Start AP: AP_START */
    IPC_CMD_AP_STOP,             /* Stop AP: AP_STOP */
    IPC_CMD_QUIT,                /* Quit daemon: QUIT */
} ipc_cmd_t;

/* Communication protocol response status codes */
typedef enum {
    IPC_RESP_OK = 0,             /* Operation successful */
    IPC_RESP_ERROR = -1,         /* General error */
    IPC_RESP_INVALID_PARAM = -4, /* Parameter error */
    IPC_RESP_TIMEOUT = -5,       /* Timeout */
    IPC_RESP_NOT_SUPPORTED = -10,/* Not supported */
    IPC_RESP_BUSY = -11,         /* Busy */
} ipc_resp_t;

#endif /* BFLBWIFID_H */
