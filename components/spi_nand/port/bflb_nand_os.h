/* Copyright (c) 2026 Bouffalo Lab
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef BFLB_NAND_OS_H
#define BFLB_NAND_OS_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#if defined(CONFIG_FREERTOS) && CONFIG_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "bflb_mtimer.h"

typedef SemaphoreHandle_t nand_mutex_t;

static inline nand_mutex_t nand_mutex_create(void)
{
    return xSemaphoreCreateMutex();
}

static inline void nand_mutex_destroy(nand_mutex_t m)
{
    vSemaphoreDelete(m);
}

static inline void nand_mutex_lock(nand_mutex_t m)
{
    xSemaphoreTake(m, portMAX_DELAY);
}

static inline void nand_mutex_unlock(nand_mutex_t m)
{
    xSemaphoreGive(m);
}

static inline uint64_t nand_time_us(void)
{
    return bflb_mtimer_get_time_us();
}

static inline void nand_delay_us(uint32_t us)
{
    bflb_mtimer_delay_us(us);
}

#elif defined(CONFIG_SPI_NAND_HOST_TEST)

#include <pthread.h>
#include <time.h>

typedef pthread_mutex_t *nand_mutex_t;

static inline nand_mutex_t nand_mutex_create(void)
{
    nand_mutex_t m = malloc(sizeof(*m));

    if (m && pthread_mutex_init(m, NULL)) {
        free(m);
        return NULL;
    }

    return m;
}

static inline void nand_mutex_destroy(nand_mutex_t m)
{
    pthread_mutex_destroy(m);
    free(m);
}

static inline void nand_mutex_lock(nand_mutex_t m)
{
    int result = pthread_mutex_lock(m);

    assert(!result);
    (void)result;
}

static inline void nand_mutex_unlock(nand_mutex_t m)
{
    int result = pthread_mutex_unlock(m);

    assert(!result);
    (void)result;
}

static inline uint64_t nand_time_us(void)
{
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000U + t.tv_nsec / 1000U;
}

static inline void nand_delay_us(uint32_t us)
{
    struct timespec t = { us / 1000000U, (long)(us % 1000000U) * 1000L };

    nanosleep(&t, NULL);
}

#else

#include "bflb_mtimer.h"

typedef unsigned char *nand_mutex_t;

static inline nand_mutex_t nand_mutex_create(void)
{
    return calloc(1, 1);
}

static inline void nand_mutex_destroy(nand_mutex_t m)
{
    free(m);
}

static inline void nand_mutex_lock(nand_mutex_t m)
{
    assert(!*m);
    *m = 1;
}

static inline void nand_mutex_unlock(nand_mutex_t m)
{
    *m = 0;
}

static inline uint64_t nand_time_us(void)
{
    return bflb_mtimer_get_time_us();
}

static inline void nand_delay_us(uint32_t us)
{
    bflb_mtimer_delay_us(us);
}
#endif

#endif
