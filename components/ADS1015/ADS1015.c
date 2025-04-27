#include "ADS1015.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "hal/i2c_types.h"
#include <stdint.h>
#include "driver/i2c_master.h"
#include "esp_log.h"

#define I2C_MASTER_PORT       I2C_NUM_0
#define I2C_MASTER_SCL_IO     41
#define I2C_MASTER_SDA_IO     40
#define I2C_MASTER_FREQ_HZ    100000
#define ADS1015_ADDR          0x48  // 7-bit address
i2c_master_dev_handle_t ADS1015_I2C_install() {
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_PORT,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    esp_err_t ret = i2c_new_master_bus(&i2c_mst_config, &bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("ADS1015", "Failed to create I2C master bus: %s", esp_err_to_name(ret));
        return NULL;
    }else{
        ESP_LOGE("ADS1015", "Installed master bus");
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = ADS1015_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ,
    };
    i2c_master_dev_handle_t adc_dev_handle;
    ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &adc_dev_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("ADS1015", "Failed to add device to I2C bus: %s", esp_err_to_name(ret));
        i2c_del_master_bus(bus_handle);
        return NULL;
    }else{
        ESP_LOGE("ADS1015", "Added device to bus");
    }

    ESP_ERROR_CHECK(i2c_master_probe(bus_handle, 0x48, -1));

    const uint8_t config_bytes[] = {0x01, 0x84, 0x83};
    ret = i2c_master_transmit(adc_dev_handle, config_bytes, sizeof(config_bytes), -1);
    if (ret != ESP_OK) {
        ESP_LOGE("ADS1015", "Failed to transmit config bytes: %s", esp_err_to_name(ret));
        i2c_master_bus_rm_device(adc_dev_handle);
        i2c_del_master_bus(bus_handle);
        return NULL;
    }

    return adc_dev_handle;
}
int16_t read_ADS1015(i2c_master_dev_handle_t handle){
    uint8_t reading[2];
    uint8_t conv_reg = 0x00;
    i2c_master_transmit(handle, &conv_reg, 1, -1);
    i2c_master_receive(handle, reading, 2, -1);
    int16_t raw_adc = ((reading[0] << 8) | reading[1]) >> 4;
    return raw_adc;
}

static void disp_buf(uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}





