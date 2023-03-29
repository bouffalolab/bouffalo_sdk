#include "shell.h"
#include <FreeRTOS.h>
#include "task.h"
#include "board.h"

#include "bluetooth.h"
#include "conn.h"
#if defined(BL702) || defined(BL602)
#include "ble_lib_api.h"
#elif defined(BL616)
#include "btble_lib_api.h"
#endif
#include "hci_driver.h"
#include "bl616_glb.h"
#include "ble_cli_cmds.h"
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

void bt_enable_cb(int err)
{
    if (!err) {
        bt_addr_le_t bt_addr;
        bt_get_local_public_address(&bt_addr);
        printf("BD_ADDR:(MSB)%02x:%02x:%02x:%02x:%02x:%02x(LSB) \n",
            bt_addr.a.val[5], bt_addr.a.val[4], bt_addr.a.val[3], bt_addr.a.val[2], bt_addr.a.val[1], bt_addr.a.val[0]);
        ble_cli_register();
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
