if(MINGW OR CYGWIN OR WIN32)
    set(WHERE_CMD where)
    set(TOOLCHAIN_SUFFIX ".exe")
elseif(UNIX OR APPLE)
    set(WHERE_CMD which)
    set(TOOLCHAIN_SUFFIX "")
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  message(STATUS "Enable ccache")
endif()

set(CMAKE_SYSTEM_NAME Generic)

# specify cross compilers and tools
SET(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
SET(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_LINKER ${CROSS_COMPILE}ld${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_AR ${CROSS_COMPILE}ar${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_ASM_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <OBJECTS>")
set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <OBJECTS>")
set(CMAKE_CXX_CREATE_STATIC_LIBRARY "<CMAKE_AR> crD <TARGET> <OBJECTS>")
set(CMAKE_RANLIB ${CROSS_COMPILE}ranlib${TOOLCHAIN_SUFFIX} CACHE INTERNAL "")
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy CACHE INTERNAL "")
set(CMAKE_OBJDUMP ${CROSS_COMPILE}objdump CACHE INTERNAL "")
set(CMAKE_SIZE ${CROSS_COMPILE}size CACHE INTERNAL "")

# Picolibc is mandatory and is the default C library of the supported
# Bouffalo riscv64-unknown-elf toolchain. Do not probe or fall back to Newlib.
set(BFLB_USE_PICOLIBC ON)
add_compile_definitions(BFLB_USE_PICOLIBC=1
                        __PICOLIBC_ERRNO_FUNCTION=__errno)
message(STATUS "Use Picolibc TLS errno")

# The SDK is built with a fixed GCC 14.3 toolchain. Keep the legacy-code
# diagnostics visible without letting CONFIG_STRICT turn them into errors.
foreach(language C CXX)
    string(APPEND CMAKE_${language}_FLAGS_INIT
           " -Wno-error=implicit-function-declaration"
           " -Wno-error=int-conversion"
           " -Wno-error=incompatible-pointer-types"
           " -Wno-error=return-mismatch"
           " -Wno-error=declaration-missing-parameter-type"
           " -Wno-error=array-bounds"
           " -Wno-error=stringop-overflow"
           " -Wno-error=stringop-overread"
           " -Wno-error=stringop-truncation"
           " -Wno-error=format-overflow"
           " -Wno-error=format-truncation")
endforeach()

# Fixed ISA/ABI pairs for the Bouffalo toolchain. No multilib probing: the
# toolchain is built specifically for this SDK.
if("${CHIP}" STREQUAL "bl618dg")
    if("${CPU_ID}" STREQUAL "lp")
        set(BFLB_RISCV_ARCH_FLAGS -march=rv32emc_zicsr_zifencei -mabi=ilp32e)
        set(BFLB_RISCV_ARCH_FLAGS_NOFPU ${BFLB_RISCV_ARCH_FLAGS})
    elseif("${CPU_MODEL}" STREQUAL "a0")
        set(BFLB_RISCV_ARCH_FLAGS -march=rv32imafc_zicsr_zifencei -mabi=ilp32f)
        set(BFLB_RISCV_ARCH_FLAGS_NOFPU -march=rv32imac_zicsr_zifencei -mabi=ilp32)
    else()
        set(BFLB_RISCV_ARCH_FLAGS -march=rv32imafdc_zicsr_zifencei -mabi=ilp32d)
        set(BFLB_RISCV_ARCH_FLAGS_NOFPU -march=rv32imac_zicsr_zifencei -mabi=ilp32)
    endif()
else()
    set(BFLB_RISCV_ARCH_FLAGS -march=rv32imafc_zicsr_zifencei -mabi=ilp32f)
    set(BFLB_RISCV_ARCH_FLAGS_NOFPU -march=rv32imac_zicsr_zifencei -mabi=ilp32)
endif()

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_FIND_ROOT_PATH ${CROSS_COMPILE}gcc)
# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
