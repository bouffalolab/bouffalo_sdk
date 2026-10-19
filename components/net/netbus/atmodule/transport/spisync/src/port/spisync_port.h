#ifndef __spisync_port_H__
#define __spisync_port_H__

#include <stdint.h>
#define TPDBG_ENABLE   (0)

#if TPDBG_ENABLE
#ifndef SHORT_FILE
#define SHORT_FILE \
            (strchr(__FILE__, '\\') \
            ? ((strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)) \
            : ((strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)))
#endif
#define spisync_trace(...)  //printf
#define spisync_dbg           printf
#define spisync_log(M, ...) do { printf("[%u][%s +%d] " M "",\
                                (xPortIsInsideInterrupt())?(xTaskGetTickCountFromISR()):(xTaskGetTickCount()),\
                                SHORT_FILE, __LINE__,\
                                ##__VA_ARGS__);\
                                } while(0==1)
#define spisync_err(M, ...) do { printf("[%u][%s +%d] " M "",\
                                (xPortIsInsideInterrupt())?(xTaskGetTickCountFromISR()):(xTaskGetTickCount()),\
                                SHORT_FILE, __LINE__,\
                                ##__VA_ARGS__);\
                                } while(0==1)
#else
#define spisync_trace(...)
#define spisync_dbg           
#define spisync_log(...)
#define spisync_err(M, ...)
#define spisync_buf(...)
#endif // TPDBG_ENABLE

void spisync_buf(const char *str, void *buf, uint32_t len);

void *malloc_aligned_with_padding(int size, int align_bytes);
void free_aligned_with_padding(void *ptr);
void *malloc_aligned_with_padding_nocache(int size, int align_bytes);
void free_aligned_with_padding_nocache(void *ptr);

#endif

