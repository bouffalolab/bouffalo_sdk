/**
 *
 * Copyright (c) 2022  Alibaba Group Holding Limited
 *
 */

#ifndef __BT_STACK_H__
#define __BT_STACK_H__

#include <stdint.h>
#include <xutils/list.h>
#include <bt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* class of device definition */
/*
| octer  |                 1               |              2            |          3           |
| bit    | 0 ~ 1 |        2 ~ 7            |         8 ~ 12         |        13 ~ 23          |
| value  |   0   | BT_STACK_CODE_MINOR_XXX | BT_STACK_COD_MAJOR_XXX | BT_STACK_COD_SERVICE_XXX|
*/
/// Major Service Classes
#define BT_STACK_COD_SERVICE_LIMIT_DISCOVERY_MODE (0x01 << 0) /*!< Limited Discoverable Mode */
#define BT_STACK_COD_SERVICE_LE_AUDIO             (0x01 << 1) /*!< LE Audio */
#define BT_STACK_COD_SERVICE_RSV                  (0x01 << 2) /*!< reserved */
#define BT_STACK_COD_SERVICE_POSITIONING          (0x01 << 3) /*!< Positioning (Location identification) */
#define BT_STACK_COD_SERVICE_NETWORKING           (0x01 << 4) /*!< Networking (LAN, Ad hoc, …) */
#define BT_STACK_COD_SERVICE_RENDERING            (0x01 << 5) /*!< Rendering (Printing, Speakers, …) */
#define BT_STACK_COD_SERVICE_CAPTURING            (0x01 << 6) /*!< Capturing (Scanner, Microphone, …) */
#define BT_STACK_COD_SERVICE_OBJECT_TRANSFER      (0x01 << 7) /*!< Object Transfer (v-Inbox, v-Folder, …) */
#define BT_STACK_COD_SERVICE_AUDIO                (0x01 << 8) /*!< Audio (Speaker, Microphone, Headset service, …) */
#define BT_STACK_COD_SERVICE_TELEPHONY            (0x01 << 9) /*!< Telephony (Cordless telephony, Modem, Headset service, …) */
#define BT_STACK_COD_SERVICE_INFOMATION           (0x01 << 10) /*!< Information (WEB-server, WAP-server, …) */

/// Major Device Classes
#define BT_STACK_COD_MAJOR_MISC          (0x00) /*!< Miscellaneous */
#define BT_STACK_COD_MAJOR_COMPUTER      (0x01) /*!< Computer (desktop, notebook, PDA, organizer, … ) */
#define BT_STACK_COD_MAJOR_PHONE         (0x02) /*!< Phone (cellular, cordless, pay phone, modem, …) */
#define BT_STACK_COD_MAJOR_NET           (0x03) /*!< LAN /Network Access point */
#define BT_STACK_COD_MAJOR_AV            (0x04) /*!< Audio/Video (headset, speaker, stereo, video display, VCR, … */
#define BT_STACK_COD_MAJOR_PREIPHERAL    (0x05) /*!< Peripheral (mouse, joystick, keyboard, … ) */
#define BT_STACK_COD_MAJOR_IMG           (0x06) /*!< Imaging (printer, scanner, camera, display, …) */
#define BT_STACK_COD_MAJOR_WEARABLE      (0x07) /*!< Wearable */
#define BT_STACK_COD_MAJOR_TOY           (0x08) /*!< Toy */
#define BT_STACK_COD_MAJOR_HEALTH        (0x09) /*!< Health */
#define BT_STACK_COD_MAJOR_UNCATEGORIZED (0x1f) /*!< Uncategorized: device code not specified */

/// The Minor Device Class field - Computer Major Class
#define BT_STACK_CODE_MINOR_COMPUTER_UNCATEGORIZED (0x00) /*!< Uncategorized, code for device not assigned */
#define BT_STACK_CODE_MINOR_COMPUTER_DESKTOP       (0x01) /*!< Desktop workstation */
#define BT_STACK_CODE_MINOR_COMPUTER_SERVER_CLASS  (0x02) /*!< Server-class computer */
#define BT_STACK_CODE_MINOR_COMPUTER_LAPTOP        (0x03) /*!< Laptop */
#define BT_STACK_CODE_MINOR_COMPUTER_HANDHELD      (0x04) /*!< Handheld PC/PDA (clamshell) */
#define BT_STACK_CODE_MINOR_COMPUTER_PALM          (0x05) /*!< Palm-size PC/PDA */
#define BT_STACK_CODE_MINOR_COMPUTER_WEARABLE      (0x06) /*!< Wearable computer (watch size) */
#define BT_STACK_CODE_MINOR_COMPUTER_TABLET        (0x07) /*!< Tablet */

