SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath  ./../../..)

export BL_SDK_BASE

CROSS_COMPILE ?= riscv64-unknown-elf-


include $(BL_SDK_BASE)/project.build
$(shell ./tools/genromfs/genromfs -d romfs/ -f ./build/build_out/romfs.bin)
