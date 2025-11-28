SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../../..)

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-

CONFIG_LINK_IN_PSRAM ?= 1
cmake_definition+=-DCONFIG_LINK_IN_PSRAM=$(CONFIG_LINK_IN_PSRAM)

include $(BL_SDK_BASE)/project.build