/// Minor Device Class field – Phone Major Class
#define BT_STACK_CODE_MINOR_PHONE_UNCATEGORIZED (0x00) /*!< Uncategorized, code for device not assigned */
#define BT_STACK_CODE_MINOR_PHONE_CELLULAR      (0x01) /*!< Cellular */
#define BT_STACK_CODE_MINOR_PHONE_CORDLESS      (0x02) /*!< Cordless */
#define BT_STACK_CODE_MINOR_PHONE_SMART         (0x03) /*!< Smartphone */
#define BT_STACK_CODE_MINOR_PHONE_WIRED         (0x04) /*!< Wired modem or voice gateway */
#define BT_STACK_CODE_MINOR_PHONE_COMMON_ISDN   (0x05) /*!< Common ISDN access */

/// Minor Device Class field – LAN/Network Access Point Major Class
#define BT_STACK_CODE_MINOR_NET_FULL  (0x00 << 3) /*!< Fully available */
#define BT_STACK_CODE_MINOR_NET_1_17  (0x01 << 3) /*!< 1% to 17% utilized */
#define BT_STACK_CODE_MINOR_NET_17_33 (0x02 << 3) /*!< 17% to 33% utilized */
#define BT_STACK_CODE_MINOR_NET_33_50 (0x03 << 3) /*!< 33% to 50% utilized */
#define BT_STACK_CODE_MINOR_NET_50_67 (0x04 << 3) /*!< 50% to 67% utilized */
#define BT_STACK_CODE_MINOR_NET_67_83 (0x05 << 3) /*!< 67% to 83% utilized */
#define BT_STACK_CODE_MINOR_NET_83_99 (0x06 << 3) /*!< 83% to 99% utilized */
#define BT_STACK_CODE_MINOR_NET_NONE  (0x07 << 3) /*!< No service available */

/// Minor Device Class field – Audio/Video Major Class
#define BT_STACK_CODE_MINOR_AV_UNCATEGORIZED             (0x00) /*!< Uncategorized, code not assigned */
#define BT_STACK_CODE_MINOR_AV_WEARABLE_HEADSET          (0x01) /*!< Wearable Headset Device */
#define BT_STACK_CODE_MINOR_AV_HANDSFREE                 (0x02) /*!< Hands-free Device */
#define BT_STACK_CODE_MINOR_AV_RSV1                      (0x03) /*!< (Reserved) */
#define BT_STACK_CODE_MINOR_AV_MICROPHONE                (0x04) /*!< Microphone */
#define BT_STACK_CODE_MINOR_AV_LOUDSPEAKER               (0x05) /*!< Loudspeaker */
#define BT_STACK_CODE_MINOR_AV_HEADPHONES                (0x06) /*!< Headphones */
#define BT_STACK_CODE_MINOR_AV_PORTABLE_AUDIO            (0x07) /*!< Portable Audio */
#define BT_STACK_CODE_MINOR_AV_CAR_AUDIO                 (0x08) /*!< Car audio */
#define BT_STACK_CODE_MINOR_AV_SETTOP_BOX                (0x09) /*!< Set-top box */
#define BT_STACK_CODE_MINOR_AV_HIFI                      (0x0a) /*!< HiFi Audio Device */
#define BT_STACK_CODE_MINOR_AV_VCR                       (0x0b) /*!< VCR */
#define BT_STACK_CODE_MINOR_AV_VIDEO                     (0x0c) /*!< Video Camera */
#define BT_STACK_CODE_MINOR_AV_CAMCORDER                 (0x0d) /*!< Camcorder */
#define BT_STACK_CODE_MINOR_AV_VIDEO_MONITOR             (0x0e) /*!< Video Monitor */
#define BT_STACK_CODE_MINOR_AV_VIDIO_DISPLAY_LOUDSPEAKER (0x0f) /*!< Video Display and Loudspeaker */
#define BT_STACK_CODE_MINOR_AV_VIDEO_CONFERENCING        (0x10) /*!< Video Conferencing */
#define BT_STACK_CODE_MINOR_AV_RSV2                      (0x11) /*!< (Reserved) */
#define BT_STACK_CODE_MINOR_AV_GAMEING_TOY               (0x12) /*!< Gaming/Toy */

