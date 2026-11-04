sdk_generate_library()
sdk_library_add_sources(
    src/bflb_nand.c
    src/bflb_nand_command.c
    src/bflb_nand_dhara.c
    src/devices/bflb_nand_winbond.c
    src/devices/bflb_nand_foresee.c
    port/bflb_nand_bus.c
    port/bflb_spi_nand_lhal.c
    third_party/dhara/dhara/map.c
    third_party/dhara/dhara/journal.c
    third_party/dhara/dhara/error.c
)
sdk_add_include_directories(include)
sdk_add_private_include_directories(src port third_party/dhara)
