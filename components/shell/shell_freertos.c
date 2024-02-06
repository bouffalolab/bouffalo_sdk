#include "shell.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "ring_buffer.h"
#include "bflb_uart.h"
#include "mem.h"

#if defined(CONFIG_SHELL_EXEC_THREAD) && CONFIG_SHELL_EXEC_THREAD
static int shell_exec_argc;
static char *shell_exec_argv[SHELL_ARG_NUM + 1];
static char shell_exec_line[SHELL_CMD_SIZE];
static ptrdiff_t shell_exec_line_diff;
TaskHandle_t shell_exec_handle;
static ATTR_NOCACHE_RAM_SECTION volatile bool shell_exec_end = true;

static void shell_exec_task(void *pvParameters)
{
    shell_exec_end = false;
    __ASM volatile("fence");
    ((cmd_function_t)(pvParameters))(shell_exec_argc, shell_exec_argv);
    shell_exec_end = true;
    __ASM volatile("fence");
    vTaskDelete(shell_exec_handle);
    shell_exec_handle = NULL;
}

void shell_dup_line(char *cmd, uint32_t length)
{
    memcpy(shell_exec_line, cmd, length + 1);
    shell_exec_line_diff = shell_exec_line - cmd;
}

void shell_abort_exec(int sig)
{
    (void)sig;
    if (shell_exec_end == false) {
        shell_exec_end = true;
        __ASM volatile("fence");
        vTaskDelete(shell_exec_handle);
        shell_exec_handle = NULL;
    }
}

int shell_start_exec(cmd_function_t func, int argc, char *argv[])
{
    BaseType_t xReturned;
    shell_abort_exec(SHELL_SIGINT);
    shell_exec_argc = argc;

    for (uint8_t i = 0; i < argc; i++) {
        shell_exec_argv[i] = argv[i] + shell_exec_line_diff;
    }
    shell_exec_argv[argc] = NULL;

    __ASM volatile("fence");
    xReturned = xTaskCreate(shell_exec_task, (char *)"shell_exec_task", SHELL_EXEC_THREAD_STACK_SIZE, func, SHELL_EXEC_THREAD_PRIO, &shell_exec_handle);

    if (xReturned == pdPASS) {
        return 0;
    } else {
        shell_exec_end = true;
        return -1;
    }
}

#endif

static TaskHandle_t shell_handle;
SemaphoreHandle_t sem_shell = NULL;

Ring_Buffer_Type shell_rb;

uint8_t shell_buffer[512];

void shell_release_sem(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (xPortIsInsideInterrupt()) {
        int ret = xSemaphoreGiveFromISR(sem_shell, &xHigherPriorityTaskWoken);
        if (ret == pdPASS) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    } else {
        xSemaphoreGive(sem_shell);
    }
}

struct bflb_device_s *uart_shell = NULL;

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
    if (shell) {
        uart_shell = shell;
        bflb_uart_rxint_mask(uart_shell, false);
        bflb_irq_attach(uart_shell->irq_num, uart_shell_isr, NULL);
        bflb_irq_enable(uart_shell->irq_num);
    }

    vSemaphoreCreateBinary(sem_shell);

    Ring_Buffer_Init(&shell_rb, shell_buffer, sizeof(shell_buffer), NULL, NULL);

    shell_init();
    xTaskCreate(shell_task, (char *)"shell_task", SHELL_THREAD_STACK_SIZE, NULL, SHELL_THREAD_PRIO, &shell_handle);
}

void shell_exe_cmd(uint8_t *cmd, uint16_t len)
{
    Ring_Buffer_Write(&shell_rb, cmd, len);
    shell_release_sem();
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
    if(strlcpy(pcWriteBuffer, "Task", 1536) >= 1536)
        printf("[OS]: strlcpy truncated \r\n");
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
    if(strlcpy(pcWriteBuffer, pcHeader, 1536 - (pcWriteBuffer - info)) >= \
               1536 - (pcWriteBuffer - info))
        printf("[OS]: strlcpy truncated \r\n");
    vTaskList(pcWriteBuffer + strlen(pcHeader));
    printf("\r\n");
    printf(info);

    free(info);
}
SHELL_CMD_EXPORT_ALIAS(ps_cmd, ps, shell ps);
