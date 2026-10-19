#ifndef _SDH_OSAL_H_
#define _SDH_OSAL_H_

#include "bflb_core.h"

#ifndef CONFIG_BSP_SDH_OSAL_POLLING_MODE
#define CONFIG_BSP_SDH_OSAL_POLLING_MODE 0
#endif

#if CONFIG_BSP_SDH_OSAL_POLLING_MODE
/* polling mode (No interrupt are used) */
#define SDH_OSAL_TYPE (0)

#else /* CONFIG_BSP_SDH_OSAL_POLLING_MODE */

/* Interrupt used */
#ifdef CONFIG_FREERTOS
/* freeRTOS */
#define SDH_OSAL_TYPE (1)
#else
/* no os */
#define SDH_OSAL_TYPE (0)
#endif

#endif /* CONFIG_BSP_SDH_OSAL_POLLING_MODE */

void sdh_osal_delay_ms(uint32_t delay_ms);

#if (CONFIG_BSP_SDH_OSAL_POLLING_MODE == 0)
/* Binary Semaphore, Created in an Empty state */
void *sdh_osal_semb_create(void);
int sdh_osal_semb_delete(void *semb);
int sdh_osal_semb_take(void *semb, uint32_t timeout_ms);
int sdh_osal_semb_give(void *semb);
#endif

/* Recursive mutex: same task can take it multiple times without deadlock.
 * Semantics aligned with Linux sdio_claim_host / mmc_claim_host. */
void *sdh_osal_rmutex_create(void);
int sdh_osal_rmutex_delete(void *rmutex);
int sdh_osal_rmutex_take(void *rmutex);
int sdh_osal_rmutex_give(void *rmutex);

/* Task adaptation. Only available when an RTOS is present. */
typedef void (*sdh_osal_task_entry_t)(void *arg);
void *sdh_osal_task_create(const char *name,
                           sdh_osal_task_entry_t entry,
                           void *arg,
                           uint32_t stack_size,
                           uint32_t priority);
int sdh_osal_task_delete(void *task);
void sdh_osal_task_exit_self(void);

#endif
