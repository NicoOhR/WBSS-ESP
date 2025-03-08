idf_component_register(SRCS "esp_now_driver.c"
                       REQUIRES esp_driver_gpio esp_wifi nvs_flash
                       INCLUDE_DIRS ".")

