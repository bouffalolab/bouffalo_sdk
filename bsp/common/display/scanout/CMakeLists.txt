# Scanout core and the scanout backends that are switched on in scanout/Kconfig
target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/display_scanout.c)

if(CONFIG_DISPLAY_SCANOUT_BL618DG_DPU)
    if(NOT CHIP STREQUAL "bl618dg")
        message(FATAL_ERROR "The DPU scanout requires BL618DG")
    endif()
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/bl618dg_dpu/display_bl618dg_dpu_scanout.c
        ${CMAKE_CURRENT_LIST_DIR}/bl618dg_dpu/display_bl618dg_dpu_hw.c
        ${CMAKE_CURRENT_LIST_DIR}/bl618dg_dpu/display_bl618dg_dpu_clock.c)
endif()
if(CONFIG_DISPLAY_SCANOUT_SIM_V1)
    if(NOT CHIP STREQUAL "bl616")
        message(FATAL_ERROR "The SIM v1 scanout requires BL616")
    endif()
    target_sources(app PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/sim_v1/display_sim_v1_scanout.c
        ${CMAKE_CURRENT_LIST_DIR}/sim_v1/display_sim_v1_hw.c)
endif()
