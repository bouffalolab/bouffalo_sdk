#include "bflb_mtimer.h"
#include "board.h"
#include "mbedtls/aes.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

int main(void)
{
    board_init();

    mbedtls_aes_self_test(1);
    mbedtls_sha1_self_test(1);
    mbedtls_sha256_self_test(1);
    mbedtls_sha512_self_test(1);

    printf("mbedtls test success\r\n");
    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
