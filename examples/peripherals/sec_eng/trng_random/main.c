#include "bflb_mtimer.h"
#include "bflb_sec_trng.h"
#include "board.h"

static void trng_dump_data(uint8_t *data, uint8_t len)
{
    int i;
    printf("TRNG data \r\n");
    for (i = 0; i < len; i++) {
        printf("%02x ", data[i]);
    }
    printf("\r\n");
}

int main(void)
{
    uint8_t data[32];

    board_init();

    struct bflb_device_s *trng;

    trng = bflb_device_get_by_name("trng");

    while (1) {
        bflb_trng_read(trng, data);
        trng_dump_data(data, 32);
        bflb_mtimer_delay_ms(2000);
    }
}