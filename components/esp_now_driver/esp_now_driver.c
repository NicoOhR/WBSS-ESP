#include "esp_now_driver.h"
#include <stdint.h>

esp_err_t esp_now_init_driver() {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_ERROR_CHECK(esp_now_init());
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddr, sizeof(broadcastAddr));
  peerInfo.channel = 0; // Use the current channel
  peerInfo.encrypt = false;

  // Add the broadcast peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    ESP_LOGE(TAG, "Failed to add broadcast peer");
    return ESP_FAIL;
  }

  ESP_LOGI(TAG, "ESP-NOW initialized successfully");
  return ESP_OK;
};

esp_err_t esp_now_send_data(sensor_dataframe_t *sensor_data) {
  esp_err_t result = esp_now_send(broadcastAddr, (uint8_t *)&sensor_data,
                                  sizeof(&sensor_data));

  return result;
}
