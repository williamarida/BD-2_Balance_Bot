idf_component_register(SRCS "main.cpp" "controller.c"
  INCLUDE_DIRS "."
  REQUIRES bluepad32 btstack esp_wifi esp_netif esp_event nvs_flash json esp_timer lwip)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/wifi_credentials.h")
  target_compile_definitions(${COMPONENT_LIB} PRIVATE BD2_WIFI_CONFIGURED=1)
endif()
