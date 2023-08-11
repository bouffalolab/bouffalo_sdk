#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "shell.h"
#include "ps_extend.h"

#error "please add and modify code to FreeRTOSConfig.h, then remove this error \
/* There is already a high frequency timer running - just reset its count back \
to zero. */ \
static inline unsigned long long getCycleCounter() { \
#if __riscv_xlen == 32 \
  register unsigned int cycle, cycleh, cycleh_tmp; \
  do { \
    asm volatile ("rdcycleh %0" : "=r"(cycleh)); \
    asm volatile ("rdcycle %0" : "=r"(cycle)); \
    asm volatile ("rdcycleh %0" : "=r"(cycleh_tmp)); \
  } while(cycleh != cycleh_tmp); \
  return ((unsigned long long)cycleh << 32) | cycle; \
#elif __riscv_xlen == 64 \
  register unsigned long long cycle; \
  asm volatile ("rdcycle %0" : "=r"(cycle)); \
  return cycle; \
#endif \
} \
 \
static inline void resetCycleCounter() { \
  asm volatile ("csrw mcycle, x0"); \
} \
 \
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() resetCycleCounter() \
#define portGET_RUN_TIME_COUNTER_VALUE() getCycleCounter() \
 \
#define configCPU_CORE_CLOCK_HZ (320 * 1000 * 1000) \
#define portHAS_STAT_TRAP_TIME 1 \
#define configGENERATE_RUN_TIME_STATS 1 \
#define INCLUDE_xTaskGetIdleTaskHandle 1 \
#define configRUN_TIME_COUNTER_TYPE uint64_t


struct irq_ctx {
    char *name;
    int irqnum;
    uint64_t irq_run_time;
};

// 16 internal irq, 64 external irq, and 64 exception
#define EXCEPTION_OFFSET (16 + 64)
static void (*handler_list[1][16 + 64 + 64])(void) = {

};

static void irq_register_with_ctx(int irqnum, void *ctx)
{
    handler_list[0][irqnum] = ctx;
    return;
}

static void irq_ctx_get(int irqnum, void **ctx)
{
    *ctx = handler_list[0][irqnum];

    return;
}

static void *irq_ctx_list(int *num)
{
    *num = 16 + 64 + 64;
    return handler_list[0];
}

void irq_ctx_count_cost(int irqnum, uint64_t cost)
{
    struct irq_ctx *ctx = NULL;
    //_irq_num_check(irqnum);
    irq_ctx_get(irqnum, (void **)&ctx);
    if(ctx != NULL) {
        ctx->irq_run_time += cost;
    }
}

void exp_ctx_count_cost(int expnum, uint64_t cost)
{
    struct irq_ctx *ctx = NULL;
    //_irq_num_check(irqnum);
    irq_ctx_get(expnum + EXCEPTION_OFFSET, (void **)&ctx);
    if(ctx != NULL) {
        ctx->irq_run_time += cost;
    }
}

#define PS_BUFFER_SIZE 4096
struct tasks_stat {
    uint64_t task_num;
    TaskStatus_t *ts_array;
    uint64_t total_run_time;
    uint64_t idle_run_time;
    uint64_t trap_run_time;
};
struct irqs_stat {
    uint64_t irq_num;
    struct irq_ctx *irq_array;
};
static struct tasks_stat ps_tasks_stat;
static struct irqs_stat ps_irqs_stat;
static uint64_t interval;

