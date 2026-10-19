sdk_generate_library()

sdk_add_include_directories(inc)

sdk_library_add_sources(
  MQTT-C/src/mqtt.c
  MQTT-C/src/mqtt_pal.c
)


# sdk_library_add_sources(src/tcp_publisher.c)
# sdk_add_link_options(-ucmd_mqtt_publish)

