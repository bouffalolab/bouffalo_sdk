#ifndef __LPFW_BCMC_MAILBOX_H__
#define __LPFW_BCMC_MAILBOX_H__

#include <stdint.h>
#include <stddef.h>

#define LPFW_BCMC_MAILBOX_MAGIC      0x42434D43u /* BCMC */
#define LPFW_BCMC_MAILBOX_VERSION    2u
#define LPFW_BCMC_MAILBOX_FIXED_SIZE 24u
#define LPFW_BCMC_PACKET_DESC_SIZE   8u
#define LPFW_BCMC_DESC_CAPACITY      12u
#define LPFW_BCMC_FRAME_SIZE         1536u
#define LPFW_BCMC_PAYLOAD_FRAME_NUM  2u
#define LPFW_BCMC_PAYLOAD_SIZE       (LPFW_BCMC_FRAME_SIZE * LPFW_BCMC_PAYLOAD_FRAME_NUM)

#define LPFW_BCMC_PACKET_FLAG_NONE   0u

#define LPFW_BCMC_ERR_NONE           0u
#define LPFW_BCMC_ERR_DESC_FULL      1u
#define LPFW_BCMC_ERR_PAYLOAD_FULL   2u
#define LPFW_BCMC_ERR_FRAME_TOO_LARGE 3u

struct lpfw_bcmc_packet_desc {
    uint32_t offset;
    uint16_t len;
    uint16_t flags;
};

struct lpfw_bcmc_mailbox {
    uint32_t magic;
    uint16_t version;
    uint16_t header_size;
    uint16_t desc_capacity;
    uint16_t packet_count;
    uint32_t payload_size;
    uint32_t payload_used;
    uint16_t last_error;
    uint16_t drop_count;
    struct lpfw_bcmc_packet_desc desc[];
};

#define LPFW_BCMC_MAILBOX_HEADER_SIZE(desc_capacity) \
    (LPFW_BCMC_MAILBOX_FIXED_SIZE + ((uint32_t)(desc_capacity) * LPFW_BCMC_PACKET_DESC_SIZE))
#define LPFW_BCMC_MAILBOX_POOL_SIZE \
    (LPFW_BCMC_MAILBOX_HEADER_SIZE(LPFW_BCMC_DESC_CAPACITY) + LPFW_BCMC_PAYLOAD_SIZE)

_Static_assert(sizeof(struct lpfw_bcmc_packet_desc) == LPFW_BCMC_PACKET_DESC_SIZE, "BCMC desc ABI size changed");
_Static_assert(sizeof(struct lpfw_bcmc_mailbox) == LPFW_BCMC_MAILBOX_FIXED_SIZE, "BCMC mailbox ABI size changed");
_Static_assert(offsetof(struct lpfw_bcmc_mailbox, magic) == 0, "BCMC mailbox magic offset changed");
_Static_assert(offsetof(struct lpfw_bcmc_mailbox, payload_size) == 12, "BCMC mailbox payload_size offset changed");
_Static_assert(offsetof(struct lpfw_bcmc_mailbox, last_error) == 20, "BCMC mailbox last_error offset changed");
_Static_assert(offsetof(struct lpfw_bcmc_mailbox, desc) == LPFW_BCMC_MAILBOX_FIXED_SIZE, "BCMC mailbox desc offset changed");

#endif
