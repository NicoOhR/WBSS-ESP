idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES spi_flash ADS8320 driver CAN_driver esp_now_driver ADC_driver
                    INCLUDE_DIRS "")

set(EXTRA_COMPONENT_DIRS components)
