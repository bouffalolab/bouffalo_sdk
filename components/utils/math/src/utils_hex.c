#include <stdint.h>
#include <stddef.h>
#include <utils_hex.h>

const char hex_asc[] = "0123456789abcdef";
const char hex_asc_upper[] = "0123456789ABCDEF";
#define hex_asc_lo(x)   hex_asc_upper[((x) & 0x0f)]
#define hex_asc_hi(x)   hex_asc_upper[((x) & 0xf0) >> 4]

static inline char *hex_byte_pack(char *buf, uint8_t byte)
{
    *buf++ = hex_asc_hi(byte);
    *buf++ = hex_asc_lo(byte);
    return buf;
}

static int char2hex(char c, uint8_t *x)
{
	if (c >= '0' && c <= '9') {
		*x = c - '0';
	} else if (c >= 'a' && c <= 'f') {
		*x = c - 'a' + 10;
	} else if (c >= 'A' && c <= 'F') {
		*x = c - 'A' + 10;
	} else {
		return -1;
	}

	return 0;
}

char *utils_bin2hex(char *dst, const void *src, size_t count)
{
    const unsigned char *_src = src;

    while (count--) {
        dst = hex_byte_pack(dst, *_src++);
    }

    return dst;
}

size_t utils_hex2bin(const char *hex, size_t hexlen, uint8_t *buf, size_t buflen)
{
	uint8_t dec;

	if (buflen < hexlen / 2 + hexlen % 2) {
		return 0;
	}

	/* if hexlen is uneven, insert leading zero nibble */
	if (hexlen % 2) {
		if (char2hex(hex[0], &dec) < 0) {
			return 0;
		}
		buf[0] = dec;
		hex++;
		buf++;
	}

	/* regular hex conversion */
	for (size_t i = 0; i < hexlen / 2; i++) {
		if (char2hex(hex[2 * i], &dec) < 0) {
			return 0;
		}
		buf[i] = dec << 4;

		if (char2hex(hex[2 * i + 1], &dec) < 0) {
			return 0;
		}
		buf[i] += dec;
	}

	return hexlen / 2 + hexlen % 2;
}