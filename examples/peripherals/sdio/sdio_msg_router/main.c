
/**
 * @file main.c
 * @brief SDIO message test main program
 */

#include "bflb_core.h"
#include "bflb_mtimer.h"

#include "board.h"

#include "FreeRTOS.h"
#include "task.h"

#include "mr_frame_buff_ctrl.h"

#include "mr_msg_ctrl.h"

#include "mr_speed_test_start.h"
#include "mr_netdev.h"
#include "mr_tty.h"

#define DBG_TAG "MAIN"
#include "log.h"

/*****************************************************************************
 * Global Variables
 *****************************************************************************/

static TaskHandle_t start_task_hd;

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief Start task for system initialization
 * @param param Task parameter (unused)
 * @retval None
 */
void start_task(void *param)
{
    /* shell init */
    struct bflb_device_s *uart0 = bflb_device_get_by_name("uart0");
    extern void shell_init_with_task(struct bflb_device_s * shell);
    shell_init_with_task(uart0);
    LOG_I("\r\nShell ready\r\n");

    mr_msg_ctrl_priv_t *msg_ctrl = NULL;

    int msg_ctrl_sdio_init(mr_msg_ctrl_priv_t * *msg_ctrl);
    msg_ctrl_sdio_init(&msg_ctrl);
    if (msg_ctrl == NULL) {
        LOG_E("Error: mr_msg_ctrl_init failed\n");
        vTaskDelete(NULL);
        return;
    }

    /* Initialize speed test framework */
    speed_test_init(msg_ctrl);

    /* Initialize Ethernet EMAC */
    void netdev_emac_init(mr_msg_ctrl_priv_t * msg_ctrl);
    netdev_emac_init(msg_ctrl);

    /* Initialize tty cli */
    void tty_cli_init(mr_msg_ctrl_priv_t * msg_ctrl);
    tty_cli_init(msg_ctrl);

    /* Delete current task */
    vTaskDelete(NULL);
}

/**
 * @brief Main function
 * @retval int Exit status
 */
int main(void)
{
    /* Initialize board */
    board_init();

    bflb_mtimer_delay_ms(100);

    LOG_I("SDIO MSG TEST\r\n");

    /* Create and start initial task */
    xTaskCreate(start_task, (char *)"start_task", 1024, NULL, configMAX_PRIORITIES - 2, &start_task_hd);
    vTaskStartScheduler();

    /* Should never reach here */
    while (1) {
        bflb_mtimer_delay_ms(5000);
    }
}
