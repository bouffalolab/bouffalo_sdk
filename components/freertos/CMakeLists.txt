################# Add global include #################
list(APPEND ADD_INCLUDE 
"${CMAKE_CURRENT_SOURCE_DIR}"
"${CMAKE_CURRENT_SOURCE_DIR}/include"
"${CMAKE_CURRENT_SOURCE_DIR}/portable/gcc/risc-v/${CHIP}"
)
#######################################################

################# Add private include #################
# list(APPEND ADD_PRIVATE_INCLUDE 
# )
#######################################################

############## Add current dir source files ###########
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/croutine.c"
"${CMAKE_CURRENT_SOURCE_DIR}/croutine.c"
"${CMAKE_CURRENT_SOURCE_DIR}/event_groups.c"
"${CMAKE_CURRENT_SOURCE_DIR}/list.c"
"${CMAKE_CURRENT_SOURCE_DIR}/queue.c"
"${CMAKE_CURRENT_SOURCE_DIR}/tasks.c"
"${CMAKE_CURRENT_SOURCE_DIR}/timers.c"
"${CMAKE_CURRENT_SOURCE_DIR}/portable/memmang/heap_5.c"
"${CMAKE_CURRENT_SOURCE_DIR}/portable/gcc/risc-v/${mcu}/port.c"
"${CMAKE_CURRENT_SOURCE_DIR}/portable/gcc/risc-v/${mcu}/portASM.S"
)
list(APPEND ADD_SRCS  ${sources})
# aux_source_directory(src ADD_SRCS)
# list(REMOVE_ITEM ADD_SRCS "${CMAKE_CURRENT_SOURCE_DIR}")
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
list(APPEND ADD_DEFINITIONS -DportasmHANDLE_INTERRUPT=FreeRTOS_Interrupt_Handler)
#######################################################

############ Add private compile option ################
#add compile option for this component that won't affect other modules  
# list(APPEND ADD_DEFINITIONS_PRIVATE -Dxxx)
#######################################################

generate_library()
