################# Add global include #################
list(APPEND ADD_INCLUDE 
"${CMAKE_CURRENT_SOURCE_DIR}/libc/inc" 
"${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer" 
"${CMAKE_CURRENT_SOURCE_DIR}/soft_crc" 
"${CMAKE_CURRENT_SOURCE_DIR}/memheap"
"${CMAKE_CURRENT_SOURCE_DIR}/libc/inc/arm_gcc"
"${CMAKE_CURRENT_SOURCE_DIR}/libc/inc/bits"
"${CMAKE_CURRENT_SOURCE_DIR}/libc/inc/sys"
"${CMAKE_CURRENT_SOURCE_DIR}/misc"
"${CMAKE_CURRENT_SOURCE_DIR}/list"
"${CMAKE_CURRENT_SOURCE_DIR}/device"
"${CMAKE_CURRENT_SOURCE_DIR}/partition"
)
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE 
# )
#######################################################

############## Add current dir source files ###########
file(GLOB_RECURSE sources 
"${CMAKE_CURRENT_SOURCE_DIR}/libc/src/*.c" 
"${CMAKE_CURRENT_SOURCE_DIR}/ring_buffer/*.c" 
"${CMAKE_CURRENT_SOURCE_DIR}/soft_crc/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/memheap/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/misc/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/device/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/partition/*.c"
)

#aux_source_directory(. sources)
list(APPEND ADD_SRCS  ${sources})
list(REMOVE_ITEM ADD_SRCS 
"${CMAKE_CURRENT_SOURCE_DIR}/libc/src/strtox.c"
"${CMAKE_CURRENT_SOURCE_DIR}/libc/src/atox.c"
"${CMAKE_CURRENT_SOURCE_DIR}/libc/src/stdlib.c")
#######################################################

########### Add required/dependent components #########
#list(APPEND ADD_REQUIREMENTS xxx)
#######################################################

############ Add static libs ##########################
# if(CONFIG_COMPONENT1_INCLUDE_STATIC_LIB)
#     list(APPEND ADD_STATIC_LIB "libxxx.a")
# endif()
#######################################################

############ Add dynamic libs #########################
# list(APPEND ADD_DYNAMIC_LIB "libxxx.so"
# )
#######################################################

############ Add global compile option ################
#add components denpend on this component
# list(APPEND ADD_DEFINITIONS -Dxxx)
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules  
# list(APPEND ADD_DEFINITIONS_PRIVATE -Dxxx)
#######################################################

generate_library()
