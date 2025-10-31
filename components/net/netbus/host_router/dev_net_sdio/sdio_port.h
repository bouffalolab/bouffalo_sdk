
#ifndef __SDIO_PORT_H__
#define __SDIO_PORT_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <hostrouter_log.h>

#ifdef CFG_BL616
#define SDIO_MAWOC(addr) ((((uintptr_t)(addr) & ~0xF0000000) | 0x20000000))
#else
#define SDIO_MAWOC(addr) (addr)
#endif

void sdio_glb_reset(void);
void sdio_gpio_init(void);

uint8_t sdu_read_s_reg(uint32_t offset);
void sdu_write_s_reg(uint32_t offset, uint8_t value);

uint32_t sdio_get_rxbuf(uint8_t port_num);
void sdio_attach_rxbuf(uint8_t fn_num, uint8_t port_num);

void sdio_host_check_init(void);

void gen_card2host_int(void);

void hr_coredump(void);

#endif
