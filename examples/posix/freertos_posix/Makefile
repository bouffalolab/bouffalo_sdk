SDK_DEMO_PATH ?= $(abspath .)
BL_SDK_BASE ?= $(abspath ./../../..)

export BL_SDK_BASE


# TARGET parameter:
#   not specified or all - compile all tests (default)
#   01_pthread ~ 09_sched_unistd - compile single test
TARGET ?= all

# Select compilation mode based on TARGET
ifeq ($(TARGET),all)
    # Compile all tests
    export POSIX_TEST_MODE = all
    export POSIX_TEST_TARGET =
else
    # Compile single test
    export POSIX_TEST_MODE = single
    export POSIX_TEST_TARGET = $(TARGET)
endif

include $(BL_SDK_BASE)/project.build
