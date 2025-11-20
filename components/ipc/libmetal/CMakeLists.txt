
sdk_generate_library()

set(LIBMETAL_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/libmetal)
set(LIBMETAL_LIB_DIR ${LIBMETAL_ROOT}/lib)

set(VERSION_FILE ${LIBMETAL_ROOT}/VERSION)
if(EXISTS ${VERSION_FILE})
    file(READ ${VERSION_FILE} VERSION_CONTENT)
    string(REGEX MATCH "VERSION_MAJOR = ([0-9]+)" _ ${VERSION_CONTENT})
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_MINOR = ([0-9]+)" _ ${VERSION_CONTENT})
    set(VERSION_MINOR ${CMAKE_MATCH_1})
    string(REGEX MATCH "VERSION_PATCH = ([0-9]+)" _ ${VERSION_CONTENT})
    set(VERSION_PATCH ${CMAKE_MATCH_1})
    set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
    message(STATUS "Read libmetal version ${VERSION_STRING} from VERSION file")
else()
    set(VERSION_MAJOR 1)
    set(VERSION_MINOR 8)
    set(VERSION_PATCH 0)
    set(VERSION_STRING "1.8.0")
    message(STATUS "Using default libmetal version ${VERSION_STRING}")
endif()

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/metal/system/freertos)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/metal/compiler/gcc)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/metal/processor/generic)

macro(process_template_file input_file output_file)
    file(READ ${input_file} content)
    string(REPLACE "@PROJECT_SYSTEM@" "freertos" content "${content}")
    string(REPLACE "@PROJECT_PROCESSOR@" "generic" content "${content}")
    string(REPLACE "@PROJECT_MACHINE@" "Generic" content "${content}")
    string(REPLACE "@PROJECT_SYSTEM_UPPER@" "FREERTOS" content "${content}")
    string(REPLACE "@PROJECT_PROCESSOR_UPPER@" "GENERIC" content "${content}")
    string(REPLACE "@PROJECT_MACHINE_UPPER@" "GENERIC" content "${content}")
    string(REPLACE "@PROJECT_VERSION_MAJOR@" "${VERSION_MAJOR}" content "${content}")
    string(REPLACE "@PROJECT_VERSION_MINOR@" "${VERSION_MINOR}" content "${content}")
    string(REPLACE "@PROJECT_VERSION_PATCH@" "${VERSION_PATCH}" content "${content}")
    string(REPLACE "@PROJECT_VERSION@" "${VERSION_STRING}" content "${content}")

    string(REGEX REPLACE "#cmakedefine HAVE_STDATOMIC_H" "/* #undef HAVE_STDATOMIC_H */" content "${content}")
    string(REGEX REPLACE "#cmakedefine HAVE_FUTEX_H" "/* #undef HAVE_FUTEX_H */" content "${content}")
    string(REGEX REPLACE "#cmakedefine HAVE_PROCESSOR_ATOMIC_H" "/* #undef HAVE_PROCESSOR_ATOMIC_H */" content "${content}")
    string(REGEX REPLACE "#cmakedefine HAVE_PROCESSOR_CPU_H" "/* #undef HAVE_PROCESSOR_CPU_H */" content "${content}")

    file(WRITE ${output_file} "${content}")
endmacro()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libmetal/lib/config.h ${CMAKE_CURRENT_BINARY_DIR}/metal/lib/config.h COPYONLY)

file(GLOB_RECURSE ALL_LIB_HEADERS
     "${LIBMETAL_LIB_DIR}/*.h"
     "${LIBMETAL_LIB_DIR}/system/freertos/*.h"
     "${LIBMETAL_LIB_DIR}/compiler/gcc/*.h"
     "${LIBMETAL_LIB_DIR}/processor/generic/*.h")

foreach(header_file ${ALL_LIB_HEADERS})
    file(RELATIVE_PATH rel_path ${LIBMETAL_LIB_DIR} ${header_file})
    set(output_path ${CMAKE_CURRENT_BINARY_DIR}/metal/${rel_path})
    get_filename_component(output_dir ${output_path} DIRECTORY)
    file(MAKE_DIRECTORY ${output_dir})
    process_template_file(${header_file} ${output_path})
endforeach()

sdk_add_private_include_directories(
    #${CMAKE_CURRENT_BINARY_DIR}/metal
    #${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/metal/system/freertos
    ${CMAKE_CURRENT_BINARY_DIR}/metal/compiler/gcc
    ${CMAKE_CURRENT_BINARY_DIR}/metal/processor/generic
)

sdk_add_include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

sdk_add_compile_definitions(
    -DMETAL_INTERNAL
    -DDEFAULT_LOGGER_ON
    #/*The following macros will be moved to kconfig in the future*/
    -DVDEV_START_ADDR=0x210c8000
    -DVDEV_SIZE=0x4000
    -DVDEV_STATUS_ADDR=VDEV_START_ADDR
    -DVDEV_STATUS_SIZE=0x400
    -DSHM_START_ADDR=VDEV_START_ADDR+VDEV_STATUS_SIZE
    -DSHM_SIZE=VDEV_SIZE-VDEV_STATUS_SIZE
    -DVRING_COUNT=2
    -DVRING_RX_ADDRESS=VDEV_START_ADDR+SHM_SIZE-VDEV_STATUS_SIZE
    -DVRING_TX_ADDRESS=VDEV_START_ADDR+SHM_SIZE
    -DVRING_ALIGNMENT=4
    -DVRING_SIZE=13
)

sdk_library_add_sources(
    ${LIBMETAL_LIB_DIR}/init.c
    ${LIBMETAL_LIB_DIR}/device.c
    ${LIBMETAL_LIB_DIR}/dma.c
    ${LIBMETAL_LIB_DIR}/io.c
    ${LIBMETAL_LIB_DIR}/irq.c
    ${LIBMETAL_LIB_DIR}/log.c
    ${LIBMETAL_LIB_DIR}/shmem.c
    ${LIBMETAL_LIB_DIR}/softirq.c
    ${LIBMETAL_LIB_DIR}/version.c
    ${LIBMETAL_LIB_DIR}/system/freertos/condition.c
    ${LIBMETAL_LIB_DIR}/system/freertos/device.c
    ${LIBMETAL_LIB_DIR}/system/freertos/init.c
    ${LIBMETAL_LIB_DIR}/system/freertos/io.c
    ${LIBMETAL_LIB_DIR}/system/freertos/irq.c
    #${LIBMETAL_LIB_DIR}/system/freertos/sys.c
    ${LIBMETAL_LIB_DIR}/system/freertos/shmem.c
    ${LIBMETAL_LIB_DIR}/system/freertos/time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port_freertos/sys.c
)

message(STATUS "libmetal enabled with freertos system")
