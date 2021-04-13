################# Add global include #################
list(APPEND ADD_INCLUDE 
"${CMAKE_CURRENT_SOURCE_DIR}/hal_drv/inc"
"${CMAKE_CURRENT_SOURCE_DIR}/std_drv/inc"
"${CMAKE_CURRENT_SOURCE_DIR}/regs"
"${CMAKE_CURRENT_SOURCE_DIR}/startup"
"${CMAKE_CURRENT_SOURCE_DIR}/risc-v/Core/Include"
)
#######################################################

################# Add private include #################
list(APPEND ADD_PRIVATE_INCLUDE 
"${CMAKE_CURRENT_SOURCE_DIR}/hal_drv/default_config"
)
#######################################################

############## Add current dir source files ###########
file(GLOB_RECURSE sources 
"${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/hal_drv/src/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/startup/interrupt.c"
"${CMAKE_CURRENT_SOURCE_DIR}/startup/system_bl702.c"
"${CMAKE_CURRENT_SOURCE_DIR}/startup/GCC/entry.S"
"${CMAKE_CURRENT_SOURCE_DIR}/startup/GCC/start_load.c"
)
list(APPEND ADD_SRCS  ${sources})
# aux_source_directory(src ADD_SRCS)
list(REMOVE_ITEM ADD_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/std_drv/src/bl702_snflash.c"
)
#######################################################

########### Add required/dependent components #########
list(APPEND ADD_REQUIREMENTS common)
#######################################################

############ Add static libs ##########################
# if(CONFIG_COMPONENT1_INCLUDE_STATIC_LIB)
#     list(APPEND ADD_STATIC_LIB "lib/libtest.a")
# endif()
#######################################################

############ Add dynamic libs #########################
# list(APPEND ADD_DYNAMIC_LIB "lib/arch/v831/libmaix_nn.so"
#                             "lib/arch/v831/libmaix_cam.so"
# )
#######################################################

############ Add global compile option ################
#add components denpend on this component
list(APPEND ADD_DEFINITIONS -DARCH_RISCV)
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules  
# list(APPEND ADD_DEFINITIONS_PRIVATE -DAAAAA=1)
#######################################################

generate_library()
