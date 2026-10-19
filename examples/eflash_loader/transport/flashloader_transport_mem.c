#include "flashloader_transport.h"

#include "flashloader_config.h"
#include "flashloader_port.h"

static volatile uint32_t *const g_mailbox =
    (volatile uint32_t *)(uintptr_t)FLASHLOADER_MAILBOX_ADDR;
static volatile uint32_t *const g_descriptor =
    (volatile uint32_t *)(uintptr_t)FLASHLOADER_DESCRIPTOR_ADDR;
static volatile uint8_t *const g_shared =
    (volatile uint8_t *)(uintptr_t)FLASHLOADER_DATA_ADDR;
static volatile uint16_t *const g_shared_ack =
    (volatile uint16_t *)(uintptr_t)FLASHLOADER_DATA_ADDR;

static void compiler_barrier(void)
{
#if defined(__riscv)
    __asm__ volatile("fence rw, rw" ::: "memory");
#else
    __asm__ volatile("" ::: "memory");
#endif
}

void flashloader_transport_init(void)
{
    g_descriptor[0] = 0U;
    g_descriptor[1] = FLASHLOADER_PROTOCOL_VERSION;
    g_descriptor[2] = FLASHLOADER_CAPABILITIES;
    g_descriptor[3] = FLASHLOADER_SHARED_SIZE;
    *g_mailbox = 0U;
    compiler_barrier();
    /* Magic is the host-visible commit marker for the whole descriptor. */
    g_descriptor[0] = FLASHLOADER_DESCRIPTOR_MAGIC;
    compiler_barrier();
}

int flashloader_transport_receive(uint8_t *frame, size_t capacity,
                                  size_t *frame_length, uint8_t *handshake)
{
    uint16_t payload_length;
    size_t total_length;

    if ((frame == NULL) || (frame_length == NULL) || (handshake == NULL)) {
        return -1;
    }

    while (*g_mailbox != FLASHLOADER_HOST_READY) {
        __asm__ volatile("nop");
    }
    compiler_barrier();

    /* FlashCube sends one dummy byte, not a command frame, for handshake. */
    if (g_shared[0] == 0U) {
        *handshake = 1U;
        *frame_length = 0U;
        return 0;
    }

    payload_length = (uint16_t)g_shared[2] | ((uint16_t)g_shared[3] << 8);
    total_length = (size_t)payload_length + 4U;
    if ((payload_length > FLASHLOADER_MAX_PAYLOAD) ||
        (total_length > capacity)) {
        total_length = 4U;
    }

    for (size_t i = 0U; i < total_length; i++) {
        frame[i] = g_shared[i];
    }
    *handshake = 0U;
    *frame_length = total_length;
    return 0;
}

int flashloader_transport_send(const uint8_t *data, size_t length)
{
    if ((data == NULL) || (length > FLASHLOADER_SHARED_SIZE)) {
        return -1;
    }

    /* Publish the two-byte ACK last so PD can be replaced without a torn ACK. */
    for (size_t i = 2U; i < length; i++) {
        g_shared[i] = data[i];
    }
    if (length >= 2U) {
        *g_shared_ack = (uint16_t)data[0] | ((uint16_t)data[1] << 8);
    } else if (length == 1U) {
        g_shared[0] = data[0];
    }
    compiler_barrier();
    *g_mailbox = FLASHLOADER_SERVER_ACK;
    compiler_barrier();
    return 0;
}

void flashloader_transport_send_pending(void)
{
    static const uint8_t pending[] = { 'P', 'D' };

    (void)flashloader_transport_send(pending, sizeof(pending));
}

#if (FLASHLOADER_CAPABILITIES & FLASHLOADER_CAP_HOST_ACK) != 0U
void flashloader_transport_wait_response_consumed(void)
{
    uint64_t start = flashloader_port_time_ms();

    while ((*g_mailbox == FLASHLOADER_SERVER_ACK) &&
           ((flashloader_port_time_ms() - start) <
            FLASHLOADER_HOST_ACK_TIMEOUT_MS)) {
        __asm__ volatile("nop");
    }
    compiler_barrier();
}
#endif
