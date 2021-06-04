BOARD?=bl706_iot
CHIP?=bl702
APP_DIR?=examples
APP?=helloworld
CPU_ID?=m0

SUPPORT_SHELL?=n
SUPPORT_FREERTOS?=n
SUPPORT_LVGL?=n
SUPPORT_FLOAT?=n
SUPPORT_BLE?=n
SUPPORT_XZ?=n

INTERFACE?=jlink
BAUDRATE ?=12000

export BOARD
export CHIP
export APP_DIR
export APP
export CPU_ID
export SUPPORT_SHELL
export SUPPORT_FREERTOS
export SUPPORT_LVGL
export SUPPORT_FLOAT
export SUPPORT_BLE
export SUPPORT_XZ

# The command to remove a file.
RM = cmake -E rm -rf

build:
	cmake -E make_directory build
	cmake -E copy project.build build
	make -C ./build -f ./project.build

download:
	./tools/bflb_flash_tool/bflb_mcu_tool --chipname=$(CHIP) --interface=$(INTERFACE) --baudrate=$(BAUDRATE)
	
clean:
	$(RM) out
	$(RM) build
.PHONY:build clean download

