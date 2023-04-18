sdk_generate_library()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/c sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/core/api sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/kernels sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/kernels/internal sources)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/schema sources)
sdk_library_add_sources(${sources})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro/*.cc")
list(APPEND SRCS ${sources})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro/benchmarks/*.cc")
list(REMOVE_ITEM SRCS ${sources})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro/examples/*.cc")
list(REMOVE_ITEM SRCS ${sources})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro/models/*.cc")
list(REMOVE_ITEM SRCS ${sources})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro/testing/*.cc")
list(REMOVE_ITEM SRCS ${sources})

file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/tensorflow/lite/micro/*test.cc")
list(REMOVE_ITEM SRCS ${sources})

sdk_library_add_sources(${SRCS})
sdk_add_include_directories(third_party/flatbuffers/include)
sdk_add_include_directories(third_party/gemmlowp)
sdk_add_include_directories(third_party/ruy)
sdk_add_include_directories(.)

sdk_add_compile_definitions(
-DTF_LITE_USE_GLOBAL_CMATH_FUNCTIONS
-DTF_LITE_USE_GLOBAL_MIN
-DTF_LITE_USE_GLOBAL_MAX
-DTF_LITE_STATIC_MEMORY
)

sdk_add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-threadsafe-statics>)
