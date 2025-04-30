#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

#include "FreeRTOSConfig.h"

/**
 * CONFIG_BLUETOOTH: Enable the bluetooh stack
 */
//#ifndef CONFIG_BLUETOOTH
//#error "CONFIG_BLUETOOTH not defined,this header shoudn't include"
//#endif

#ifdef CONFIG_BT_BONDABLE
#undef CONFIG_BT_BONDABLE
#define CONFIG_BT_BONDABLE 1
#endif

#define CONFIG_BT_SMP_ALLOW_UNAUTH_OVERWRITE 1

#if defined(CONFIG_BT_STACK_PTS)

#ifndef  PTS_CHARC_LEN_EQUAL_MTU_SIZE
#define  PTS_CHARC_LEN_EQUAL_MTU_SIZE	
#endif


//#ifndef  CONFIG_BT_STACK_PTS_SM_SLA_KDU_BI_01
//#define  CONFIG_BT_STACK_PTS_SM_SLA_KDU_BI_01	
//#endif

//#ifndef  PTS_GAP_SLAVER_CONFIG_READ_CHARC
//#define  PTS_GAP_SLAVER_CONFIG_READ_CHARC		
//#endif

//#ifndef  PTS_GAP_SLAVER_CONFIG_WRITE_CHARC
//#define  PTS_GAP_SLAVER_CONFIG_WRITE_CHARC		
//#endif

//#ifndef  PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC
//#define  PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC		
//#endif

//#ifndef  PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC
//#define  PTS_GAP_SLAVER_CONFIG_INDICATE_CHARC	
//#endif
#define CONFIG_BT_GATT_READ_MULTIPLE 1
#endif

/**
 * CONFIG_BT_HCI_RX_STACK_SIZE: rx thread stack size
 */
#ifndef CONFIG_BT_HCI_RX_STACK_SIZE
#define CONFIG_BT_HCI_RX_STACK_SIZE  512
#endif

#ifndef CONFIG_BT_RX_STACK_SIZE
#if defined(CONFIG_BT_MESH)
#if defined(CONFIG_BT_MESH_MODEL) || defined(CONFIG_AUTO_PTS)
#define CONFIG_BT_RX_STACK_SIZE  3072
#else
#define CONFIG_BT_RX_STACK_SIZE  2048
#endif
#else
#if !defined(CONFIG_BT_CONN)
#define CONFIG_BT_RX_STACK_SIZE  1024
#else
#define CONFIG_BT_RX_STACK_SIZE  2048//1536//1024
#endif
#endif
#endif

#ifndef CONFIG_BT_CTLR_RX_PRIO_STACK_SIZE
#define CONFIG_BT_CTLR_RX_PRIO_STACK_SIZE 156
#endif

#define CONFIG_BT_HCI_ECC_STACK_SIZE  384

#ifndef CONFIG_BT_RX_PRIO
#define CONFIG_BT_RX_PRIO (configMAX_PRIORITIES - 4)
#endif
/**
 * CONFIG_BT: Tx thread stack size
 */

#ifndef CONFIG_BT_HCI_TX_STACK_SIZE
#if !defined(CONFIG_BT_CONN)
#define CONFIG_BT_HCI_TX_STACK_SIZE  1024
#else
#if defined(CFG_IOT_SDK) || defined(BL_MCU_SDK)
#define CONFIG_BT_HCI_TX_STACK_SIZE 1536//1024//200
#else
#define CONFIG_BT_HCI_TX_STACK_SIZE 1536+512//1024//200
#endif /*CFG_IOT_SDK || BL_MCU_SDK*/
#endif
#endif

/**
 * CONFIG_BT_HCI_TX_PRIO: tx thread priority
 */
#ifndef CONFIG_BT_HCI_TX_PRIO
#define CONFIG_BT_HCI_TX_PRIO (configMAX_PRIORITIES - 3)
#endif

#ifndef CONFIG_BT_CTLR_RX_PRIO
#define CONFIG_BT_CTLR_RX_PRIO (configMAX_PRIORITIES - 4)
#endif

/**
* CONFIG_BT_HCI_CMD_COUNT: hci cmd buffer count,range 2 to 64
*/
#ifndef CONFIG_BT_HCI_CMD_COUNT
#define CONFIG_BT_HCI_CMD_COUNT 2
#endif

/**
* CONFIG_BT_RX_BUF_COUNT: number of buffer for incoming ACL packages or HCI
* events,range 2 to 255
*/
#ifndef CONFIG_BT_RX_BUF_COUNT

#if defined(CONFIG_BT_MESH)
#define CONFIG_BT_RX_BUF_COUNT 10
#else
#define CONFIG_BT_RX_BUF_COUNT 5
#endif //CONFIG_BT_MESH
#endif

