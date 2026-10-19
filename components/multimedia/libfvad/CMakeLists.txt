sdk_generate_library()

sdk_add_include_directories(
    include
    src
    src/vad
    src/signal_processing
)

sdk_add_private_compile_definitions(
    -Dstatic_assert=_Static_assert
)

sdk_library_add_sources(
    src/fvad.c
    src/vad/vad_core.c
    src/vad/vad_filterbank.c
    src/vad/vad_gmm.c
    src/vad/vad_sp.c
    src/signal_processing/division_operations.c
    src/signal_processing/energy.c
    src/signal_processing/get_scaling_square.c
    src/signal_processing/resample_48khz.c
    src/signal_processing/resample_by_2_internal.c
    src/signal_processing/resample_fractional.c
    src/signal_processing/spl_inl.c
)