/// Minor Device Class field – Peripheral Major Class
/*
| bit    |          0 ~ 3                       |                 4 ~ 5                |
| value  | BT_STACK_CODE_MINOR_PREIPHERAL_XXX_L | BT_STACK_CODE_MINOR_PREIPHERAL_XXX_H |

BT_STACK_CODE_MINOR_PREIPHERAL_XXX = BT_STACK_CODE_MINOR_PREIPHERAL_XXX_H | BT_STACK_CODE_MINOR_PREIPHERAL_XXX_H
*/
#define BT_STACK_CODE_MINOR_PREIPHERAL_NONE_H     (0x00 << 6) /*!< Not Keyboard / Not Pointing Device */
#define BT_STACK_CODE_MINOR_PREIPHERAL_POINTING_H (0x01 << 6) /*!< Pointing device */
#define BT_STACK_CODE_MINOR_PREIPHERAL_KEYBOARD_H (0x02 << 6) /*!< Keyboard */
#define BT_STACK_CODE_MINOR_PREIPHERAL_COMBO_H    (0x03 << 6) /*!< Combo keyboard/pointing device */

#define BT_STACK_CODE_MINOR_PREIPHERAL_UNCATEGORIZED_L    (0x00) /*!< Uncategorized device  */
#define BT_STACK_CODE_MINOR_PREIPHERAL_JOYSTICK_L         (0x01) /*!< Joystick */
#define BT_STACK_CODE_MINOR_PREIPHERAL_GAMEPAD_L          (0x02) /*!< Gamepad */
#define BT_STACK_CODE_MINOR_PREIPHERAL_REMOTE_L           (0x03) /*!< Remote control */
#define BT_STACK_CODE_MINOR_PREIPHERAL_SENSING_L          (0x04) /*!< Sensing device */
#define BT_STACK_CODE_MINOR_PREIPHERAL_DIGITIZE_TABLET_L  (0x05) /*!< Digitizer tablet */
#define BT_STACK_CODE_MINOR_PREIPHERAL_CARD_READER_L      (0x06) /*!< Card Reader (e.g. SIM Card Reader) */
#define BT_STACK_CODE_MINOR_PREIPHERAL_DIGITAL_PEN_L      (0x07) /*!< Digital Pen */
#define BT_STACK_CODE_MINOR_PREIPHERAL_HANDHELD_SCANNER_L (0x08) /*!< Handheld scanner for bar-codes, RFID, etc. */
#define BT_STACK_CODE_MINOR_PREIPHERAL_HANDHELD_GESTURAL_L                                                             \
    (0x09) /*!< Handheld gestural input device (e.g., “wand” form factor) */

/// Minor Device Class field – Imaging Major Class
#define BT_STACK_CODE_MINOR_IMG_DISPLAY (0x01 << 4) /*!< Display */
#define BT_STACK_CODE_MINOR_IMG_CAMERA  (0x01 << 5) /*!< Camera */
#define BT_STACK_CODE_MINOR_IMG_SCANNER (0x01 << 6) /*!< Scanner */
#define BT_STACK_CODE_MINOR_IMG_PRINTER (0x01 << 7) /*!< Printer */

/// Minor Device Class field – Wearable Major Class
#define BT_STACK_CODE_MINOR_WEARABLE_WRISTWATCH (0x01) /*!< Wristwatch */
#define BT_STACK_CODE_MINOR_WEARABLE_PAGER      (0x02) /*!< Pager */
#define BT_STACK_CODE_MINOR_WEARABLE_JACKET     (0x03) /*!< Jacket */
#define BT_STACK_CODE_MINOR_WEARABLE_HELMET     (0x04) /*!< Helmet */
#define BT_STACK_CODE_MINOR_WEARABLE_GLASSES    (0x05) /*!< Glasses */

/// Minor Device Class field – Toy Major Class
#define BT_STACK_CODE_MINOR_TOY_ROBOT      (0x01) /*!< Robo */
#define BT_STACK_CODE_MINOR_TOY_VEHICLE    (0x02) /*!< Vehicl */
#define BT_STACK_CODE_MINOR_TOY_DOLL       (0x03) /*!< Doll / Action figur */
#define BT_STACK_CODE_MINOR_TOY_CONTROLLER (0x04) /*!< Controlle */
#define BT_STACK_CODE_MINOR_TOY_GAME       (0x05) /*!< Game */

