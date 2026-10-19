add_custom_target(size_report
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND python ${BL_SDK_BASE}/tools/footprint/size_report -k $<TARGET_FILE:${proj_name}.elf> -z ${build_dir} -o ${build_dir} --workspace=${build_dir} -d 99 all)