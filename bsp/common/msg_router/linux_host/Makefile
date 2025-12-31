CONFIG_MODULE_SIG=n
MODULE_NAME = sdio_msg_router

# Network device module enable
CONFIG_MR_NETDEV?=y

# TTY module and user command enable
CONFIG_MR_TTY?=y
CONFIG_MR_TTY_CMD?=y
CONFIG_MR_TTY_USER_1?=y
CONFIG_MR_TTY_USER_2?=y
CONFIG_MR_TTY_USER_3?=y

# Speed test module enable
CONFIG_MR_SPEED_TEST?=y

# Bootrom download module enable
CONFIG_MR_BOOTROM?=n

# It must be equal to the slave machine's setting
CONFIG_MR_SDIO_QUEUE_DEPTH?=4

# Source files organized by functional modules
SRC := sdio_drv/mr_sdio_manage.c sdio_drv/mr_sdio2.c sdio_drv/mr_sdio3.c
SRC += msg_ctrl/mr_msg_ctrl.c
SRC += debugfs/mr_debugfs.c

# Network device module
ifeq ($(CONFIG_MR_NETDEV),y)
	ccflags-y += -DCONFIG_MR_NETDEV
	SRC += eth_netdev/mr_netdev.c
endif

# TTY module
ifeq ($(CONFIG_MR_TTY),y)
	ccflags-y += -DCONFIG_MR_TTY
	SRC += tty/mr_tty.c

	ifeq ($(CONFIG_MR_TTY_CMD),y)
		ccflags-y += -DCONFIG_MR_TTY_CMD
	endif
	ifeq ($(CONFIG_MR_TTY_USER_1),y)
		ccflags-y += -DCONFIG_MR_TTY_USER_1
	endif
	ifeq ($(CONFIG_MR_TTY_USER_2),y)
		ccflags-y += -DCONFIG_MR_TTY_USER_2
	endif
	ifeq ($(CONFIG_MR_TTY_USER_3),y)
		ccflags-y += -DCONFIG_MR_TTY_USER_3
	endif
endif

# Speed test module
ifeq ($(CONFIG_MR_SPEED_TEST),y)
	ccflags-y += -DCONFIG_MR_SPEED_TEST
	SRC += speed_test/mr_speed_test.c
endif

# Bootrom download module
ifeq ($(CONFIG_MR_BOOTROM),y)
	ccflags-y += -DCONFIG_MR_BOOTROM
	SRC += bootrom/mr_bootrom.c 
endif

# SDIO queue depth configuration
ccflags-y += -DCONFIG_MR_SDIO2_QUEUE_DEPTH=$(CONFIG_MR_SDIO_QUEUE_DEPTH)

# Include paths for modular headers
ccflags-y += -I$(PWD)/bootrom
ccflags-y += -I$(PWD)/sdio_drv  
ccflags-y += -I$(PWD)/msg_ctrl
ccflags-y += -I$(PWD)/eth_netdev
ccflags-y += -I$(PWD)/speed_test
ccflags-y += -I$(PWD)/debugfs
ccflags-y += -I$(PWD)/tty

KVERSION = $(shell uname -r)
KDIR :=  /lib/modules/$(KVERSION)/build
$(MODULE_NAME)-objs := $(patsubst %.c,%.o,$(SRC))
obj-m := $(MODULE_NAME).o
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules -j4

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: all clean

