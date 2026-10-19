SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

CHIP ?= bl616
BOARD ?= bl616dk
MACSW_CONFIG_SELECT ?= default

# add custom cmake definition
# cmake_definition+=-Dxxx=sss

CONFIG_BLE_ENABLE ?= y
cmake_definition+=-DCONFIG_BLE_ENABLE=$(CONFIG_BLE_ENABLE)

CONFIG_USBH_MSC ?= n

include $(BL_SDK_BASE)/project.build
