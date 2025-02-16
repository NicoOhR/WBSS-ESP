/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "ADS8320.h"
#include "CAN_driver.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <inttypes.h>
#include <stdio.h>

void chip_info(void) {
  /* Print chip information */
  esp_chip_info_t chip_info;
  uint32_t flash_size;
  esp_chip_info(&chip_info);
  printf("This is %s chip with %d CPU core(s), %s%s%s%s, ", CONFIG_IDF_TARGET,
         chip_info.cores,
         (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
         (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
         (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
         (chip_info.features & CHIP_FEATURE_IEEE802154)
             ? ", 802.15.4 (Zigbee/Thread)"
             : "");

  unsigned major_rev = chip_info.revision / 100;
  unsigned minor_rev = chip_info.revision % 100;
  printf("silicon revision v%d.%d, ", major_rev, minor_rev);
  if (esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
    printf("Get flash size failed");
    return;
  }

  printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded"
                                                       : "external");

  printf("Minimum free heap size: %" PRIu32 " bytes\n",
         esp_get_minimum_free_heap_size());
}

void app_main(void) {
  init_ext_adc();
  init_can();
  long data = 0;
  uint8_t test[4] = {0x11, 0x22, 0x33, 0x44};
  chip_info();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  // data = convert_to_volts(read_ext_adc());
  data = read_ext_adc();
  while (true) {
    printf("External ADC Value: %ld\n", data);
    send_message(0x100, test, sizeof(test));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
