#ifndef _SEC_MUTEX_H
#define _SEC_MUTEX_H

#include "bflb_core.h"

void bflb_sec_mutex_init(void);
int bflb_sec_aes_mutex_take(void);
int bflb_sec_aes_mutex_give(void);
int bflb_sec_sha_mutex_take(void);
int bflb_sec_sha_mutex_give(void);
int bflb_sec_pka_mutex_take(void);
int bflb_sec_pka_mutex_give(void);

#endif