#if defined(CONFIG_BT_MESH_PTS)
//#define CONFIG_MESH_IOPT_BV_02_C
//#define MESH_LCTL_BIND_WITH_GENLVL   //Lighting CTL Temperature state bind with Genneric Level
//#define MESH_LHSLH_BIND_WITH_GENLVL  //Lighting HSL Hue state bind with Genneric Level
//#define MESH_LHSLSA_BIND_WITH_GENLVL //Lighting HSL Saturation state bind with Genneric Level
#endif
/**
* CONFIG_BT_RX_BUF_RSV_COUNT: number of buffer that HCI_LE_EVENT reserved
* events,range 1 to CONFIG_BT_RX_BUF_COUNT
*/
#define CONFIG_BT_RX_BUF_RSV_COUNT (1)
#if (CONFIG_BT_RX_BUF_RSV_COUNT >= CONFIG_BT_RX_BUF_COUNT)
#error "CONFIG_BT_RX_BUF_RSV_COUNT config error"
#endif

/**
* CONFIG_BT_RX_BUF_LEN: the max length for rx buffer
* range 73 to 2000
*/
#ifndef CONFIG_BT_RX_BUF_LEN
#if defined(CONFIG_BT_BREDR)
#define CONFIG_BT_RX_BUF_LEN 680 //CONFIG_BT_L2CAP_RX_MTU + 4 + 4
#else
#define CONFIG_BT_RX_BUF_LEN 255 //108 //76
#endif
#endif

/**
* CONFIG_BT_CENTRAL: Enable central Role
*/
#ifdef CONFIG_BT_CENTRAL
#undef CONFIG_BT_CENTRAL
#define CONFIG_BT_CENTRAL 1
#endif

/**
* CONFIG_BT_WHITELIST : Enable autoconnect whilt list device */
#ifndef CONFIG_BT_WHITELIST
#define CONFIG_BT_WHITELIST 1
#endif

/**
* CONFIG_BT_PERIPHERAL: Enable peripheral Role
*/
#ifdef CONFIG_BT_PERIPHERAL
#undef CONFIG_BT_PERIPHERAL
#define CONFIG_BT_PERIPHERAL 1
#endif

#if defined(CONFIG_BT_CENTRAL) || defined(CONFIG_BT_PERIPHERAL)
#undef CONFIG_BT_CONN
#define CONFIG_BT_CONN 1
#endif

#ifdef CONFIG_BT_CONN 

#ifndef CONFIG_BT_CREATE_CONN_TIMEOUT
#define CONFIG_BT_CREATE_CONN_TIMEOUT 3 
#endif

#if defined(BFLB_BLE)
#ifndef CONFIG_BT_CONN_PARAM_UPDATE_TIMEOUT
#define CONFIG_BT_CONN_PARAM_UPDATE_TIMEOUT 5
#endif
#endif
/**
* CONFIG_BLUETOOTH_L2CAP_TX_BUF_COUNT: number of buffer for outgoing L2CAP packages
* range 2 to 255
*/
#ifndef CONFIG_BT_L2CAP_TX_BUF_COUNT
#define CONFIG_BT_L2CAP_TX_BUF_COUNT CFG_BLE_TX_BUFF_DATA
#endif

/**
* CONFIG_BT_L2CAP_TX_MTU: Max L2CAP MTU for L2CAP tx buffer
* range 65 to 2000 if SMP enabled,otherwise range 23 to 2000
*/
#ifndef CONFIG_BT_L2CAP_TX_MTU
#ifdef CONFIG_BT_SMP
#if defined(CONFIG_BT_A2DP_SOURCE)
#define CONFIG_BT_L2CAP_TX_MTU 680
#else
#define CONFIG_BT_L2CAP_TX_MTU 247 //96 //65
#endif
#else
#if defined(CONFIG_BT_A2DP_SOURCE)
#define CONFIG_BT_L2CAP_TX_MTU 680
#else
#define CONFIG_BT_L2CAP_TX_MTU 247 //96 //65
#endif
#endif
#endif

#ifndef CONFIG_USER_DATA_LEN_UPDATE
#define CONFIG_USER_DATA_LEN_UPDATE 1
#endif

#ifndef CONFIG_BT_GATT_DIS_SETTINGS
#define CONFIG_BT_GATT_DIS_SETTINGS 1
#endif
/**
* CONFIG_BT_L2CAP_TX_USER_DATA_SIZE: the max length for L2CAP tx buffer user data size
* range 4 to 65535
*/
#ifndef CONFIG_BT_L2CAP_TX_USER_DATA_SIZE
#define CONFIG_BT_L2CAP_TX_USER_DATA_SIZE 4
#endif

#if defined(CONFIG_BT_STACK_PTS)&& (defined(PTS_GAP_SLAVER_CONFIG_WRITE_CHARC) || defined(PTS_TEST_CASE_INSUFFICIENT_KEY))
#define CONFIG_BT_ATT_PREPARE_COUNT 64
#else
/**
* CONFIG_BT_ATT_PREPARE_COUNT: Number of buffers available for ATT prepare write, setting
* this to 0 disables GATT long/reliable writes.
* range 0 to 64
*/
#ifndef CONFIG_BT_ATT_PREPARE_COUNT
#define CONFIG_BT_ATT_PREPARE_COUNT 0
#endif
#endif

