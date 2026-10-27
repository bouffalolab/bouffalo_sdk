#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#if defined(BL602)
#include "ble_lib_api.h"
#else
#include "btble_lib_api.h"
#endif
#if defined(BL616)
#include "bl616_glb.h"
#elif defined(BL618DG)
#include "bl618dg_glb.h"
#elif defined(BL616CL)
#include "bl616cl_glb.h"
#endif

#include "hci_driver.h"
#include "hci_core.h"
#if defined(CONFIG_BT_STACK_CLI)
#include "ble_cli_cmds.h"
#include "btble_test_port.h"
#else
#include <string.h>
#include "misc/util.h"
#endif

static void ble_connected(struct bt_conn *conn, u8_t err)
{
    if (conn->type != BT_CONN_TYPE_LE) {
        return;
    }

#if defined(CONFIG_BT_STACK_CLI)
    /* 性能测试 hook：连接成功(err==0)/失败(err!=0)都需通知测试逻辑 */
    ble_test_on_connected(conn, err);
#endif

    if (err) {
        return;
    }

    printf("%s", __func__);
}

static void ble_disconnected(struct bt_conn *conn, u8_t reason)
{
    if (conn->type != BT_CONN_TYPE_LE) {
        return;
    }

    printf("%s", __func__);

#if defined(CONFIG_BT_STACK_CLI)
    /* 性能测试 hook：Slave 自行重广播 / Master 不重广播继续发起连接。
     * 返回 true 表示测试已接管广播控制，跳过默认重广播；
     * 非测试态返回 false，走默认重广播逻辑。 */
    if (ble_test_on_disconnected(conn, reason)) {
        return;
    }
#endif

    if (set_adv_enable(true)) {
        printf("Restart adv fail. \r\n");
    }
}

static struct bt_conn_cb ble_conn_callbacks = {
    .connected = ble_connected,
    .disconnected = ble_disconnected,
};

static void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \r\n",
               bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3],
               bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
        bt_conn_cb_register(&ble_conn_callbacks);
#if defined(CONFIG_BT_STACK_CLI)
        ble_cli_register();
        ble_test_port_init();
#endif
    }
}

static void btble_cli_start_task(void *pvParameters)
{
#if defined(BL602)
    ble_controller_init(configMAX_PRIORITIES - 1);
#else
    btble_controller_init(configMAX_PRIORITIES - 1);
#endif
    hci_driver_init();
    bt_enable(bt_enable_cb);

    vTaskDelete(NULL);
}

int btble_cli_start(void)
{
    BaseType_t ret;

    configASSERT((configMAX_PRIORITIES > 4));

    ret = xTaskCreate(btble_cli_start_task, (char *)"btble_cli",
                      1024, NULL, configMAX_PRIORITIES - 2,
                      NULL);
    return (ret == pdPASS) ? 0 : -1;
}

#if !defined(CONFIG_BT_STACK_CLI)
static void ble_set_adv_channel(int argc, char **argv)
{
    uint8_t channel = 0;

    if (argc != 2) {
        printf("Usage: ble_set_adv_channel <mask:1-7>\r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &channel);
    if (set_adv_channel_map(channel)) {
        printf("Failed to set adv channel\r\n");
    } else {
        printf("Set adv channel success\r\n");
    }
}

static void ble_start_adv(int argc, char **argv)
{
    struct bt_le_adv_param param = {
        .id = BT_ID_DEFAULT,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_2,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_2,
    };
    uint8_t adv_type = 0, mode = 0;
    uint8_t flags = BT_LE_AD_NO_BREDR;
    const char *name = bt_get_name();
    struct bt_data ad[] = {
        BT_DATA(BT_DATA_FLAGS, &flags, sizeof(flags)),
        BT_DATA(BT_DATA_NAME_COMPLETE, name, strlen(name)),
    };
    int err;

    if (argc != 3 && argc != 5) {
        printf("Usage: ble_start_adv <type:0-2> <mode:0-2> [min] [max]\r\n");
        return;
    }

    get_uint8_from_string(&argv[1], &adv_type);
    get_uint8_from_string(&argv[2], &mode);
    if (adv_type > 2 || mode > 2) {
        printf("Invalid advertising type or mode\r\n");
        return;
    }

    if (adv_type == 0) {
        param.options = BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME | BT_LE_ADV_OPT_ONE_TIME;
    } else if (adv_type == 1) {
        param.options = BT_LE_ADV_OPT_USE_NAME;
    }
    if (mode == 0) {
        flags |= BT_LE_AD_GENERAL;
    } else if (mode == 1) {
        flags |= BT_LE_AD_LIMITED;
    }
    if (argc == 5) {
        get_uint16_from_string(&argv[3], &param.interval_min);
        get_uint16_from_string(&argv[4], &param.interval_max);
    }

    err = bt_le_adv_start(&param, ad, ARRAY_SIZE(ad), adv_type == 2 ? NULL : ad,
                          adv_type == 2 ? 0 : 1);
    if (err) {
        printf("Failed to start advertising (err %d)\r\n", err);
    } else {
        printf("Advertising started\r\n");
    }
}

static void ble_stop_adv(int argc, char **argv)
{
    int err = bt_le_adv_stop();

    if (err) {
        printf("Failed to stop advertising (err %d)\r\n", err);
    } else {
        printf("Advertising stopped\r\n");
    }
}

SHELL_CMD_EXPORT_ALIAS(ble_set_adv_channel, ble_set_adv_channel, ble set adv channel mask 1-7);
SHELL_CMD_EXPORT_ALIAS(ble_start_adv, ble_start_adv, ble start adv type mode [min] [max]);
SHELL_CMD_EXPORT_ALIAS(ble_stop_adv, ble_stop_adv, ble stop adv);
#endif

#if defined(BL616CL)
#include "mm.h"

int heap_add_em(int argc, char **argv)
{
    extern uint8_t __LD_CONFIG_EM_SEL;
    volatile uint32_t em_size;

    em_size = (uint32_t)&__LD_CONFIG_EM_SEL;

    if (em_size > 0) {
        uint32_t em_heap_addr = 0x21020000 - em_size;
        GLB_Set_EM_Sel(GLB_WRAM160KB_EM0KB);
        mm_register_heap(MM_HEAP_EM_0, "EM", MM_ALLOCATOR_HEAP5,
                         (void *)em_heap_addr, em_size);
    }

    return 0;
}
SHELL_CMD_EXPORT_ALIAS(heap_add_em, heap_add_em, heap add EM.);
#endif