// similar to vTaskList
static int u_task_list(char *write_buffer, int32_t buffer_len, float *usage, float *trap_usage)
{
    TaskStatus_t *ts_array;
    UBaseType_t array_size;
    uint32_t i, j;
    int n = 0;
    char cStatus;
    uint64_t total_run_time, trap_run_time, idle_count = 0;

    /* Take a snapshot of the number of tasks in case it changes while this
       function is executing. */
    array_size = uxTaskGetNumberOfTasks();

    if(!ps_tasks_stat.ts_array) {
        ps_tasks_stat.ts_array = pvPortMalloc(array_size * sizeof(TaskStatus_t));
        ps_tasks_stat.task_num = 0;
        ps_tasks_stat.total_run_time = 0;
        ps_tasks_stat.idle_run_time = 0;
        ps_tasks_stat.trap_run_time = 0;
    }
    ts_array = pvPortMalloc(array_size * sizeof(TaskStatus_t));

    if(!ts_array || !ps_tasks_stat.ts_array) {
        vPortFree(ts_array);
        vPortFree(ps_tasks_stat.ts_array);
        return -pdFREERTOS_ERRNO_ENOMEM;
    }

    array_size = uxTaskGetSystemState(ts_array, array_size, &total_run_time);

    if(ps_tasks_stat.total_run_time)
        interval = total_run_time - ps_tasks_stat.total_run_time;
    else
        interval = total_run_time;

    /* Create a human readable table from the binary data. */
    for(i = 0; i < array_size; i++) {
        switch(ts_array[i].eCurrentState) {
            /*
             * Macros used by vListTask to indicate which state a task is in.
             */
#define tskRUNNING_CHAR         ( 'X' )
#define tskBLOCKED_CHAR         ( 'B' )
#define tskREADY_CHAR           ( 'R' )
#define tskDELETED_CHAR         ( 'D' )
#define tskSUSPENDED_CHAR       ( 'S' )
        case eRunning:	    cStatus = tskRUNNING_CHAR;
            break;
        case eReady:	    cStatus = tskREADY_CHAR;
            break;
        case eBlocked:	    cStatus = tskBLOCKED_CHAR;
            break;
        case eSuspended:    cStatus = tskSUSPENDED_CHAR;
            break;
        case eDeleted:	    cStatus = tskDELETED_CHAR;
            break;
        default:
            cStatus = 0x00;
            break;
        }

        /* find in ps_tasks_stat */
        for(j = 0; j < ps_tasks_stat.task_num; j++) {
            if(ps_tasks_stat.ts_array[j].xHandle == ts_array[i].xHandle \
               && ps_tasks_stat.ts_array[j].xTaskNumber == ts_array[i].xTaskNumber) {
                n = snprintf(write_buffer, buffer_len, "%-15.15s\t%c\t%u\t%u\t%u\t%p\t%.4f%%\r\n", ts_array[i].pcTaskName, \
                             cStatus, (unsigned int)ts_array[i].uxCurrentPriority, (unsigned int)ts_array[i].usStackHighWaterMark, \
                             (unsigned int)ts_array[i].xTaskNumber, ts_array[i].pxStackBase, \
                             (ts_array[i].ulRunTimeCounter - ps_tasks_stat.ts_array[j].ulRunTimeCounter) \
                             / (float)interval * 100);
                break;
            }
        }

        /* cannot find in ps_tasks_stat */
        if(j == ps_tasks_stat.task_num) {
            n = snprintf(write_buffer, buffer_len,  "%-15.15s\t%c\t%u\t%u\t%u\t%p\t%.4f%%\r\n", ts_array[i].pcTaskName, \
                         cStatus, (unsigned int)ts_array[i].uxCurrentPriority, (unsigned int)ts_array[i].usStackHighWaterMark, \
                         (unsigned int)ts_array[i].xTaskNumber, ts_array[i].pxStackBase, \
                         ts_array[i].ulRunTimeCounter / (float)interval * 100);
        }

        if(ts_array[i].xHandle == xTaskGetIdleTaskHandle()) {
            idle_count = ts_array[i].ulRunTimeCounter - ps_tasks_stat.idle_run_time;
            ps_tasks_stat.idle_run_time = ts_array[i].ulRunTimeCounter;
        }
        if(n < 0) {
            vPortFree(ts_array);
            return n;
        }
        buffer_len -= n;
        write_buffer += n;
        if(buffer_len <= 0) {
            vPortFree(ts_array);
            return -pdFREERTOS_ERRNO_ENOBUFS;
        }
    }

    // todo: defer free new ts_array
    vPortFree(ps_tasks_stat.ts_array);
    ps_tasks_stat.ts_array = ts_array;
    ps_tasks_stat.task_num = array_size;
    ps_tasks_stat.total_run_time = total_run_time;
    *usage = (interval - idle_count) / (float)interval * 100;
    trap_run_time = ullPortGetTrapAllCurrentTime();
    *trap_usage = (trap_run_time - ps_tasks_stat.trap_run_time) / (float)interval * 100;
    ps_tasks_stat.trap_run_time = trap_run_time;
    return interval / configCPU_CORE_CLOCK_HZ;
}