/**
*  CONFIG_BLUETOOTH_SMP:Eable the Security Manager Protocol
*  (SMP), making it possible to pair devices over LE
*/
#ifdef CONFIG_BT_SMP
#undef CONFIG_BT_SMP
#define CONFIG_BT_SMP 1

/**
*  CONFIG_BT_SIGNING:enables data signing which is used for transferring
*  authenticated data in an unencrypted connection
*/
#ifdef CONFIG_BT_SIGNING
#undef CONFIG_BT_SIGNING
#define CONFIG_BT_SIGNING 1
#endif

/**
*  CONFIG_BT_SMP_SC_ONLY:enables support for Secure Connection Only Mode. In this
*  mode device shall only use Security Mode 1 Level 4 with exception
*  for services that only require Security Mode 1 Level 1 (no security).
*  Security Mode 1 Level 4 stands for authenticated LE Secure Connections
*  pairing with encryption. Enabling this option disables legacy pairing
*/
#ifdef CONFIG_BT_SMP_SC_ONLY
#undef CONFIG_BT_SMP_SC_ONLY
#define CONFIG_BT_SMP_SC_ONLY 1
#endif

#ifdef CONFIG_BT_FIXED_PASSKEY
#undef CONFIG_BT_FIXED_PASSKEY
#define CONFIG_BT_FIXED_PASSKEY 1
#endif

#ifdef CONFIG_BT_SMP_ENFORCE_MITM
#undef CONFIG_BT_SMP_ENFORCE_MITM
#define CONFIG_BT_SMP_ENFORCE_MITM 1
#endif

/**
*  CONFIG_BT_USE_DEBUG_KEYS:This option places Security Manager in
*  a Debug Mode. In this mode predefined
*  Diffie-Hellman private/public key pair is used as described
*  in Core Specification Vol. 3, Part H, 2.3.5.6.1. This option should
*  only be enabled for debugging and should never be used in production.
*  If this option is enabled anyone is able to decipher encrypted air
*  traffic.
*/
#ifdef CONFIG_BT_USE_DEBUG_KEYS
#ifndef CONFIG_BT_TINYCRYPT_ECC
#error "CONFIG_BT_USE_DEBUG_KEYS depends on CONFIG_BT_TINYCRYPT_ECC"
#endif
#undef CONFIG_BT_USE_DEBUG_KEYS
#define CONFIG_BT_USE_DEBUG_KEYS 1
#endif

#endif //#ifdef CONFIG_BT_SMP

/**
*  CONFIG_BT_L2CAP_DYNAMIC_CHANNEL:enables support for LE Connection
*  oriented Channels,allowing the creation of dynamic L2CAP Channels
*/
#ifdef CONFIG_BT_L2CAP_DYNAMIC_CHANNEL
#undef CONFIG_BT_L2CAP_DYNAMIC_CHANNEL
#define CONFIG_BT_L2CAP_DYNAMIC_CHANNEL 1
#endif

/**
*   CONFIG_BT_PRIVACY:Enable local Privacy Feature support. This makes it possible
*   to use Resolvable Private Addresses (RPAs).
*/
#ifdef CONFIG_BT_PRIVACY
#ifndef CONFIG_BT_SMP
#error "CONFIG_BT_PRIVACY depends on CONFIG_BT_SMP"
#endif
#undef CONFIG_BT_PRIVACY
#define CONFIG_BT_PRIVACY 1

/**
* CONFIG_BT_RPA_TIMEOUT:Resolvable Private Address timeout
* range 1 to 65535,seconds
*/
#ifndef CONFIG_BT_RPA_TIMEOUT
#if defined(CONFIG_AUTO_PTS) ||defined(CONFIG_BT_STACK_PTS)
#define CONFIG_BT_RPA_TIMEOUT 60
#else
#define CONFIG_BT_RPA_TIMEOUT 900
#endif
#endif
#endif

/**
*  CONFIG_BT_GATT_DYNAMIC_DB:enables GATT services to be added dynamically to database
*/
#ifdef CONFIG_BT_GATT_DYNAMIC_DB
#undef CONFIG_BT_GATT_DYNAMIC_DB
#define CONFIG_BT_GATT_DYNAMIC_DB 1
#else
#define CONFIG_BT_GATT_DYNAMIC_DB 1
#endif

/**
*  CONFIG_BT_GATT_CLIENT:GATT client role support
*/
#ifdef CONFIG_BT_GATT_CLIENT
#undef CONFIG_BT_GATT_CLIENT
#define CONFIG_BT_GATT_CLIENT 1
#endif

/**
*  CONFIG_BT_MAX_PAIRED:Maximum number of paired devices
*  range 1 to 128
*/
#ifndef CONFIG_BT_MAX_PAIRED
#define CONFIG_BT_MAX_PAIRED  CONFIG_BT_MAX_CONN
#endif
#endif

