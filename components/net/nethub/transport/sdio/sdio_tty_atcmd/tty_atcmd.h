#ifndef _TTY_ATCMD_H_
#define _TTY_ATCMD_H_

#include "mr_tty.h"

/* TTY Command Channel Configuration */
#define TTY_ATCMD_SLOT_CNT   (4)
#define TTY_ATCMD_MTU        (1500)

typedef int (*atcmd_dnld_cb_t)(void *arg, uint8_t *data_buff, uint16_t data_size);

extern mr_tty_priv_t *g_tty_atcmd_priv;

int tty_atcmd_init(mr_msg_ctrl_priv_t *msg_ctrl);
int tty_atcmd_dnld_recv_register(atcmd_dnld_cb_t dnld_cb, void *arg);
int tty_atcmd_upld_send(uint8_t *data_buff, uint16_t data_size);

#endif
