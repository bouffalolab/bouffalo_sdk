#ifndef TTY_H_8OXTIMDC
#define TTY_H_8OXTIMDC

#ifdef BL_INTF_SDIO
#include "bl_sdio_eth.h"
#else
#include "bl_usb_eth.h"
#endif

int bl_handle_ext_frame_tty(struct bl_eth_device *dev, const uint8_t *frame, const uint16_t frame_len);

int bl_tty_init(void);
void bl_tty_exit(void);

struct bl_tty_queued_data {
    u16 tty_id;
};

#endif /* end of include guard: TTY_H_8OXTIMDC */