/**
* If this option is set TinyCrypt library is used for emulating the
* ECDH HCI commands and events needed by e.g. LE Secure Connections.
* In builds including the BLE Host, if not set the controller crypto is
* used for ECDH and if the controller doesn't support the required HCI
* commands the LE Secure Connections support will be disabled.
* In builds including the HCI Raw interface and the BLE Controller, this
* option injects support for the 2 HCI commands required for LE Secure
* Connections so that Hosts can make use of those
*/
#ifdef CONFIG_BT_TINYCRYPT_ECC
#undef CONFIG_BT_TINYCRYPT_ECC
#define CONFIG_BT_TINYCRYPT_ECC 1
#endif
/**
*  CONFIG_BLUETOOTH_MAX_CONN:Maximum number of connections
*  range 1 to 128
*/
#ifndef CONFIG_BT_MAX_CONN
#define CONFIG_BT_MAX_CONN CFG_CON
#endif

/*If the application layer sends a att reqeust packet simultaneously for each ble connection, 
*and receives att reqeust packets at the same time, each connection requires 2 acl tx buffers.
*/
#if (CONFIG_BT_MAX_CONN <= 3 && CONFIG_BT_L2CAP_TX_BUF_COUNT < 2 * CONFIG_BT_MAX_CONN)
    #error "Case1, number of acl tx buffers in blestack is too small."
#elif (CONFIG_BT_MAX_CONN >= 4 && CONFIG_BT_MAX_CONN <= 6 && CONFIG_BT_L2CAP_TX_BUF_COUNT < 2 + CONFIG_BT_MAX_CONN)
    #error "Case2, number of acl tx buffers in blestack is too small."
#elif (CONFIG_BT_MAX_CONN >= 7 && CONFIG_BT_L2CAP_TX_BUF_COUNT < CONFIG_BT_MAX_CONN)
    #error "Case3, number of acl tx buffers in blestack is too small."
#endif 

/**
*  CONFIG_BT_DEVICE_NAME:Bluetooth device name. Name can be up
*  to 248 bytes long (excluding NULL termination). Can be empty string
*/
#ifndef CONFIG_BT_DEVICE_NAME
#if defined(CONFIG_AUTO_PTS)
#define CONFIG_BT_DEVICE_NAME "AUTO_PTS_TEST0123456789012345"
#else
#if defined(BL602)
#define CONFIG_BT_DEVICE_NAME "BL602-BLE-DEV"
#elif defined(BL702)
#define CONFIG_BT_DEVICE_NAME "BL702-BLE-DEV"
#elif defined(BL702L)
#define CONFIG_BT_DEVICE_NAME "BL702L-BLE-DEV"
#else
#define CONFIG_BT_DEVICE_NAME "BTBLE-DEV"
#endif
#endif
#endif
/**
*  CONFIG_BT_CONTROLLER_NAME:Bluetooth controller name.
*/
#ifndef CONFIG_BT_CONTROLLER_NAME
#if defined(BL602)
#define CONFIG_BT_CONTROLLER_NAME "BL602-BLE-DEV"
#else
#define CONFIG_BT_CONTROLLER_NAME "BL702-BLE-DEV"
#endif
#endif

/**
*  CONFIG_BT_MAX_SCO_CONN:Maximum number of simultaneous SCO connections.
*/
#ifndef CONFIG_BT_MAX_SCO_CONN
#define CONFIG_BT_MAX_SCO_CONN CONFIG_MAX_SCO
#endif

/**
*  CONFIG_BT_WORK_QUEUE_STACK_SIZE:Work queue stack size.
*/
#ifndef CONFIG_BT_WORK_QUEUE_STACK_SIZE
#ifndef CONFIG_BT_MESH
#if defined(CFG_IOT_SDK) || defined(BL_MCU_SDK)
#define CONFIG_BT_WORK_QUEUE_STACK_SIZE 1536//1280//512
#else
#define CONFIG_BT_WORK_QUEUE_STACK_SIZE 1536+512//1280//512
#endif /*CFG_IOT_SDK || BL_MCU_SDK*/
#else
#if !defined(CONFIG_BT_CONN)
#define CONFIG_BT_WORK_QUEUE_STACK_SIZE 1024
#else
#define CONFIG_BT_WORK_QUEUE_STACK_SIZE 2048
#endif /* CONFIG_BT_MESH */
#endif
#endif

/**
*  CONFIG_BT_WORK_QUEUE_PRIO:Work queue priority.
*/
#ifndef CONFIG_BT_WORK_QUEUE_PRIO
#define CONFIG_BT_WORK_QUEUE_PRIO (configMAX_PRIORITIES - 2)
#endif

/**
*  CONFIG_BT_HCI_RESERVE:Headroom that the driver needs for sending and receiving buffers.
*/
#ifndef CONFIG_BT_HCI_RESERVE
#ifdef CONFIG_BLUETOOTH_H4
#define CONFIG_BT_HCI_RESERVE 0
#elif defined(CONFIG_BLUETOOTH_H5) || defined(CONFIG_BLUETOOTH_SPI)
#define CONFIG_BT_HCI_RESERVE 1
#else
#define CONFIG_BT_HCI_RESERVE 1
#endif
#endif

