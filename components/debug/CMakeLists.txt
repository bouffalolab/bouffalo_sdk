# backtrace
if(CONFIG_BACKTRACE)
    sdk_generate_library()
    sdk_library_add_sources(src/riscv/rv32f_32gpr/backtrace.c)
    sdk_add_include_directories(include/riscv/rv32f_32gpr)
endif()
