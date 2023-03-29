sdk_add_include_directories(inc)

sdk_add_static_library(lib/libwifi6.a)

add_subdirectory(bl6_os_adapter)

sdk_add_compile_definitions(-DPLATFORM_PRIVDE_ERRNO)

# Enable common section for wifi6
sdk_add_compile_options(-fcommon)

# disable undef warning
sdk_add_compile_options(-Wno-undef)

