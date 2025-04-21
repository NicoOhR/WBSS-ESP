#include "CAN_driver.h"

#define TX_GPIO 2
#define RX_GPIO 42

void init_can(void) {
  // General configuration
  twai_general_config_t g_config =
      TWAI_GENERAL_CONFIG_DEFAULT(TX_GPIO, RX_GPIO, TWAI_MODE_NORMAL);

  // Timing configuration for 500 kbps
  twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();

  // Filter configuration to accept all messages
  twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

  // Install TWAI driver
  esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
  if (err != ESP_OK) {
    ESP_LOGE("TWAI", "Failed to install driver: %s", esp_err_to_name(err));
    return;
  }

  // Start TWAI driver
  err = twai_start();
  if (err != ESP_OK) {
    ESP_LOGE("TWAI", "Failed to start driver: %s", esp_err_to_name(err));
    // Uninstall driver if start failed
    twai_driver_uninstall();
    return;
  }

  ESP_LOGI("TWAI", "TWAI driver installed and started successfully");
}

void send_message(uint32_t id, uint8_t *data, uint8_t data_len) {
  if (data_len > 8) {
    ESP_LOGE("TWAI", "Data length must be 8 bytes or less");
    return;
  }

  // Configure message to transmit
  twai_message_t message = {
      .identifier = id,
      .data_length_code = data_len,
      .extd = 0, // Standard frame
      .rtr = 0   // Data frame
  };

  memcpy(message.data, data, data_len);

  // Queue message for transmission
  esp_err_t err = twai_transmit(&message, pdMS_TO_TICKS(1000));
  if (err == ESP_OK) {
    ESP_LOGI("TWAI", "Message queued for transmission");
  } else {
    ESP_LOGE("TWAI", "Failed to queue message for transmission: %s",
             esp_err_to_name(err));
  }
}