/**
*  CONFIG_BLUETOOTH_DEBUG_LOG:Enable bluetooth debug log.
*/
#if defined(BFLB_BLE)
#if defined(CFG_BLE_STACK_DBG_PRINT)
#undef CONFIG_BT_DEBUG
#define CONFIG_BT_DEBUG 1
#endif
#else
#ifdef CONFIG_BT_DEBUG_LOG
#undef CONFIG_BT_DEBUG_LOG
#define CONFIG_BT_DEBUG_LOG 1
#undef CONFIG_BT_DEBUG
#define CONFIG_BT_DEBUG 1
#endif
#endif
/**
*  CONFIG_BT_DEBUG_L2CAP:Enable bluetooth l2cap debug log.
*/
#ifdef CONFIG_BT_DEBUG_L2CAP
#undef CONFIG_BT_DEBUG_L2CAP
#define CONFIG_BT_DEBUG_L2CAP 1
#endif

/**
*  CONFIG_BT_DEBUG_CONN:Enable bluetooth conn debug log.
*/
#ifdef CONFIG_BT_DEBUG_CONN
#undef CONFIG_BT_DEBUG_CONN
#define CONFIG_BT_DEBUG_CONN 1
#endif

/**
*  CONFIG_BT_DEBUG_ATT:Enable bluetooth att debug log.
*/
#ifdef CONFIG_BT_DEBUG_ATT
#undef CONFIG_BT_DEBUG_ATT
#define CONFIG_BT_DEBUG_ATT 1
#endif

/**
*  CONFIG_BT_DEBUG_GATT:Enable bluetooth gatt debug log.
*/
#ifdef CONFIG_BT_DEBUG_GATT
#undef CONFIG_BT_DEBUG_GATT
#define CONFIG_BT_DEBUG_GATT 1
#endif

/**
*  CONFIG_BT_DEBUG_HCI_CORE:Enable bluetooth hci core debug log.
*/
#ifdef CONFIG_BT_DEBUG_HCI_CORE
#undef CONFIG_BT_DEBUG_HCI_CORE
#define CONFIG_BT_DEBUG_HCI_CORE 1
#endif

/**
*  CONFIG_BT_DEBUG_HCI_DRIVER:Enable bluetooth hci driver debug log.
*/
#ifdef CONFIG_BT_DEBUG_HCI_DRIVER
#undef CONFIG_BT_DEBUG_HCI_DRIVER
#define CONFIG_BT_DEBUG_HCI_DRIVER 1
#endif

/**
*  CONFIG_BT_TEST:Enable bluetooth test.
*/
#ifdef CONFIG_BT_TEST
#undef CONFIG_BT_TEST
#define CONFIG_BT_TEST 1
#endif

/**
*  CONFIG_BT_DEBUG_CORE:Enable bluetooth core debug log.
*/
#ifdef CONFIG_BT_DEBUG_CORE
#undef CONFIG_BT_DEBUG_CORE
#define CONFIG_BT_DEBUG_CORE 1
#endif

#ifndef CONFIG_BT_ATT_TX_MAX
/*
*  Take throuthput test into consideration, set att tx max the same with lowstack tx buffer count.
*  att semaphore determine the max numble packets can send to lowsatck at once.
*/
#define CONFIG_BT_ATT_TX_MAX 10
#endif

#ifndef CONFIG_BT_CONN_TX_MAX
/*
*  Take throuthput test into consideration, set upperstack conn tx max the same with lowstack tx buffer count.
*/
#define CONFIG_BT_CONN_TX_MAX 10
#endif

#ifndef CONFIG_BT_DEVICE_APPEARANCE
#define CONFIG_BT_DEVICE_APPEARANCE 833
#endif

#if defined(BFLB_BLE)
#ifndef CONFIG_BT_RECV_IS_RX_THREAD
#define CONFIG_BT_RECV_IS_RX_THREAD
#endif

#ifndef CONFIG_NET_BUF_USER_DATA_SIZE
#define CONFIG_NET_BUF_USER_DATA_SIZE 10
#endif

#ifndef CONFIG_BT_ID_MAX
#define CONFIG_BT_ID_MAX 1
#endif

#ifndef CONFIG_BT_REMOTE_VERSION
#define CONFIG_BT_REMOTE_VERSION 0
#endif

//#define PTS_GAP_SLAVER_CONFIG_NOTIFY_CHARC 1

#ifndef CONFIG_BT_L2CAP_TX_FRAG_COUNT
#define CONFIG_BT_L2CAP_TX_FRAG_COUNT 0
#endif

#ifndef CONFIG_BT_DEVICE_NAME_DYNAMIC
#define CONFIG_BT_DEVICE_NAME_DYNAMIC 1
#endif

// max lenght of ADV payload is 37 bytes (by BT core spec)
// AdvA field takes up 6 bytes
// if only Local Name is appended, then max lenght of Local Name shall be
// 37-6-2=31 bytes, where UUID of Local Name takes up 2 bytes
#define CONFIG_BT_DEVICE_NAME_MAX 29

#define CONFIG_BT_PERIPHERAL_PREF_MIN_INT 0x0024
#define CONFIG_BT_PERIPHERAL_PREF_MAX_INT 0x0028
#define CONFIG_BT_PERIPHERAL_PREF_SLAVE_LATENCY 0
#define CONFIG_BT_PERIPHERAL_PREF_TIMEOUT 500

