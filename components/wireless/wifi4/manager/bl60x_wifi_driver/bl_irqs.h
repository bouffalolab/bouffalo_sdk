
/**
 ****************************************************************************************
 *
 * @file bl_irqs.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */

#ifndef _RWNX_IRQS_H_
#define _RWNX_IRQS_H_
#include <stdint.h>
#include "bl_defs.h"

int bl_irqs_init(struct bl_hw *bl_hw);
int bl_irqs_enable(void);
int bl_irqs_disable(void);
int bl_irqs_notify(void);
int bl_irq_wait_event(uint32_t tick);
void bl_irq_bottomhalf(struct bl_hw *bl_hw);

#endif /* _RWNX_IRQS_H_ */
