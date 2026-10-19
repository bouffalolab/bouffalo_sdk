#include <stdint.h>

uint32_t spisync_checksum(void *buf, uint32_t len)
{
    uint32_t sum = 0;
    uint16_t *data = (uint16_t *)buf;

    // buf
    for (uint32_t i = 0; i < len / 2; i++) {
        sum += data[i];
    }
    if (len % 2) { // Handle odd length
        sum += ((uint8_t *)buf)[len - 1] << 8; // Add last byte
    }

    // Wrap around
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint32_t)~sum; // One's complement
}
