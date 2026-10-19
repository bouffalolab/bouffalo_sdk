#ifndef NH_VCHAN_BACKEND_H
#define NH_VCHAN_BACKEND_H

#include "mr_msg_ctrl.h"
#include "nh_profile.h"

int nethub_sdio_vchan_backend_init(mr_msg_ctrl_priv_t *msg_ctrl);

extern const nh_vchan_ops_t nhsdio_vchan_ops;

#endif
