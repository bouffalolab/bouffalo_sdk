BOARD?=bl706_iot
CHIP?=bl702
APP_DIR?=examples
APP?=helloworld
CPU_ID?=none
COMx?=

SUPPORT_FLOAT?=n
INTERFACE?=uart
BAUDRATE ?=2000000

FORMAT_DIR?=.

export BOARD
export CHIP
export APP_DIR
export APP
export CPU_ID
export SUPPORT_FLOAT

# The command to remove a file.
RM = cmake -E rm -rf

build:
	cmake -E make_directory build
	cmake -E copy project.build build
	make -C ./build -f ./project.build

download:
	./tools/bflb_flash_tool/bflb_mcu_tool --chipname=$(CHIP) --interface=$(INTERFACE) --port=$(COMx) --baudrate=$(BAUDRATE)

format:
	find $(FORMAT_DIR)/ -name "*.c" -o -name "*.h" -o -name "*.cc" -o -name "*.cpp"| xargs clang-format -style=file -i

clean:
	$(RM) out
	$(RM) build

.PHONY:build clean download format

