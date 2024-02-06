sdk_add_compile_options(
-fno-jump-tables
-fno-common
-fms-extensions
-ffunction-sections
-fdata-sections
-fmessage-length=0
-fstrict-volatile-bitfields
-fshort-enums
-MMD
-Wall
-Wchar-subscripts
# -Wformat
-Wno-format
-Wundef
-Wuninitialized
-Winit-self
-Wignored-qualifiers
# -Wno-unused-parameter
# -Wno-sign-compare
# -Wno-error=unused-variable
# -Wno-error=unused-but-set-variable
# -Wno-error=maybe-uninitialized
# -Wno-error=format=
# -Wno-error=unused-function
# -Wno-error=implicit-function-declaration
# -Wno-error=incompatible-pointer-types
# -Wno-error=discarded-qualifiers
# -Wno-error=int-to-pointer-cast
# -Wno-error=absolute-value
# -Wno-error=type-limits
# -Wno-error=cpp
# $<$<COMPILE_LANGUAGE:C>:-Werror>
# $<$<COMPILE_LANGUAGE:C>:-Wextra>
# $<$<COMPILE_LANGUAGE:C>:-Wno-old-style-declaration>
# $<$<COMPILE_LANGUAGE:C>:-Wno-override-init>
# $<$<COMPILE_LANGUAGE:C>:-Wno-enum-conversion>
# $<$<COMPILE_LANGUAGE:C>:-Wno-cast-function-type>
$<$<COMPILE_LANGUAGE:C>:-std=gnu99>
$<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
$<$<COMPILE_LANGUAGE:CXX>:-nostdlib>
$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
-fstack-usage
-save-temps=obj
)

sdk_add_link_options(
-Wl,--cref
-Wl,--gc-sections
-nostartfiles
-fms-extensions
-ffunction-sections
-fdata-sections
--specs=nano.specs
-Wl,--print-memory-usage
)

sdk_add_link_libraries(c m)

sdk_add_compile_options_ifdef(CONFIG_DEBUG -g3)

if(NOT DEFINED CONFIG_GCC_OPTIMISE_LEVEL)
sdk_add_compile_options(-O2)
else()
sdk_add_compile_options(${CONFIG_GCC_OPTIMISE_LEVEL})
endif()