#ifndef CONFIG_BT_PHY_UPDATE
#define CONFIG_BT_PHY_UPDATE 1
#endif

#if defined(CONFIG_BT_BREDR)
#define CONFIG_BT_PAGE_TIMEOUT 0x2000 //5.12s
#define CONFIG_BT_L2CAP_RX_MTU 672

#ifndef CONFIG_BT_RFCOMM_TX_STACK_SIZE
#define CONFIG_BT_RFCOMM_TX_STACK_SIZE  1024
#endif
#ifndef CONFIG_BT_RFCOMM_TX_PRIO
#define CONFIG_BT_RFCOMM_TX_PRIO (configMAX_PRIORITIES - 5)
#endif

#define PCM_PRINTF 0
#define DISABLE_BREDR_INVALID_ENC_KEY_TEST 1
#endif

#if defined(CONFIG_BT_AUDIO)
#define CONFIG_BT_MAX_ISO_CONN 8  //range 1 to 64

#endif

/*******************************Bouffalo Lab Modification******************************/

//#define BFLB_BLE_DISABLE_STATIC_ATTR
//#define BFLB_BLE_DISABLE_STATIC_CHANNEL

#define BFLB_DISABLE_BT
#define BFLB_FIXED_IRK 0
#define BFLB_DYNAMIC_ALLOC_MEM
#define BFLB_BT_LINK_KEYS_STORE
#if defined(BL702) || defined(BL702L)
#if defined(CFG_BLE_PDS) && defined(BFLB_BLE) && defined(BFLB_DYNAMIC_ALLOC_MEM)&& !defined(CONFIG_BT_MESH)
#define BFLB_STATIC_ALLOC_MEM   1
#else
#define BFLB_STATIC_ALLOC_MEM   0
#endif
#else
#define BFLB_STATIC_ALLOC_MEM   0
#endif
#define CONFIG_BT_SCAN_WITH_IDENTITY 1
#define CONFIG_BT_DEVICE_NAME_GATT_WRITABLE
#define CONFIG_BT_GAP_APPEARANCE_WRITABLE

#if defined(CONFIG_AUTO_PTS)
#ifdef BFLB_FIXED_IRK
#undef BFLB_FIXED_IRK
#define BFLB_FIXED_IRK 1
#endif
#define CONFIG_BT_L2CAP_DYNAMIC_CHANNEL
#ifdef CONFIG_BT_DEVICE_NAME_GATT_WRITABLE
#undef CONFIG_BT_DEVICE_NAME_GATT_WRITABLE
#define CONFIG_BT_DEVICE_NAME_GATT_WRITABLE 1
#endif 
#define CONFIG_BT_GATT_SERVICE_CHANGED 1
//#define CONFIG_BT_GATT_CACHING 1
#define CONFIG_BT_SCAN_WITH_IDENTITY 1
//#define CONFIG_BT_ADV_WITH_PUBLIC_ADDR 1
#ifdef CONFIG_BT_ATT_PREPARE_COUNT
#undef CONFIG_BT_ATT_PREPARE_COUNT
#define CONFIG_BT_ATT_PREPARE_COUNT 64
#endif
#define CONFIG_BT_TESTING 1
#define CONFIG_BT_GATT_READ_MULTIPLE 1
#define CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS 1
#endif
#endif //BFLB_BLE

/*******************************Bouffalo Lab Patch******************************/
/*Fix the issue that DHKEY_check_failed error happens in smp procedure if CONFIG_BT_PRIVACY is enabled.*/
#define BFLB_BLE_PATCH_DHKEY_CHECK_FAILED
/*To notify upper layer that write_ccc is completed*/
#define BFLB_BLE_PATCH_NOTIFY_WRITE_CCC_RSP
/*Timer/Queue/Sem allocated during connection establishment is not released when disconnection
happens, which cause memory leak issue.*/
#define BFLB_BLE_PATCH_FREE_ALLOCATED_BUFFER_IN_OS
/*To avoid duplicated pubkey callback.*/
#define BFLB_BLE_PATCH_AVOID_DUPLI_PUBKEY_CB
/*Cancle att->timeout work if att_req is cancelled to avoid att timeout.*/
#define BFLB_BLE_PATCH_CANCEL_ATT_TIMEOUT_TIMER_WHEN_ATT_REQ_CANCELLED
/* Fix call bt_conn_disconnect->bt_hci_disconnect->bt_hci_cmd_send_sync, the current task yeild.
 * When disconnect done and conn->state change to BT_CONN_DISCONNECTED, current task retore.
 * Then bt_conn_set_state called with parameter BT_CONN_DISCONNECT. the conn->ref shall be error.
 */
#define BFLB_BLE_PATCH_DISCONNECT_ERROR_WHEN_TASK_YEILD
#if defined(CONFIG_BT_GATT_DYNAMIC_DB)
#define BFLB_BLE_DYNAMIC_SERVICE
#endif
#if !defined(CONFIG_AUTO_PTS)
/*To avoid sevice changed indication sent at the very beginning, without any new service added.*/
#define BFLB_BLE_PATCH_SET_SCRANGE_CHAGD_ONLY_IN_CONNECTED_STATE
#endif
#ifdef CONFIG_BT_SETTINGS
/*Semaphore is used during flash operation. Make sure that freertos has already run up when it
  intends to write information to flash.*/
