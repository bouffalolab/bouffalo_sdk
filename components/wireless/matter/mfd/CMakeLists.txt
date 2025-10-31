sdk_generate_library()

sdk_add_include_directories(./include)
sdk_add_private_include_directories(./)

if (NOT "${CHIP}" STREQUAL "bl616")
  message(FATAL_ERROR "Matter MFD only supports for BL616")
endif()

sdk_add_private_compile_definitions(-DBL_MFD_PLAT_H=\"bl_mfd_bl616.h\")
sdk_library_add_sources(bl_mfd.c bl_mfd_decrypt_bl616.c)
