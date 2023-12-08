sdk_generate_library()

sdk_add_include_directories(include)

sdk_library_add_sources(src/core.c
                        src/mixer.c
                        src/pcm.c
                        src/snd.c
)

sdk_add_compile_definitions(
  -DCONFIG_USE_MINIALSA=1
)
