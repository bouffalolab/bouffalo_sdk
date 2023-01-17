#include "shell.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "ring_buffer.h"
#include "bflb_uart.h"

static TaskHandle_t shell_handle;
SemaphoreHandle_t sem_shell = NULL;

Ring_Buffer_Type shell_rb;

uint8_t shell_buffer[512];

void shell_release_sem(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    int ret = xSemaphoreGiveFromISR(sem_shell, &xHigherPriorityTaskWoken);
    if (ret == pdPASS) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

static struct bflb_device_s *uart_shell;

void uart_shell_isr(int irq, void *arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uart_shell);
    if (intstatus & UART_INTSTS_RX_FIFO) {
        while (bflb_uart_rxavailable(uart_shell)) {
            Ring_Buffer_Write_Byte(&shell_rb, bflb_uart_getchar(uart_shell));
        }
        shell_release_sem();
    }
    if (intstatus & UART_INTSTS_RTO) {
        while (bflb_uart_rxavailable(uart_shell)) {
            Ring_Buffer_Write_Byte(&shell_rb, bflb_uart_getchar(uart_shell));
        }
        shell_release_sem();
        bflb_uart_int_clear(uart_shell, UART_INTCLR_RTO);
    }
}

static void shell_task(void *pvParameters)
{
    uint8_t data;
    uint32_t len;
    while (1) {
        if (xSemaphoreTake(sem_shell, portMAX_DELAY) == pdTRUE) {
            len = Ring_Buffer_Get_Length(&shell_rb);
            for (uint32_t i = 0; i < len; i++) {
                Ring_Buffer_Read_Byte(&shell_rb, &data);
                shell_handler(data);
            }
        }
    }
}

void shell_init_with_task(struct bflb_device_s *shell)
{
    uart_shell = shell;
    vSemaphoreCreateBinary(sem_shell);

    bflb_uart_rxint_mask(uart_shell, false);
    bflb_irq_attach(uart_shell->irq_num, uart_shell_isr, NULL);
    bflb_irq_enable(uart_shell->irq_num);

    Ring_Buffer_Init(&shell_rb, shell_buffer, sizeof(shell_buffer), NULL, NULL);

    shell_init();
    xTaskCreate(shell_task, (char *)"shell_task", SHELL_THREAD_STACK_SIZE, NULL, SHELL_THREAD_PRIO, &shell_handle);
}

static void ps_cmd(int argc, char **argv)
{
    char *pcWriteBuffer, *info;
    const char *const pcHeader = "State   Priority  Stack    #          Base\r\n********************************************************\r\n";
    BaseType_t xSpacePadding;

    info = malloc(1536);
    if (NULL == info) {
        return;
    }
    pcWriteBuffer = info;

    /* Generate a table of task stats. */
    strcpy(pcWriteBuffer, "Task");
    pcWriteBuffer += strlen(pcWriteBuffer);

    /* Minus three for the null terminator and half the number of characters in
    "Task" so the column lines up with the centre of the heading. */
    for (xSpacePadding = strlen("Task"); xSpacePadding < (configMAX_TASK_NAME_LEN - 3); xSpacePadding++) {
        /* Add a space to align columns after the task's name. */
        *pcWriteBuffer = ' ';
        pcWriteBuffer++;

        /* Ensure always terminated. */
        *pcWriteBuffer = 0x00;
    }
    strcpy(pcWriteBuffer, pcHeader);
    vTaskList(pcWriteBuffer + strlen(pcHeader));
    printf(info);

    free(info);
}
SHELL_CMD_EXPORT_ALIAS(ps_cmd, ps, shell ps);