#define BFLB_BLE_PATCH_SETTINGS_LOAD
#endif
#define BFLB_BLE_PATCH_AVOID_CONN_UPDATE_WHEN_PREVIOUS_IS_NOT_OVER
#ifdef CONFIG_BT_SMP
#define BFLB_BLE_SMP_LOCAL_AUTH
#define BFLB_BLE_SMP_SUPPORT_DISABLE_PAIR
#endif
#define BFLB_BLE_MTU_CHANGE_CB
#if defined(CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS)
#define BFLB_BLE_GAP_SET_PERIPHERAL_PREF_PARAMS
#endif

#if defined(CFG_BT_RESET)
#define BFLB_HOST_ASSISTANT
#endif

#define BFLB_RELEASE_CMD_SEM_IF_CONN_DISC
/*Fix the issue when local auth_req is 0(no boinding), 
BT_SMP_DIST_ENC_KEY bit is not cleared while remote ENC_KEY is received.*/
#define BFLB_BLE_PATCH_CLEAR_REMOTE_KEY_BIT

#if defined(CONFIG_BT_CENTRAL) || defined(CONFIG_BT_OBSERVER)
#if defined(BL602) || defined(BL702)
#define BFLB_BLE_NOTIFY_ADV_DISCARDED
#endif
#endif

#if defined(CONFIG_BT_CENTRAL)
#define BFLB_BLE_NOTIFY_ALL
#define BFLB_BLE_DISCOVER_ONGOING
#endif

#define BFLB_BLE_SET_LOCAL_ATT_MTU_SIZE
#if defined(CONFIG_BT_SMP)
#define BFLB_BLE_SMP_SC_ONLY
#endif

/* Fix do att_send_req but att->tx_sem is busy, the request will push into att->tx_queue, when
 * pop att->tx_queue and send request, att_send not ref req->buf, this will cause buf to be
 * released early.
 */
#define BFLB_BLE_PATCH_ATT_SEND_REQ_WHEN_TX_SEM_BUSY_BUF_ERR

#define BFLB_BLE_ENABLE_TEST_PSM 0
#define BFLB_BREDR_SCO_TYPE_FIX
#define BFLB_BLE_PATCH_FORCE_UPDATE_GAP_DEVICE_NAME
#define BFLB_BLE_PATCH_ADD_ERRNO_IN_DISCOVER_CALLBACK
#define BFLB_BLE_PATCH_NOT_DO_TX_IF_DISCONNECTED
/* Fix the issue that sc(gatt service chagned configuration) cannot be saved because of sc entries are all used.
 * Root cause: the sc entries configured by unbonded peer devices are not cleared.
 * Fix: Clear the sc entries configred by unbonded peer devices if those peer devices are disconnected.
 */
#define BFLB_BLE_PATCH_CLEAR_UNBONDED_DEVICE_SC_WHEN_DISCONNECTED
/* Fix the issue that gatt configurations are not cleared even if the peer devices are unpaired.
 * Root cause: 1.when CONFIG_BT_SETTINGS is not enabled, bt_gatt_clear is not called to clear the configurations.
 * 2. Unpaired device's address is cleared before doing bt_gatt_clear, then gatt configurations cannot be found.
 * Fix: 1.Whether CONFIG_BT_SETTINGS is enabled or not, clear the configurations when doing unpair.
 * 2. When doing unpair, clear gatt congigurations before clearing the peer device address.
 */