/// Minor Device Class field – Health
#define BT_STACK_CODE_MINOR_HEALTH_UNDEFINED          (0x00) /*!< Undefined */
#define BT_STACK_CODE_MINOR_HEALTH_BLOOD              (0x01) /*!< Blood Pressure Monitor */
#define BT_STACK_CODE_MINOR_HEALTH_THERMOMETER        (0x02) /*!< Thermometer */
#define BT_STACK_CODE_MINOR_HEALTH_WEIGHING_SCALE     (0x03) /*!< Weighing Scale */
#define BT_STACK_CODE_MINOR_HEALTH_GLUCOSE_METER      (0x04) /*!< Glucose Meter */
#define BT_STACK_CODE_MINOR_HEALTH_PULSE_OXIMETER     (0x05) /*!< Pulse Oximeter */
#define BT_STACK_CODE_MINOR_HEALTH_HEART_RATE         (0x06) /*!< Heart/Pulse Rate Monitor */
#define BT_STACK_CODE_MINOR_HEALTH_DATA_DISP          (0x07) /*!< Health Data Display */
#define BT_STACK_CODE_MINOR_HEALTH_STEP_COUNT         (0x08) /*!< Step Counter */
#define BT_STACK_CODE_MINOR_HEALTH_BODY_COMPOSITION   (0x09) /*!< Body Composition Analyzer */
#define BT_STACK_CODE_MINOR_HEALTH_PEAK_FLOW          (0x0a) /*!< Peak Flow Monitor */
#define BT_STACK_CODE_MINOR_HEALTH_MEDICATION_MONITOR (0x0b) /*!< Medication Monitor */
#define BT_STACK_CODE_MINOR_HEALTH_KNEE_PROSTHESIS    (0x0c) /*!< Knee Prosthesis */
#define BT_STACK_CODE_MINOR_HEALTH_ANKLE_PROSTHESIS   (0x0d) /*!< Ankle Prosthesis */
#define BT_STACK_CODE_MINOR_HEALTH_GENERIC            (0x0e) /*!< Generic Health Manager */
#define BT_STACK_CODE_MINOR_HEALTH_PMD                (0x0f) /*!< Personal Mobility Device */

/// Class of device
typedef struct {
    uint32_t reserved_2 : 2;  /*!< undefined */
    uint32_t minor      : 6;  /*!< minor class */
    uint32_t major      : 5;  /*!< major class */
    uint32_t service    : 11; /*!< service class */
    uint32_t reserved_8 : 8;  /*!< undefined */
} bt_stack_cod_t;

/// Extended Inquiry Response data
typedef bt_data_t bt_eir_data_t;

/// Extended Inquiry Response data type
typedef bt_data_type_t bt_eir_data_type_t;

typedef union
{
    /**
     * @brief BT_STACK_AUTH_CMPL_EVT
     */
    struct auth_cmpl_param {
        bt_stack_status_t status;
        bt_dev_addr_t     bda;         /*!< remote bluetooth device address*/
        char *            device_name; /*!< device name */
    } auth_cmpl;                       /*!< authentication complete parameter struct */

    /**
     * @brief BT_STACK_PIN_REQ_EVT
     */
    struct pin_req_param {
        bt_dev_addr_t bda;          /*!< remote bluetooth device address*/
        int           min_16_digit; /*!< TRUE if the pin returned must be at least 16 digits */
    } pin_req;                      /*!< pin request parameter struct */

    /**
     * @brief BT_STACK_CFM_REQ_EVT
     */
    struct cfm_req_param {
        bt_dev_addr_t bda;     /*!< remote bluetooth device address*/
        uint32_t      num_val; /*!< the numeric value for comparison. */
    } cfm_req;                 /*!< confirm request parameter struct */

    /**
     * @brief BT_STACK_KEY_NOTIF_EVT
     */
    struct key_notif_param {
        bt_dev_addr_t bda;     /*!< remote bluetooth device address*/
        uint32_t      passkey; /*!< the numeric value for passkey entry. */
    } key_notif;               /*!< passkey notif parameter struct */

    /**
     * @brief BT_STACK_KEY_REQ_EVT
     */
    struct key_req_param {
        bt_dev_addr_t bda; /*!< remote bluetooth device address*/
    } key_req;             /*!< passkey request parameter struct */

    /**
     * @brief BT_STACK_SET_EIR_DATA_EVT *
     */
    struct config_eir_data_param {
        bt_stack_status_t  stat;     /*!< set EIR status */
        bt_eir_data_type_t eir_type; /*!< EIR types in EIR type */
    } set_eir;                       /*!< set EIR data */
} bt_stack_cb_param_t;

