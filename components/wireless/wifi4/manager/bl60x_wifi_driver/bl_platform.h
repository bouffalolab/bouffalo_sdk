
/**
 ****************************************************************************************
 *
 * @file bl_platform.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef _RWNX_PLAT_H_
#define _RWNX_PLAT_H_
#define BL606_WIFI_PLATFORM_ADDRESS (0x60000000)

#define writel(b,addr) (*(volatile unsigned int *)(addr) = (b))
#define RWNX_REG_WRITE(val, plat, offset)         \
    writel(val, plat + offset)

struct bl_hw;
int bl_platform_on(struct bl_hw *bl_hw);
void bl_platform_off(struct bl_hw *bl_hw);
#endif /* _RWNX_PLAT_H_ */
