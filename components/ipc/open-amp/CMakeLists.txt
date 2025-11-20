
sdk_generate_library()

sdk_library_add_sources(open-amp/lib/version.c)
sdk_library_add_sources(open-amp/lib/virtio/virtio.c)
sdk_library_add_sources(open-amp/lib/virtio/virtqueue.c)
sdk_library_add_sources(open-amp/lib/rpmsg/rpmsg.c)
sdk_library_add_sources(open-amp/lib/rpmsg/rpmsg_virtio.c)
sdk_library_add_sources(open-amp/lib/remoteproc/remoteproc.c)
sdk_library_add_sources(open-amp/lib/remoteproc/elf_loader.c)
sdk_library_add_sources(open-amp/lib/remoteproc/rsc_table_parser.c)
sdk_library_add_sources(open-amp/lib/remoteproc/remoteproc_virtio.c)
sdk_library_add_sources(open-amp/lib/utils/utilities.c)

if(CONFIG_OPENAMP_PROXY)
    sdk_library_add_sources(open-amp/lib/proxy/rpmsg_retarget.c)
    sdk_library_add_sources(open-amp/lib/service/rpmsg/rpc/rpmsg_rpc_client.c)
    sdk_library_add_sources(open-amp/lib/service/rpmsg/rpc/rpmsg_rpc_server.c)
    message(STATUS "OpenAMP: rpmsg_retarget.c enabled")
else()
    message(STATUS "OpenAMP: rpmsg_retarget.c disabled to avoid C library conflicts")
endif()

if(CONFIG_OPENAMP_VIRTIO_MMIO_DRV)
    sdk_library_add_sources(open-amp/lib/virtio_mmio/virtio_mmio_drv.c)
    message(STATUS "OpenAMP: virtio_mmio_drv.c enabled")
else()
    message(STATUS "OpenAMP: virtio_mmio_drv.c disabled")
endif()

# Read version from VERSION file
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/open-amp/VERSION ver)

string(REGEX MATCH "VERSION_MAJOR = ([0-9]*)" _ ${ver})
set(PROJECT_VERSION_MAJOR ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_MINOR = ([0-9]*)" _ ${ver})
set(PROJECT_VERSION_MINOR ${CMAKE_MATCH_1})

string(REGEX MATCH "VERSION_PATCH = ([0-9]*)" _ ${ver})
set(PROJECT_VERSION_PATCH ${CMAKE_MATCH_1})

set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

message(STATUS "OpenAMP version: ${PROJECT_VERSION}")

sdk_add_include_directories(open-amp/lib/include)
sdk_add_include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/generated/openamp)

target_link_libraries(libopen-amp PRIVATE liblibmetal)

if (CONFIG_OPENAMP_VQ_RX_EMPTY_NOTIFY)
    sdk_add_compile_definitions(-DVQ_RX_EMPTY_NOTIFY=1)
else()
    sdk_add_compile_definitions(-DVQ_RX_EMPTY_NOTIFY=0)
endif()

if (CONFIG_OPENAMP_VIRTIO_DRIVER_SUPPORT)
    sdk_add_compile_definitions(-DVIRTIO_DRIVER_SUPPORT=1)
else()
    sdk_add_compile_definitions(-DVIRTIO_DRIVER_SUPPORT=0)
endif()

if (CONFIG_OPENAMP_VIRTIO_DEVICE_SUPPORT)
    sdk_add_compile_definitions(-DVIRTIO_DEVICE_SUPPORT=1)
else()
    sdk_add_compile_definitions(-DVIRTIO_DEVICE_SUPPORT=0)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/open-amp/lib/version.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/generated/openamp/version_def.h @ONLY)
sdk_add_include_directories(${CMAKE_CURRENT_BINARY_DIR}/include/generated/openamp)
