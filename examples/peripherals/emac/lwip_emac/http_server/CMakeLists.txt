sdk_add_include_directories(.)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../README_zh.md")
	sdk_add_compile_definitions(-DHTTP_README_CHINESE_ENABLED)
endif()

target_sources(app PRIVATE
	http_server.c
	readme_assets.c
)
