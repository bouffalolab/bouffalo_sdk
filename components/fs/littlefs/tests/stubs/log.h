/* Host tests replace only the device logging boundary. */
#ifndef LFS_KV_TEST_LOG_H
#define LFS_KV_TEST_LOG_H
#include <stdio.h>
#define LOG_E(...) fprintf(stderr, __VA_ARGS__)
#endif
