sdk_generate_library()
sdk_library_add_sources(src/event.c src/event_svr.c src/rpc.c src/uservice.c src/utask.c)

sdk_add_private_compile_definitions(CONFIG_USERVICE_EVENT_TASK_STACK_SIZE=2048)
sdk_add_include_directories(include)
sdk_add_private_include_directories(src)