#define BFLB_BLE_PATCH_DO_GATT_CLEAR_IF_UNPAIRED
#define BFLB_BLE_PATCH_SC_CFG_FAIL_TO_BE_REMOVED_FROM_FLASH
#define BFLB_BLE_PATCH_AVOID_NEXT_ATT_REQ_SENT_BEFORE_PREVIOUS_ATT_RSP_RCVD
/*Fix issue in two cases:
 * 1.upper layer calls bt_conn_create_le, host receives target's adv, and sends stop ble scan hci cmd,then recv_thread is blocked. At that time, upper layer calls bt_conn_disconnect
 * in cli task, bt_conn_disconnect updates the conn->state to BT_CONN_DISCONNECTED from BT_CONN_CONNECT_SCAN state, and reduce conn->ref. But actally this disconnect operation doesn't
 * stop the connection procedure. Connection is established successfully. After 5s,upper layer calls bt_conn_disconnect again to disconnect the connection, then assert happens in
 * bt_conn_unref because conn->ref is reduced when uppper layer calls bt_conn_disconnect at the first time.
 *
 * 2.upper layer calls bt_conn_create_le. Before host receives target's adv, upper layer calls bt_conn_disconnect in a lower task. Before conn->state is updated to BT_CONN_DISCONNECTED from
 * BT_CONN_CONNECT_SCAN, host receives target's adv in recv_thread and bt_conn_disconnect is interrupted. After recv_thread is blocked, bt_conn_disconnect continues to work, it updates
 * the conn->state to BT_CONN_DISCONNECTED from BT_CONN_CONNECT, and reduce conn->ref. But actally this disconnect operation doesn't
 * stop the connection procedure. Connection is established successfully. After 5s,upper layer calls bt_conn_disconnect again to disconnect the connection, then assert happens in
 * bt_conn_unref because conn->ref is reduced when uppper layer calls bt_conn_disconnect at the first time.
*/
#define BFLB_BLE_PATCH_AVOID_CONNECT_DISCONNECT_RISK
/* Fix the issue:upper layer calls bt_conn_disconnect in a lower task. Sometimes, hci_disconnect_complete event comes while the processing in bt_conn_set_state to
change conn->state to disconnect is not completed. conn->state is changed to disconnected when handing hci_disconnect_complete event. Once hci_disconnect_complete event
processing is completed, it continues the processing in bt_conn_set_state to change conn_state to disconnect. Because conn->state has been changed to discconnected,
then it does disconnected flow once more. This will cause hardfault issue because some resourse has been released in previouse disconnected flow.
(We see conn_cleanup is called twice).
*/
#define BFLB_BLE_PATCH_FIX_CONN_STATE_CHANGE_RISK
#define BFLB_BLE_PATCH_FREE_CONN_UPDATE_WORK_WHEN_CANCEL_CONN_IN_CONNECT_STATE
#define BFLB_BLE_AUTO_CLEAN_KEY_WHEN_KEY_MISSING
#define BFLB_BLE_AUTO_CANCEL_RELIABLE_WRITE_CHARACTERISTIC
#define BFLB_BLE_FREE_CONN_UPDATE_WORK_WHEN_DISCONNECT_IN_CONN_SCAN_STATE
#define BFLB_BLE_RESTRICT_CONN_ACTION_NOT_EXCEED_MAX_CONN
/* If there are multiple subscriptions, and host's acl tx buffer is not enough, it will block receive task to wait for host's 
 * available tx buffer with timout 30s, this make receive task cannot handle att response for last write ccc req to release tx buffer. 
 * ATT_TIMEOUT will happen because there is no available tx buffer.
 * Make application layer to do this subscriptions.
*/
#define BFLB_BLE_NOT_FORCE_WRITE_CCC_FOR_EXISTED_SUBSCRIPTIONS_IN_STACK
#if defined (CONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS)
#define BFLB_BLE_ENABLE_OR_DISABLE_SLAVE_PREF_CONN_PARAM_UDPATE
#endif
#define BFLB_BLE_DO_DISCONNECT_WHEN_ATT_TIMEOUT
#define BFLB_BLE_AVOID_REMOVE_GATT_SUBSCRIPTION_RISK

/*Support customized scan interval and scan window in scanning procedure of ble general connection establishment.*/
#define BFLB_BLE_SUPPORT_CUSTOMIZED_SCAN_PARAMERS_IN_GENERAL_CONN_ESTABLISH

/* Fix the issue : it excutes bt_conn_create_le, bt_le_scan_start, bt_le_scan_stop sequently. After stop scan ,it takes a long time to establish le connection.
 * Reason: When it stops scan in this case, it restarts scan if there is a connection's state is BT_CONN_CONNECT_SCAN, but it uses background scan parameters
 * to do slowly scan even if it is not auto connection without whitelist.
 * Notice:If Auto conn initiated by bt_le_set_auto_conn,it doesn't create le connection from whitelist.It needs scan first.
*/
#define BFLB_BLE_NOT_USE_BACKGROUD_SCAN_PARAMETERS_IF_NOT_ATUO_CONN_WITHOUT_WHITELIST
#define BFLB_BLE_AVOID_WORKQ_TIMER_CANCEL_RISK
#if !defined(CONFIG_BT_HOST_HCI_TL)
#define BFLB_BLE_NOT_ALLOCATE_RX_NETBUF_FOR_NUM_OF_COMPLETED_PKTS_EVT
#endif
/*Fix the issue:hci_tx_thread blocked by other sem, not by g_poll_sem, then conn_cleanup is delayed to be handled. When conn_update_timer expires,
 *conn_update_timeout will be called, and clean the ref,conn->ref = 0. Once hci_tx_thread runs, in bt_conn_prepare_events,it doesn't
 *excute conn_cleanup if conn->ref is 0.This will cause memory leak issue.
*/
#define BFLB_BLE_PATCH_AVOID_CONN_CLEANUP_FAILED_EXCUTED_RISK

/* Fix the issue that bt_conn_create_le (to the samve address) is called before hci_disconn_complete fully completed, the new ref is not start from 0.*/
#define BFLB_BLE_PATCH_CONN_CREATE_LE_BEFORE_DISCONN_FULLY_COMPLETE_RISK

/* Fix the issue that conn_new reentry before atomic_set is completed, then different connections may be use the same conns[i]*/
#define BFLB_BLE_PATCH_CONN_NEW_REENTRY_RISK

#endif /* BLE_CONFIG_H */
