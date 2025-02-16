#include <stdint.h>
#include "driver/twai.h"
#include "esp_err.h"
#include "esp_log.h"
#include <string.h>

void init_can();

void send_message(uint32_t id, uint8_t *data, uint8_t data_len);
