#include "sec_mutex.h"

#ifdef CONFIG_FREERTOS
#include <FreeRTOS.h>
#include "semphr.h"

static StaticSemaphore_t aes_mutex_buf;
static SemaphoreHandle_t aes_mutex = NULL;

static StaticSemaphore_t sha_mutex_buf;
static SemaphoreHandle_t sha_mutex = NULL;

static StaticSemaphore_t pka_mutex_buf;
static SemaphoreHandle_t pka_mutex = NULL;

void bflb_sec_mutex_init(void)
{
    aes_mutex = xSemaphoreCreateMutexStatic(&aes_mutex_buf);
    sha_mutex = xSemaphoreCreateMutexStatic(&sha_mutex_buf);
    pka_mutex = xSemaphoreCreateMutexStatic(&pka_mutex_buf);
}

int bflb_sec_aes_mutex_take(void)
{
    if (pdPASS != xSemaphoreTake(aes_mutex, portMAX_DELAY)) {
        return -1;
    }
    return 0;
}

int bflb_sec_aes_mutex_give(void)
{
    if (pdPASS != xSemaphoreGive(aes_mutex)) {
        return -1;
    }
    return 0;
}

int bflb_sec_sha_mutex_take(void)
{
    if (pdPASS != xSemaphoreTake(sha_mutex, portMAX_DELAY)) {
        return -1;
    }
    return 0;
}

int bflb_sec_sha_mutex_give(void)
{
    if (pdPASS != xSemaphoreGive(sha_mutex)) {
        return -1;
    }
    return 0;
}

int bflb_sec_pka_mutex_take(void)
{
    if (pdPASS != xSemaphoreTake(pka_mutex, portMAX_DELAY)) {
        return -1;
    }
    return 0;
}

int bflb_sec_pka_mutex_give(void)
{
    if (pdPASS != xSemaphoreGive(pka_mutex)) {
        return -1;
    }
    return 0;
}
#else
void bflb_sec_mutex_init(void)
{
    return 0;
}

int bflb_sec_aes_mutex_take(void)
{
    return 0;
}

int bflb_sec_aes_mutex_give(void)
{
    return 0;
}

int bflb_sec_sha_mutex_take(void)
{
    return 0;
}

int bflb_sec_sha_mutex_give(void)
{
    return 0;
}

int bflb_sec_pka_mutex_take(void)
{
    return 0;
}

int bflb_sec_pka_mutex_give(void)
{
    return 0;
}
#endif