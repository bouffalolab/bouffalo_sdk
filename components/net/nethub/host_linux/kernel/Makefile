CONFIG_MODULE_SIG=n
MODULE_NAME = hd_sdio
ccflags-y := -DCONFIG_SDIO_DBG

# Source files organized by functional modules
SRC := sdio_drv/hd_sdio_manage.o sdio_drv/hd_sdio2.o sdio_drv/hd_sdio3.o
SRC += bootrom/hd_bootrom.o
SRC += msg_ctrl/hd_msg_ctrl.o
SRC += debugfs/hd_debugfs.o
SRC += speed_test/hd_speed_test.o
SRC += eth_netdev/hd_eth_netdev.o
SRC += tty/hd_tty.o

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
$(MODULE_NAME)-objs = $(SRC:.o=.o)
obj-m := $(MODULE_NAME).o
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules -j4

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

.PHONY: all clean