typedef enum {
    BT_STACK_AUTH_CMPL_EVT = 0, /*!< AUTH complete event */
    BT_STACK_PIN_REQ_EVT,       /*!< Legacy Pairing Pin code request */
    BT_STACK_CFM_REQ_EVT,       /*!< Simple Pairing User Confirmation request. */
    BT_STACK_KEY_NOTIF_EVT,     /*!< Simple Pairing Passkey Notification */
    BT_STACK_KEY_REQ_EVT,       /*!< Simple Pairing Passkey request */
    BT_STACK_SET_EIR_DATA_EVT,  /*!< Set EIR data event */
    BT_STACK_EVT_MAX,
} bt_stack_cb_event_t;

/**
 * @brief      GAP event callback function type
 * @param[in]  event         Event type
 * @param[in]  param         Pointer to callback parameter
 */
typedef void (*bt_stack_event_cb_t)(bt_stack_cb_event_t event, bt_stack_cb_param_t *param);

typedef struct _bt_stack_cb_t {
    bt_stack_event_cb_t callback;
    msp_slist_t             next;
} bt_stack_cb_t;

/**
 * @brief      register callback function.
 * @param[in]  callback         callback function
 */
void bt_stack_register_callback(bt_stack_cb_t *callback);

/**
 * @brief      Set connectability mode for legacy bluetooth.
 * @param[in]  enable             enable/disable mode
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_connectable(int enable);

/**
 * @brief      Set discoverability mode for legacy bluetooth.
 * @param[in]  enable             enable/disable mode
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_discoverable(int enable);

/**
 * @brief      Set device Extended Inquiry Response data
 * @param[in]  fec_required       Forward Error Correction Require: 0/1
 * @param[in]  eir_data           Extended Inquiry Response data
 * @param[in]  eri_num            Extended Inquiry Response data num
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_eir(uint8_t fec_required, bt_eir_data_t *eir_data, size_t eir_num);

/**
 * @brief      Update device Extended Inquiry Response data
 * @param[in]  eir                Extended Inquiry Response data
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_update_eir(bt_eir_data_t *eir_data);

/**
 * @brief      Set device class of device
 * @param[in]  cod                device class of device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_set_cod(bt_stack_cod_t cod);

/**
 * @brief      Get device class of device
 * @param[in]  cod                device class of device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_get_cod(bt_stack_cod_t *cod);

/**
 * @brief      Call this function when BT_STACK_PIN_REQ_EVT generated, User should input the passkey displayed
 *             on remote device.
 * @param[in]  peer_addr          peer device bluetooth device address
 * @param[in]  pin                pin code, a string, length <= 16 bytes, only use for legacy 2.0 devices.
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_pin_entry(bt_dev_addr_t *peer_addr, const char *pin);

/**
 * @brief      Call this function when BT_STACK_KEY_REQ_EVT generated, User should input the passkey displayed
 *             on remote device.
 * @param[in]  peer_addr          peer device bluetooth device address
 * @param[in]  passkey            a numeric from 0 to 999999
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_passkey_entry(bt_dev_addr_t *peer_addr, uint32_t passkey);

/**
 * @brief      When BT_STACK_CFM_REQ_EVT generated, User should call this function to comfrim the passkey is
 *             same with remote device, if not call bt_stack_pairing_cancel.
 * @param[in]  peer_addr          peer device bluetooth device address, NULL for remove all peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_passkey_confirm(bt_dev_addr_t *peer_addr);

/**
 * @brief      User call this function to reject this pairing request.
 * @param[in]  peer_addr          peer device bluetooth device address, NULL for remove all peer device
 * @return     bt_stack_status_t  BT_STACK_STATUS_SUCCESS: success
 *                                other: failed
 */
bt_stack_status_t bt_stack_pairing_cancel(bt_dev_addr_t *peer_addr);

#ifdef __cplusplus
}
#endif

#endif /* __BT_STACK_H__ */
