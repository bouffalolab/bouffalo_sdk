cmake_minimum_required(VERSION 3.15)

add_subdirectory(bsp)
add_subdirectory(components)
add_subdirectory(drivers/lhal)
add_subdirectory(drivers/soc/${CHIP}/std)
sdk_add_subdirectory_ifdef(CONFIG_RF drivers/soc/${CHIP}/rf)
sdk_add_subdirectory_ifdef(CONFIG_RF drivers/rfparam)
