#include <stdint.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#if ( INCLUDE_uxTaskGetStackBase != 1 ) || ( INCLUDE_xTaskGetSchedulerState != 1 )
#error please enable INCLUDE_uxTaskGetStackBase, enable INCLUDE_xTaskGetSchedulerState, \
       and you MUST modify SSCANF_USE_STACK_SIZE macro in your platform.
#endif

#define MAX_STACK_ARGS (16) //must keep sscanf_stack_top aligned to 16
#define FIXED_ARGS (2)
#define REGISTER_ARGS (8)
#define SSCANF_USE_STACK_SIZE (1024)

#define MAX_VARIABLE_ARGS (REGISTER_ARGS - FIXED_ARGS + MAX_STACK_ARGS)
static __attribute__((aligned(16))) char sscanf_stack[SSCANF_USE_STACK_SIZE + MAX_STACK_ARGS * 4];
void *sscanf_stack_top = &sscanf_stack[SSCANF_USE_STACK_SIZE];
unsigned int wrap_sscanf_stack;

struct {
    uint32_t args[MAX_VARIABLE_ARGS];
} sscanf_call_convention;

static SemaphoreHandle_t wrap_sscanf_lock = NULL;
static StaticSemaphore_t wrap_sscanf_lock_buffer;

int __real_sscanf(void);
int __wrap_sscanf(const char *str, const char *format, ...)
{
    va_list ap;
    int ret = 0;

    asm volatile ("csrc mstatus, 8");
    if(!wrap_sscanf_lock) {
        wrap_sscanf_lock = xSemaphoreCreateBinaryStatic(&wrap_sscanf_lock_buffer);
        xSemaphoreGive(wrap_sscanf_lock);
    }
    asm volatile ("csrs mstatus, 8");

    if((uint32_t)__builtin_frame_address(0) - uxTaskGetStackBase(NULL) + 32
       <= SSCANF_USE_STACK_SIZE) {
        printf("[SYS] stack no free, sscanf switch stack \r\n");
        configASSERT(xTaskGetSchedulerState() != taskSCHEDULER_SUSPENDED);
        xSemaphoreTake(wrap_sscanf_lock, portMAX_DELAY);
        // save call args
        va_start(ap, format);
        for(int i=0; i<MAX_VARIABLE_ARGS; i++)
            sscanf_call_convention.args[i] = va_arg(ap, uint32_t);
        va_end(ap);

        // copy args to dst stack, skip register variable args
        asm volatile ("la t0, sscanf_call_convention \n"
                      "addi t3, t0, %0         \n"
                      "addi t0, t0, %1         \n"
                      "lw t1, sscanf_stack_top \n"
                      "1:              \n"
                      "lw t2, (t0)     \n"
                      "sw t2, (t1)     \n"
                      "addi t0, t0, 4  \n"
                      "addi t1, t1, 4  \n"
                      "bltu t0, t3, 1b \n" :: "p"(MAX_VARIABLE_ARGS * 4), "p"((REGISTER_ARGS - FIXED_ARGS) * 4));

        // prepare a2-a7
        asm volatile ("la t0, sscanf_call_convention \n"
                      "lw a0, %0       \n"
                      "lw a1, %1       \n"
                      "lw a2, (t0)     \n"
                      "lw a3, 4(t0)    \n"
                      "lw a4, 8(t0)    \n"
                      "lw a5, 12(t0)   \n"
                      "lw a6, 16(t0)   \n"
                      "lw a7, 20(t0)   \n" :: "m"(str), "m"(format));

        // save current sp
        asm volatile ("la t0, wrap_sscanf_stack \n"
                      "sw sp, 0(t0)");
        // avoid directly lw sp
        asm volatile ("lw t0, sscanf_stack_top \n"
                      "mv sp, t0");
        ret = __real_sscanf();
        // restore current sp
        asm volatile ( "lw t0, wrap_sscanf_stack \n"
                       "mv sp, t0");

        xSemaphoreGive(wrap_sscanf_lock);
    } else {
        va_start(ap, format);
        asm volatile ("lw t0, %2        \n"
                      "lw a0, %0        \n"
                      "lw a1, %1        \n"
                      "lw a2, 0(t0)     \n"
                      "lw a3, 4(t0)     \n"
                      "lw a4, 8(t0)     \n"
                      "lw a5, 12(t0)    \n"
                      "lw a6, 16(t0)    \n"
                      "lw a7, 20(t0)    \n" :: "m"(str), "m"(format), "m"(ap));
        va_end(ap);

        asm volatile ("lw ra, 44(sp)   \n"
                      "lw s0, 40(sp)   \n"
                      "lw s1, 36(sp)   \n"
                      "lw s2, 32(sp)   \n"
                      "addi sp, sp, 80 \n"
                      "j __real_sscanf");

        // j return to last frame
    }

    return ret;
}
