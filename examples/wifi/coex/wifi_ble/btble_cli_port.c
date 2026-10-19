#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"

#include "bluetooth.h"
#include "conn.h"
#include "conn_internal.h"
#include "btble_lib_api.h"
#if defined(BL616)
#include "bl616_glb.h"
#elif defined(BL618DG)
#include "bl618dg_glb.h"
#elif defined(BL616CL)
#include "bl616cl_glb.h"
#endif

#include "ble_cli_cmds.h"
#include "hci_driver.h"
#include "hci_core.h"
#include "btble_test_port.h"

static TaskHandle_t btble_cli_start_handle;

static void ble_connected(struct bt_conn *conn, u8_t err)
{
    if (conn->type != BT_CONN_TYPE_LE) {
        return;
    }

    /* 性能测试 hook：连接成功(err==0)/失败(err!=0)都需通知测试逻辑 */
    ble_test_on_connected(conn, err);

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

    /* 性能测试 hook：Slave 自行重广播 / Master 不重广播继续发起连接。
     * 返回 true 表示测试已接管广播控制，跳过默认重广播；
     * 非测试态返回 false，走默认重广播逻辑。 */
    if (ble_test_on_disconnected(conn, reason)) {
        return;
    }

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
        ble_cli_register();
        ble_test_port_init();
    }
}

static void btble_cli_start_task(void *pvParameters)
{
    btble_controller_init(configMAX_PRIORITIES - 1);
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
                      &btble_cli_start_handle);
    return (ret == pdPASS) ? 0 : -1;
}

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
