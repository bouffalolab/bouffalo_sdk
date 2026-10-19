sdk_generate_library()

sdk_add_include_directories(include)

sdk_library_add_sources(
  src/librws.c
  src/rws_common.c
  src/rws_error.c
  src/rws_frame.c
  src/rws_list.c
  src/rws_memory.c
  src/rws_network.c
  src/rws_socketpriv.c
  src/rws_socketpub.c
  src/rws_string.c
  src/port/rws_thread.c
)

