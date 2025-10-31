
#ifndef __MSP_BL616_GLB_GPIO_H__
#define __MSP_BL616_GLB_GPIO_H__

#include "msp_bl616_platform.h"
#include "msp_bl616_gpio.h"

BL_Err_Type ms_GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg);
BL_Err_Type msp_GLB_GPIO_Write(GLB_GPIO_Type gpioPin, uint32_t val);
#endif

