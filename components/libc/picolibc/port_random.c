#include <stdint.h>

/* POSIX random 要求返回 0 到 2^31-1 (2147483647) */
#define RANDOM_MAX 2147483647

static uint32_t g_random_seed = 1;

/* 设置种子 */
void srandom(unsigned seed) {
    g_random_seed = seed ? seed : 1;  /* POSIX: seed为0时视为1 */
}

/* 返回 0 到 2147483647 之间的随机数 */
long random(void) {
    /* 最简单的线性同余，但保证31位输出 */
    g_random_seed = (g_random_seed * 1103515245 + 12345) & RANDOM_MAX;
    return (long)g_random_seed;
}