static int u_irq_list(char *write_buffer, int32_t buffer_len)
{
    int i, n, irq_array_size;
    uint64_t irq_count;
    struct irq_ctx **ctx = (struct irq_ctx **)irq_ctx_list(&irq_array_size);
    if(!ps_irqs_stat.irq_array) {
        ps_irqs_stat.irq_num = 0;
        // todo: defer free
        ps_irqs_stat.irq_array = pvPortMalloc(irq_array_size * sizeof(struct irq_ctx));
        memset(ps_irqs_stat.irq_array, 0, irq_array_size * sizeof(struct irq_ctx));
    }
    if(!ps_irqs_stat.irq_array)
        return -pdFREERTOS_ERRNO_ENOMEM;
    for(i = 0; i < irq_array_size; i++){
        if(!ctx[i])
            continue;
        irq_count = ctx[i]->irq_run_time;
        if(irq_count == ps_irqs_stat.irq_array[i].irq_run_time)
            continue;
        n = snprintf(write_buffer, buffer_len, "[%-13.13s]\t%d\t\t\t\t\t\t%.2f%%\r\n", ctx[i]->name, ctx[i]->irqnum, \
                     (irq_count - ps_irqs_stat.irq_array[i].irq_run_time) / (float)interval * 100);
        ps_irqs_stat.irq_array[i].irq_run_time = irq_count;
        buffer_len -= n;
        write_buffer += n;
        if(buffer_len <= 0) {
            return -pdFREERTOS_ERRNO_ENOBUFS;
        }
    }
    return 0;
}

void freertos_helper_ps_init_all_irqctx(void)
{
    int i, irq_array_size;
    irq_ctx_list(&irq_array_size);
    //todo defer free
    struct irq_ctx *ctx = pvPortMalloc(irq_array_size * sizeof(struct irq_ctx));
    for(i = 0; i< irq_array_size; i++){
        if(i < EXCEPTION_OFFSET) {
            ctx->name = "irq";
            ctx->irqnum = i;
        } else {
            ctx->name = "exception";
            ctx->irqnum = i - EXCEPTION_OFFSET;
        }
        ctx->irq_run_time = 0;
        //fake address 1
        irq_register_with_ctx(i, ctx);
        ctx++;
    }
}

static void ps_cmd(char *buf, int len, int argc, char **argv)
{
    freertos_helper_ps_init();

    char *buffer;
    uint32_t buffer_len = PS_BUFFER_SIZE;
    float total_usage = 0, trap_usage = 0;
    int ago;
    const char *const header = "%uMHZ\t\t\t\t\t\t\t\tTrap: %.6f%%\r\nTask\t\tS\tP\tStack\t#\tBase" \
        "\t\tUsage(%.2f%% %us)\r\n" \
        "**********************************************************************************\r\n";

    buffer = pvPortMalloc(buffer_len);
    if (NULL == buffer) {
        printf("cli: malloc error\r\n");
        return;
    }
    *buffer = 0;
    ago = u_task_list(buffer, buffer_len, &total_usage, &trap_usage);
    if (ago >= 0) {
        printf(header, (unsigned int)configCPU_CORE_CLOCK_HZ / (1000 * 1000), trap_usage, total_usage, (unsigned int)ago);
        puts(buffer);
    } else if (ago == -pdFREERTOS_ERRNO_ENOBUFS) {
        printf("cli: buffer is not enough\r\n");
    } else if (ago == -pdFREERTOS_ERRNO_ENOMEM) {
        printf("cli: malloc error\r\n");
    } else {
        printf("cli: other error\r\n");
    }
    *buffer = 0;
    ago = u_irq_list(buffer, buffer_len);
    if (ago >= 0) {
        puts(buffer);
    } else if (ago == -pdFREERTOS_ERRNO_ENOBUFS) {
        printf("cli: buffer is not enough\r\n");
    } else if (ago == -pdFREERTOS_ERRNO_ENOMEM) {
        printf("cli: malloc error\r\n");
    } else {
        printf("cli: other error\r\n");
    }
    vPortFree(buffer);
}

SHELL_CMD_EXPORT_ALIAS(ps_cmd, ps_extend, task dump with extend usage);

int freertos_helper_ps_init(void)
{
    static int inited = 0;
    if(!inited) {
        vPortRegisterReportIrqCost(irq_ctx_count_cost);
        vPortRegisterReportExpCost(exp_ctx_count_cost);
        freertos_helper_ps_init_all_irqctx();
        inited = 1;
    }
    return 0;
}
