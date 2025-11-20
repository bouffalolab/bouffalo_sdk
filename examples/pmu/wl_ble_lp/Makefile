SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../..

export BL_SDK_BASE

CHIP ?= bl616
BOARD ?= bl616dk
CROSS_COMPILE ?= riscv64-unknown-elf-
MACSW_CONFIG_SELECT ?= default

# add custom cmake definition
# cmake_definition+=-Dxxx=sss

CONFIG_BLE_ENABLE ?= 1
cmake_definition+=-DCONFIG_BLE_ENABLE=$(CONFIG_BLE_ENABLE)

CONFIG_USBH_MSC ?= 0
cmake_definition+=-DCONFIG_USBH_MSC=$(CONFIG_USBH_MSC)

include $(BL_SDK_BASE)/project.build
