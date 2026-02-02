sdk_generate_library()

set(PROJECT_SOURCE_DIR ../openthread)

set(NCP_SOURCES
    changed_props_set.cpp
    multipan_platform.cpp
    ncp_base.cpp
    ncp_base_dispatcher.cpp
    ncp_base_radio.cpp
    ncp_spi.cpp
    ncp_hdlc.cpp
    platform/dnssd.cpp
    platform/infra_if.cpp
)

set(HDLC_SOURCES
    hdlc.cpp
)

set(SPINEL_SOURCES
    spinel.c
    spinel_buffer.cpp
    spinel_decoder.cpp
    spinel_encoder.cpp
    spinel_helper.cpp
    spinel_prop_codec.cpp
)

set(EXAMPLE_NCP_SOURCES
    ncp.c
)

sdk_add_private_compile_definitions(-DOPENTHREAD_PLATFORM_NEXUS=0)

if (DEFINED OT_NCP)
  sdk_add_private_compile_definitions(-DOPENTHREAD_SPINEL_CONFIG_OPENTHREAD_MESSAGE_ENABLE=${OT_NCP})

  sdk_add_private_include_directories(${PROJECT_SOURCE_DIR}/src)
  sdk_add_private_include_directories(${PROJECT_SOURCE_DIR}/src/include)
  foreach(source_file ${SPINEL_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/lib/spinel/${source_file})
  endforeach()

  sdk_add_private_include_directories(${PROJECT_SOURCE_DIR}/src/ncp)
  foreach(source_file ${NCP_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/ncp/${source_file})
  endforeach()

  if (OT_NCP)
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/ncp/ncp_base_ftd.cpp)
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/ncp/ncp_base_mtd.cpp)
  endif()
endif()

sdk_add_private_include_directories(${PROJECT_SOURCE_DIR}/src/lib/hdlc)
foreach(source_file ${HDLC_SOURCES})
  sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/lib/hdlc/${source_file})
endforeach()

if (DEFINED OT_NCP)
  sdk_add_include_directories(${PROJECT_SOURCE_DIR}/examples/platforms)
  foreach(source_file ${EXAMPLE_NCP_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/examples/apps/ncp/${source_file})
  endforeach()
endif()
