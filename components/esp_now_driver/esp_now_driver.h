#ifndef ESP_NOW_DRIVER_H
#define ESP_NOW_DRIVER_H

#include "esp_now.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "string.h"

static const char *TAG = "ESP-NOW";
static const uint8_t broadcastAddr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

typedef struct __attribute__((packed)){
  int id;
  uint16_t linpot;
  int64_t  timestamp;
  //eventually: 
  //wheelspeed
  //DLHR
} suspension_dataframe_t;


esp_err_t esp_now_init_driver(void);
esp_err_t esp_now_send_data(suspension_dataframe_t* sensor_data);

#endif
