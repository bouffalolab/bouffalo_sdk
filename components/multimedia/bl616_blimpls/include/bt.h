/**
 *
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 */

#ifndef __AOS_BT_H__
#define __AOS_BT_H__

#include <stdint.h>
#include <stddef.h>

#ifndef __bt_deprecated
#define __bt_deprecated __attribute__((deprecated))
#endif

#define BT_STACK_BD_ADDR_LEN    6
#define BT_STACK_MAX_BDNAME_LEN (248)

typedef enum {
    BT_DEV_ADDR_TYPE_PUBLIC    = 0x00,
    BT_DEV_ADDR_TYPE_RANDOM    = 0x01,
    BT_DEV_ADDR_TYPE_PUBLIC_ID = 0x02,
    BT_DEV_ADDR_TYPE_RANDOM_ID = 0x03,
} bt_dev_addr_type_t;

typedef struct _bt_dev_addr_t {
    uint8_t type;
    uint8_t val[BT_STACK_BD_ADDR_LEN];
} bt_dev_addr_t;

/* BT_STACK_EIR_FLAG bit definition */
#define BT_DATA_FLAGS_LIMIT_DISC         (0x01 << 0)
#define BT_DATA_FLAGS_GEN_DISC           (0x01 << 1)
#define BT_DATA_FLAGS_BREDR_NOT_SPT      (0x01 << 2)
#define BT_DATA_FLAGS_DMT_CONTROLLER_SPT (0x01 << 3)
#define BT_DATA_FLAGS_DMT_HOST_SPT       (0x01 << 4)

typedef enum {
    BT_DATA_TYPE_FLAGS             = 0x01, /* AD flags */
    BT_DATA_TYPE_UUID16_SOME       = 0x02, /* 16-bit UUID, more available */
    BT_DATA_TYPE_UUID16_ALL        = 0x03, /* 16-bit UUID, all listed */
    BT_DATA_TYPE_UUID32_SOME       = 0x04, /* 32-bit UUID, more available */
    BT_DATA_TYPE_UUID32_ALL        = 0x05, /* 32-bit UUID, all listed */
    BT_DATA_TYPE_UUID128_SOME      = 0x06, /* 128-bit UUID, more available */
    BT_DATA_TYPE_UUID128_ALL       = 0x07, /* 128-bit UUID, all listed */
    BT_DATA_TYPE_NAME_SHORTENED    = 0x08, /* Shortened name */
    BT_DATA_TYPE_NAME_COMPLETE     = 0x09, /* Complete name */
    BT_DATA_TYPE_TX_POWER          = 0x0a, /* Tx Power */
    BT_DATA_TYPE_SOLICIT16         = 0x14, /* Solicit UUIDs, 16-bit */
    BT_DATA_TYPE_SOLICIT128        = 0x15, /* Solicit UUIDs, 128-bit */
    BT_DATA_TYPE_SVC_DATA16        = 0x16, /* Service data, 16-bit UUID */
    BT_DATA_TYPE_GAP_APPEARANCE    = 0x19, /* GAP appearance */
    BT_DATA_TYPE_SOLICIT32         = 0x1f, /* Solicit UUIDs, 32-bit */
    BT_DATA_TYPE_SVC_DATA32        = 0x20, /* Service data, 32-bit UUID */
    BT_DATA_TYPE_SVC_DATA128       = 0x21, /* Service data, 128-bit UUID */
    BT_DATA_TYPE_URI               = 0x24, /* URI */
    BT_DATA_TYPE_MESH_PROV         = 0x29, /* Mesh Provisioning PDU */
    BT_DATA_TYPE_MESH_MESSAGE      = 0x2a, /* Mesh Networking PDU */
    BT_DATA_TYPE_MESH_BEACON       = 0x2b, /* Mesh Beacon */
    BT_DATA_TYPE_MANUFACTURER_DATA = 0xff  /* Manufacturer Specific Data */
} bt_data_type_t;

typedef struct _bt_data_t {
    uint8_t  type;
    uint8_t  len;
    uint8_t *data;
} bt_data_t;

/**
 * @brief      Bluetooth stack error status,
 *             Compatible with minilibc/newlibc errno.h definitions
 */
