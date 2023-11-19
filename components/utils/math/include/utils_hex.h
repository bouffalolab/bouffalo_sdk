#ifndef __UTILS_HEX_H__
#define __UTILS_HEX_H__
char *utils_bin2hex(char *dst, const void *src, size_t count);
size_t utils_hex2bin(const char *hex, size_t hexlen, uint8_t *buf, size_t buflen);
#endif
