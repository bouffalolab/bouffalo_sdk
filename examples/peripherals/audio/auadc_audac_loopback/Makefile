SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= ./../../../..

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-
CONFIG_AUDIO_MIC_PDM_MODE ?= n

# add custom cmake definition
#cmake_definition+=-Dxxx=sss
cmake_definition+=-DCONFIG_AUDIO_MIC_PDM_MODE=$(CONFIG_AUDIO_MIC_PDM_MODE)

-include $(SDK_DEMO_PATH)/.config
include $(BL_SDK_BASE)/project.build
