#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rpmsg_service.h>

#define APP_TASK_STACK_SIZE (1024)
static StackType_t thread_stack[APP_TASK_STACK_SIZE];
static StaticTask_t thread_buffer;
static TaskHandle_t thread_handle;

static volatile unsigned int received_data;
static volatile unsigned int ep2_data;
static volatile unsigned int ep3_data;
static SemaphoreHandle_t data_rx_sem;
static SemaphoreHandle_t ep2_rx_sem;
static SemaphoreHandle_t ep3_rx_sem;

static int ep1_endpoint_cb(struct rpmsg_endpoint *ept, void *data,
        size_t len, uint32_t src, void *priv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    received_data = *((unsigned int *) data);

    if (xPortIsInsideInterrupt()) {
        xSemaphoreGiveFromISR(data_rx_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xSemaphoreGive(data_rx_sem);
    }

    return RPMSG_SUCCESS;
}

/* EP2 endpoint callback - simple integer exchange */
static int ep2_endpoint_cb(struct rpmsg_endpoint *ept, void *data,
        size_t len, uint32_t src, void *priv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    ep2_data = *((unsigned int *) data);

    if (xPortIsInsideInterrupt()) {
        xSemaphoreGiveFromISR(ep2_rx_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xSemaphoreGive(ep2_rx_sem);
    }

    return RPMSG_SUCCESS;
}

/* EP3 endpoint callback - simple integer exchange */
static int ep3_endpoint_cb(struct rpmsg_endpoint *ept, void *data,
        size_t len, uint32_t src, void *priv)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    ep3_data = *((unsigned int *) data);

    if (xPortIsInsideInterrupt()) {
        xSemaphoreGiveFromISR(ep3_rx_sem, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        xSemaphoreGive(ep3_rx_sem);
    }

    return RPMSG_SUCCESS;
}

/* Endpoint IDs */
static int ep1_ep_id;
static int ep2_ep_id;
static int ep3_ep_id;

static unsigned int receive_message(void)
{
    xSemaphoreTake(data_rx_sem, portMAX_DELAY);
    return received_data;
}

/* Message reception functions */
static unsigned int receive_ep1_message(void)
{
    xSemaphoreTake(data_rx_sem, portMAX_DELAY);
    return received_data;
}

static unsigned int receive_ep2_message(void)
{
    xSemaphoreTake(ep2_rx_sem, portMAX_DELAY);
    return ep2_data;
}

static unsigned int receive_ep3_message(void)
{
    xSemaphoreTake(ep3_rx_sem, portMAX_DELAY);
    return ep3_data;
}

/* Message sending functions */
static int send_ep1_message(unsigned int message)
{
    return rpmsg_service_send(ep1_ep_id, &message, sizeof(message));
}

static int send_ep2_message(unsigned int message)
{
    return rpmsg_service_send(ep2_ep_id, &message, sizeof(message));
}

static int send_ep3_message(unsigned int message)
{
    return rpmsg_service_send(ep3_ep_id, &message, sizeof(message));
}

static void app_task(void *param)
{
    (void)param;

    int status = 0;
    unsigned int ep1_message = 0U;
    unsigned int ep2_message = 100U;
    unsigned int ep3_message = 200U;

    #if 0
    /* Wait for all endpoints to be bound */
    while (!rpmsg_service_endpoint_is_bound(ep1_ep_id) ||
           !rpmsg_service_endpoint_is_bound(ep2_ep_id) ||
           !rpmsg_service_endpoint_is_bound(ep3_ep_id)) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
#endif

    /* Run different application loops in parallel */
    for (;;) {
        /* EP1 message exchange loop - range 0-99 */
        if (ep1_message < 99) {
            ep1_message = receive_ep1_message();
            printf("[Remote] EP1 received: %d\n", ep1_message);

            if (ep1_message < 99) {
                ep1_message++;
                status = send_ep1_message(ep1_message);
            }
        }

        /* EP2 data exchange loop - range 100-199 */
        if (ep2_message < 199) {
            ep2_message = receive_ep2_message();
            printf("[Remote] EP2 received: %d\n", ep2_message);

            if (ep2_message < 199) {
                ep2_message++;
                status = send_ep2_message(ep2_message);
            }
        }

        /* EP3 operations loop - range 200-299 */
        if (ep3_message < 299) {
            ep3_message = receive_ep3_message();
            printf("[Remote] EP3 received: %d\n", ep3_message);

            if (ep3_message < 299) {
                ep3_message++;
                status = send_ep3_message(ep3_message);
            }
        }

        if (ep3_message >= 299) {
            break; /* Exit after reasonable number of iterations */
        }
    }

    printf("RPMsg Service remote demo ended.\n");
    vTaskDelete(NULL);
}

static int rpmsg_remote_app(void)
{
    /* Create semaphores */
    data_rx_sem = xSemaphoreCreateBinary();
    if (!data_rx_sem) {
        return -1;
    }

    ep2_rx_sem = xSemaphoreCreateBinary();
    if (!ep2_rx_sem) {
        return -1;
    }

    ep3_rx_sem = xSemaphoreCreateBinary();
    if (!ep3_rx_sem) {
        return -1;
    }

    /* Create application task */
    thread_handle = xTaskCreateStatic(
        app_task,                    /* Task function */
        "app_task",                  /* Task name */
        APP_TASK_STACK_SIZE,         /* Stack size */
        NULL,                        /* Parameter */
        configMAX_PRIORITIES - 1,    /* Priority */
        thread_stack,                /* Stack buffer */
        &thread_buffer               /* Task control block */
    );

    if (!thread_handle) {
        return -1;
    }

    return 0;
}

/* Make sure we register endpoint before RPMsg Service is initialized. */
static int register_endpoint(void)
{
    int status;

    status = rpmsg_service_register_endpoint("ep1", ep1_endpoint_cb);

    if (status < 0) {
        printf("rpmsg_create_ept for ep1 failed %d\n", status);
        return status;
    }

    ep1_ep_id = status;
    printf("Remote registered ep1 endpoint with ID: %d\n", ep1_ep_id);

    status = rpmsg_service_register_endpoint("ep2", ep2_endpoint_cb);
    if (status < 0) {
        printf("rpmsg_create_ept for ep2 failed %d\n", status);
        return status;
    }
    ep2_ep_id = status;
    printf("Remote registered ep2 endpoint with ID: %d\n", ep2_ep_id);

    status = rpmsg_service_register_endpoint("ep3", ep3_endpoint_cb);
    if (status < 0) {
        printf("rpmsg_create_ept for ep3 failed %d\n", status);
        return status;
    }
    ep3_ep_id = status;
    printf("Remote registered ep3 endpoint with ID: %d\n", ep3_ep_id);

    return 0;
}

/* Initialize the system */
int remote_rpmsg_init(void)
{
    int ret;

    /* Register endpoint */
    ret = register_endpoint();
    if (ret < 0) {
        return ret;
    }

    /* Start RPMsg service first */
    ret = rpmsg_service_start();
    if (ret < 0) {
        return ret;
    }

    /* Create application tasks */
    ret = rpmsg_remote_app();
    if (ret < 0) {
        return ret;
    }

    return 0;
}
