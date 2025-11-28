cmake_minimum_required(VERSION 3.15)


find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# sdk_add_compile_definitions(-D SDIO3_DEBUG)
# sdk_add_compile_definitions(-D SDIO3_DMA1_MODE_ENABLE=1)
sdk_add_compile_definitions(-D SDIO3_FUNC_NUM_MAX=1)        # 1 or 2
sdk_add_compile_definitions(-D SDIO3_FUNC_QUEUE_NUM_MAX=8)  # must be 2^n && <=16

sdk_set_main_file(main.c)

project(sdio3_test)
