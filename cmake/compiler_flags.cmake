sdk_add_compile_options(
-fno-jump-tables
-fno-common
-fms-extensions
-ffunction-sections
-fdata-sections
-fmessage-length=0
-Wall
-Wchar-subscripts
-Wformat
-Wundef
-Wuninitialized
-Winit-self
-Wignored-qualifiers
-fstrict-volatile-bitfields
-fshort-enums
-MMD
-Wno-error=unused-variable
-Wno-error=format=
-Wno-error=unused-function
-Wno-error=implicit-function-declaration
-Wno-error=deprecated-declarations
# -Wno-error=absolute-value
# -Wno-error=type-limits
# -Wno-error=cpp -Wextra
-Wno-format
# -Wno-unused-parameter
# -Wno-sign-compare
# $<$<COMPILE_LANGUAGE:C>:-Wno-old-style-declaration>
# $<$<COMPILE_LANGUAGE:C>:-Wno-override-init>
# $<$<COMPILE_LANGUAGE:C>:-Wno-enum-conversion>
# $<$<COMPILE_LANGUAGE:C>:-Wno-cast-function-type>
$<$<COMPILE_LANGUAGE:C>:-std=gnu99>
$<$<COMPILE_LANGUAGE:CXX>:-std=c++11>
$<$<COMPILE_LANGUAGE:CXX>:-nostdlib>
$<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
$<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
# $<$<COMPILE_LANGUAGE:C>:-Werror>
)

sdk_add_link_options(
-Wl,--cref
-Wl,--gc-sections
-nostartfiles
-fms-extensions
-ffunction-sections
-fdata-sections
--specs=nano.specs
)

sdk_add_link_libraries(c m)

sdk_add_compile_options_ifdef(CONFIG_DEBUG -g3)

if(NOT DEFINED CONFIG_GCC_OPTIMISE_LEVEL)
sdk_add_compile_options(-O2)
else()
sdk_add_compile_options(${CONFIG_GCC_OPTIMISE_LEVEL})
endif()