typedef enum {
    BT_STACK_STATUS_SUCCESS       = 0,   /* Success */
    BT_STACK_STATUS_EPERM         = 1,   /* Operation not permitted */
    BT_STACK_STATUS_ENOENT        = 2,   /* No such file or directory */
    BT_STACK_STATUS_ESRCH         = 3,   /* No such process */
    BT_STACK_STATUS_EIO           = 5,   /* I/O error */
    BT_STACK_STATUS_E2BIG         = 7,   /* Argument list too long */
    BT_STACK_STATUS_ENOEXEC       = 8,   /* Exec format error */
    BT_STACK_STATUS_EAGAIN        = 11,  /* Try again */
    BT_STACK_STATUS_ENOMEM        = 12,  /* Out of memory */
    BT_STACK_STATUS_EACCES        = 13,  /* Permission denied */
    BT_STACK_STATUS_EBUSY         = 16,  /* Device or resource busy */
    BT_STACK_STATUS_EEXIST        = 17,  /* File exists */
    BT_STACK_STATUS_ENODEV        = 19,  /* No such device */
    BT_STACK_STATUS_EISDIR        = 21,  /* Is a directory */
    BT_STACK_STATUS_EINVAL        = 22,  /* Invalid argument */
    BT_STACK_STATUS_ESPIPE        = 29,  /* Illegal seek */
    BT_STACK_STATUS_EDOM          = 33,  /* Math argument out of domain of func */
    BT_STACK_STATUS_ENOMSG        = 42,  /* No message of desired type */
    BT_STACK_STATUS_ENODATA       = 61,  /* No data available */
    BT_STACK_STATUS_EBADMSG       = 74,  /* Not a data message */
    BT_STACK_STATUS_EMSGSIZE      = 90,  /* Message too long */
    BT_STACK_STATUS_EADDRNOTAVAIL = 99,  /* Cannot assign requested address */
    BT_STACK_STATUS_ENOBUFS       = 105, /* No buffer space available */
    BT_STACK_STATUS_ENOTCONN      = 107, /* Transport endpoint is not connected */
    BT_STACK_STATUS_ESHUTDOWN     = 108, /* Cannot send after transport endpoint shutdown */
    BT_STACK_STATUS_ETIMEDOUT     = 110, /* Connection timed out */
    BT_STACK_STATUS_ECONNREFUSED  = 111, /* Connection refused */
    BT_STACK_STATUS_EALREADY      = 114, /* Operation already in progress */
    BT_STACK_STATUS_EINPROGRESS   = 115, /* Operation now in progress */
    BT_STACK_STATUS_ECANCELED     = 125, /* Operation Canceled */
    BT_STACK_STATUS_EUNSUPPORTED  = 134, /* Not supported */
} bt_stack_err_t;

typedef int bt_stack_status_t;

typedef enum {
    BT_STACK_IO_CAP_OUT                 = 0, /*!< DisplayOnly */
    BT_STACK_IO_CAP_IO                  = 1, /*!< DisplayYesNo */
    BT_STACK_IO_CAP_IN                  = 2, /*!< KeyboardOnly */
    BT_STACK_IO_CAP_NONE                = 3, /*!< NoInputNoOutput */
    BT_STACK_IO_CAP_IO_KEYBOARD_DISPLAY = 4, /*!< KeyboardDisplay,  Not used in BR/EDR physical transport. */
} bt_stack_io_cap_t;

/**
 * @brief      Initialize the legacy bluetooth stack
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_init(void);

/**
 * @brief      name for legacy bluetooth.
 * @param[in]  name               name of the device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_name(const char *name);

/**
 * @brief      get device name
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
const char *bt_stack_get_name();

/**
 * @brief      Set IO Capability
 * @param[in]  io_cap             IO Capability
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_io_capability(bt_stack_io_cap_t io_cap);

/**
 * @brief      Removes a device from the security database list of peer device.
 * @param[in]  peer_addr          peer device bluetooth device address, NULL for remove all peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_dev_unpair(bt_dev_addr_t *peer_addr);

/**
 * @brief      call func for each paired devices from the security database
 * @param[in]  func              callback func
 * @param[in]  data              func priv data
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_paired_dev_foreach(void (*func)(bt_dev_addr_t *addr, void *data), void *data);

/// ClassicBT Functions
#if defined(CONFIG_BT_BREDR) && CONFIG_BT_BREDR
#include <classic/bt_stack.h>
#if defined(CONFIG_BT_A2DP) && CONFIG_BT_A2DP
#include <classic/bt_stack_a2dp.h>
#include <classic/bt_stack_avrcp.h>
#endif
#if defined(CONFIG_BT_HFP_HF) && CONFIG_BT_HFP_HF
#include <classic/bt_stack_hfp_hf.h>
#endif
#endif

/// BLE Functions
#include <ble/ble.h>

#endif /* __AOS_BT_H__ */
