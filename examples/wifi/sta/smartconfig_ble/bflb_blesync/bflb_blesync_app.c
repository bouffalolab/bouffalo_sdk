#include "bflb_blesync_app.h"
#include <FreeRTOS.h>
#include <task.h>
#include "bflb_blesync.h"
#include "ble_cli_cmds.h"
#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#elif defined(BL616)
#include "btble_lib_api.h"
#include "bl616_glb.h"
#include "rfparam_adapter.h"
#elif defined(BL808)
#include "btble_lib_api.h"
#include "bl808_glb.h"
#endif

#include "hci_driver.h"
#include "hci_core.h"

#include "lwip/ip4.h"

extern const struct blesync_wifi_func *wifi_drv_register(void);

static bl_ble_sync_t *gp_index = NULL;

static void blesync_complete_cb(void *p_arg)
{
    bl_ble_sync_t *p_index = (bl_ble_sync_t *)p_arg;
    bl_ble_sync_stop(p_index);
    vPortFree(p_index);
}

static void ble_start_adv(void)
{
    struct bt_le_adv_param param;
    int err = -1;
    struct bt_data adv_data[1] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL)
    };
    struct bt_data adv_rsp[1] = {
        BT_DATA_BYTES(BT_DATA_MANUFACTURER_DATA, "BL616")
    };

    memset(&param, 0, sizeof(param));
    // Set advertise interval
    param.interval_min = BT_GAP_ADV_FAST_INT_MIN_2;
    param.interval_max = BT_GAP_ADV_FAST_INT_MAX_2;
    /*Get adv type, 0:adv_ind,  1:adv_scan_ind, 2:adv_nonconn_ind 3: adv_direct_ind*/
    param.options = (BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME);

    err = bt_le_adv_start(&param, adv_data, ARRAY_SIZE(adv_data), adv_rsp, ARRAY_SIZE(adv_rsp));
    if (err) {
        printf("Failed to start advertising (err %d) \r\n", err);
    }
    printf("Start advertising success.\r\n");
}

static void bflb_blesync_init(int err)
{
    if (err != 0) {
        return;
    }
    if (gp_index != NULL) {
        printf("bflb_blesync already started\r\n");
        return;
    }
#ifdef CONFIG_BT_STACK_CLI
    ble_cli_register();
#endif /* CONFIG_BT_STACK_CLI */
    gp_index = pvPortMalloc(sizeof(bl_ble_sync_t));
    if (gp_index == NULL) {
        return;
    }
    bl_ble_sync_start(gp_index,
                      wifi_drv_register(),
                      blesync_complete_cb,
                      (void *)gp_index);
}

void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
               bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
        bt_set_name("bflb_blesync");
        bflb_blesync_init(0);
        // start advertising
        ble_start_adv();
    }
}

static void ble_stack_start(void)
{
// Initialize BLE controller
#if defined(BL702) || defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
#else
    btble_controller_init(configMAX_PRIORITIES - 1);
#endif
    // Initialize BLE Host stack
    hci_driver_init();
    bt_enable(bt_enable_cb);
}

void bflb_blesync_start(void)
{
    static uint8_t stack_started = 0;

    if (stack_started == 1) {
        bflb_blesync_init(0);
        return;
    }
    ble_stack_start();
    stack_started = 1;
}

void bflb_blesync_stop(void)
{
    bl_ble_sync_stop(gp_index);
    vPortFree(gp_index);
    gp_index = NULL;
}
