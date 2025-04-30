#ifndef SDIOWIFI_TTY_H_P6XHQNXN
#define SDIOWIFI_TTY_H_P6XHQNXN

#if SDIOWIFI_TTY 

#include <stdbool.h>
//#include <sdiowifi_platform_adapt.h>

#include "sdiowifi_mgmr.h"

#define SDIOWIFI_TTY_DEV_COUNT 1

#define SDIOWIFI_TTY_RCV_BUF_LEN (4 * 1024)
#define SDIOWIFI_TTY_TRIGGER_LEVEL 1

#define SDIOWIFI_TTY_PATH "/dev/ttysd0"

#define SDIOWIFI_TTY_MIN(x, y) ((x) < (y) ? (x) : (y))

#define TTY_RCV_BUF_REM_UNIT           128
#define TTY_RCV_BUF_BYTES_TO_REM(val)  ((val) / TTY_RCV_BUF_REM_UNIT)

#if 255 * TTY_RCV_BUF_REM_UNIT < SDIOWIFI_TTY_RCV_BUF_LEN
#error "Adjust SDIOWIFI_TTY_RCV_BUF_LEN and TTY_RCV_BUF_REM_UNIT"
#endif

int sdiowifi_tty_init(struct sdiowifi_mgmr *sdm, int id, const char *path);
void sdiowifi_ext_frame_handle_tty_rx(struct sdiowifi_mgmr *sdm, uint16_t tty_id, uint8_t *msg, int msg_len);

int sdiowifi_tty_send_bsr(struct sdiowifi_mgmr *sdm, uint16_t tty, bool initial, uint16_t incr);

/**
 * @brief Start TTY test task.
 */
void sdiowifi_tty_test(void);

#endif

#endif /* end of include guard: SDIOWIFI_TTY_H_P6XHQNXN */
