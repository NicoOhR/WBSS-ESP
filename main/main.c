/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "ADS8320.h"
#include "CAN_driver.h"
#include "driver/gpio.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_now.h"
#include "esp_now_driver.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"
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
void on_recv(const esp_now_recv_info_t *esp_now_info, const uint8_t *data,
             int data_len) {
  // Print the source address
  printf("Received data from: %02X:%02X:%02X:%02X:%02X:%02X\n",
         esp_now_info->src_addr[0], esp_now_info->src_addr[1],
         esp_now_info->src_addr[2], esp_now_info->src_addr[3],
         esp_now_info->src_addr[4], esp_now_info->src_addr[5]);

  // Print the received data
  printf("Data received (%d bytes): ", data_len);
  for (int i = 0; i < data_len; i++) {
    printf("%02X ", data[i]);
  }
  printf("\n");
}

void app_main(void) {
  init_ext_adc();
  long data = 0;
  // pin testing + CAN
  /*init_can();*/
  /*uint8_t test[4] = {0x11, 0x22, 0x33, 0x44};*/
  /*data = convert_to_volts(read_ext_adc());*/
  /*data = read_ext_adc();*/
  /*gpio_reset_pin(41);*/
  /*gpio_set_direction(41, GPIO_MODE_OUTPUT);*/
  /*gpio_set_level(41, 1);*/
  /*chip_info();*/
  if (esp_now_init_driver() != ESP_OK) {
    ESP_LOGE("MAIN", "Failed to initialize ESP-NOW");
    return;
  }

  sensor_dataframe_t test = {
      .id = 1,
      .linpot = 22,
  };

  esp_now_register_recv_cb(on_recv);
  while (true) {
    /*gpio_set_level(41, 0);*/
    /*vTaskDelay(10000 / portTICK_PERIOD_MS); // Delay for 1 second*/
    /*gpio_set_level(41, 1);*/
    /*send_message(0x100, test, sizeof(test));*/

    // relatively sure this does not work
    /*data = read_ext_adc();*/
    /*printf("External ADC Value: %ld\n", data);*/

    // testing ESP_NOW
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 second
    esp_now_send_data(&test);
  }
}
