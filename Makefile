BOARD?=bl706_iot
APP?=helloworld
CHIP?=bl702
INTERFACE?=jlink
BAUDRATE ?=12000

SUPPORT_SHELL?=n
SUPPORT_FREERTOS?=n
SUPPORT_LVGL?=n
SUPPORT_FLOAT?=n

export BOARD
export APP
export SUPPORT_SHELL
export SUPPORT_FREERTOS
export SUPPORT_LVGL
export SUPPORT_FLOAT

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

