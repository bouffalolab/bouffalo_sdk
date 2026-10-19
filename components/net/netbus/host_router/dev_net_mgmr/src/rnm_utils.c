#include <rnm_utils.h>

#include <stdio.h>

#define PRINTF printf
#define NEWLINE "\r\n"

void rnm_hex_dump(const void *data, size_t size)
{
    size_t i;
    unsigned char *d = (unsigned char *)data;
    for (i = 0; i < size; ++i) {
        // last char in a line or of whole data
        if ((i + 1) % 16 == 0 || i + 1 == size) {
            PRINTF("%02X" NEWLINE, d[i]);
        } else {
            PRINTF("%02X ", d[i]);
            if ((i + 1) % 8 == 0) {
                PRINTF(" ");
            }
        }
    }
}
