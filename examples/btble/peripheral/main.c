#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"
#include "board.h"

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#elif defined(BL616)
#include "btble_lib_api.h"
#endif
#include "hci_driver.h"
#include "bl616_glb.h"
#include "ble_tp_svc.h"
#include "rfparam_adapter.h"
#include "hci_core.h"


static struct bflb_device_s *uart0;

extern void shell_init_with_task(struct bflb_device_s *shell);

static int btblecontroller_em_config(void)
{
    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;

    if (em_size == 0) {
        GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
    } else if (em_size == 32*1024) {
        GLB_Set_EM_Sel(GLB_WRAM128KB_EM32KB);
    } else if (em_size == 64*1024) {
        GLB_Set_EM_Sel(GLB_WRAM96KB_EM64KB);
    } else {
        GLB_Set_EM_Sel(GLB_WRAM96KB_EM64KB);
    }

    return 0;
}

static void ble_connected(struct bt_conn *conn, u8_t err)
{
    if(err || conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }
    printf("%s",__func__);
}

static void ble_disconnected(struct bt_conn *conn, u8_t reason)
{ 
    int ret;

    if(conn->type != BT_CONN_TYPE_LE)
    {
        return;
    }

    printf("%s",__func__);

    // enable adv
    ret = set_adv_enable(true);
    if(ret) {
        printf("Restart adv fail. \r\n");
    }
}

static struct bt_conn_cb ble_conn_callbacks = {
	.connected	=   ble_connected,
	.disconnected	=   ble_disconnected,
};

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
    if(err){
        printf("Failed to start advertising (err %d) \r\n", err);
    }
    printf("Start advertising success.\r\n");
}

void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
            bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);

        bt_conn_cb_register(&ble_conn_callbacks);
        ble_tp_init();

        // start advertising
        ble_start_adv();
    }
}

int main(void)
{
    board_init();

    configASSERT((configMAX_PRIORITIES > 4));

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    /* set ble controller EM Size */
    btblecontroller_em_config();

    /* Init rf */
    if (0 != rfparam_init(0, NULL, 0)) {
        printf("PHY RF init failed!\r\n");
        return 0;
    }

    // Initialize BLE controller
    #if defined(BL702) || defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
    #else
    btble_controller_init(configMAX_PRIORITIES - 1);
    #endif
    // Initialize BLE Host stack
    hci_driver_init();
    bt_enable(bt_enable_cb);

    vTaskStartScheduler();

    while (1) {
    }
}
