sdk_generate_library()

set(PROJECT_SOURCE_DIR ../openthread)

set(NCP_SOURCES
    changed_props_set.cpp
    ncp_base.cpp
    ncp_base_dispatcher.cpp
    ncp_base_radio.cpp
    ncp_spi.cpp
    ncp_hdlc.cpp
    ncp_base_ftd.cpp
    ncp_base_mtd.cpp
)

set(HDLC_SOURCES
    hdlc.cpp
)

set(SPINEL_SOURCES
    spinel.c
    spinel_buffer.cpp
    spinel_decoder.cpp
    spinel_encoder.cpp
)

set(EXAMPLE_NCP_SOURCES
    ncp.c
)

if (DEFINED OT_NCP)
  sdk_add_include_directories(${PROJECT_SOURCE_DIR}/src/lib/spinel)
  foreach(source_file ${SPINEL_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/lib/spinel/${source_file})
  endforeach()

  sdk_add_include_directories(${PROJECT_SOURCE_DIR}/src/ncp)
  foreach(source_file ${NCP_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/ncp/${source_file})
  endforeach()
endif()

sdk_add_include_directories(${PROJECT_SOURCE_DIR}/src/lib/hdlc)
foreach(source_file ${HDLC_SOURCES})
  sdk_library_add_sources(${PROJECT_SOURCE_DIR}/src/lib/hdlc/${source_file})
endforeach()

message(INFO "OT_NCP=${OT_NCP}")

if (DEFINED OT_NCP)
  sdk_add_include_directories(${PROJECT_SOURCE_DIR}/examples/platforms)
  foreach(source_file ${EXAMPLE_NCP_SOURCES})
    sdk_library_add_sources(${PROJECT_SOURCE_DIR}/examples/apps/ncp/${source_file})
  endforeach()
endif()
