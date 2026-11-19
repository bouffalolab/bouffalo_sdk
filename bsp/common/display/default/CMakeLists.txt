# The default display (default/Kconfig): one screen defined from Kconfig
if(CONFIG_DISPLAY_DEFAULT)
    if(CONFIG_DISPLAY_DEFAULT_TYPE_VIDEO)
        target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/display_default_video.c)
    elseif(CONFIG_DISPLAY_DEFAULT_TYPE_GRAM)
        target_sources(app PRIVATE ${CMAKE_CURRENT_LIST_DIR}/display_default_gram.c)
    endif()
endif()
