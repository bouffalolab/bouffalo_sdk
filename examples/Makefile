CHIP ?= bl616
BOARD ?= bl616dk
CPU_ID ?=

peripherals_makefile_files = $(shell find ./peripherals -name 'Makefile' -not -path "*/build/*")
wifi_makefile_files = $(shell find ./wifi/sta -name 'Makefile' -not -path "*/build/*")
btble_makefile_files = $(shell find ./btble -name 'Makefile' -not -path "*/build/*")

makefile_dirs = $(sort $(dir $(peripherals_makefile_files)))

makefile_dirs += ./bflb_block_pool
makefile_dirs += ./bflog/barebone_sync
makefile_dirs += ./bflog/freertos_async
makefile_dirs += ./coremark_v1.01
makefile_dirs += ./freertos
makefile_dirs += ./helloworld
makefile_dirs += ./mbedtls
makefile_dirs += ./memheap
makefile_dirs += ./partition
makefile_dirs += ./posix/freertos
makefile_dirs += ./printf
makefile_dirs += ./shell/shell_no_os
makefile_dirs += ./shell/shell_os

ifeq ($(CHIP),bl602)
filter_out_dirs += ./peripherals/csi/%
filter_out_dirs += ./peripherals/emac/%
filter_out_dirs += ./peripherals/pwm_v2/%
filter_out_dirs += ./peripherals/usbdev/%

endif

ifeq ($(CHIP),bl702)
filter_out_dirs += ./peripherals/csi/%
filter_out_dirs += ./peripherals/emac/%
filter_out_dirs += ./peripherals/pwm_v2/%
endif

ifeq ($(CHIP),bl702L)
filter_out_dirs += ./peripherals/csi/%
filter_out_dirs += ./peripherals/emac/%
filter_out_dirs += ./peripherals/pwm_v2/%
filter_out_dirs += ./peripherals/usbdev/%
endif

ifeq ($(CHIP),bl616)
filter_out_dirs += ./peripherals/csi/%
filter_out_dirs += ./peripherals/pwm_v1/%

makefile_dirs += ./fatfs
makefile_dirs += ./littlefs
makefile_dirs += ./lvgl
makefile_dirs += ./lua

makefile_dirs += $(sort $(dir $(wifi_makefile_files)))
makefile_dirs += $(sort $(dir $(btble_makefile_files)))

endif

ifeq ($(CHIP),bl808)
filter_out_dirs += ./peripherals/pwm_v1/%

makefile_dirs += ./fatfs
makefile_dirs += ./littlefs
makefile_dirs += ./lvgl
makefile_dirs += ./lua
endif

build_dirs = $(filter-out $(filter_out_dirs), $(makefile_dirs))

listall:
	@for n in $(build_dirs); do echo $$n ; done

buildall:
	@for n in $(build_dirs); do make -C $$n || break ; done

cleanall:
	@for n in $(build_dirs); do make -C $$n clean; done

.PHONY: buildall cleanall