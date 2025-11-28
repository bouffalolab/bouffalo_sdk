SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../../..)

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-
CONFIG_AUDIO_MIC_PDM_MODE ?= n

cmake_definition+=-DCONFIG_AUDIO_MIC_PDM_MODE=$(CONFIG_AUDIO_MIC_PDM_MODE)

include $(BL_SDK_BASE)/project.build
