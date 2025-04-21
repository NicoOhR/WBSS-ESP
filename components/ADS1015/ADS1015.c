#include "ADS1015.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "hal/i2c_types.h"
#include <stdint.h>

i2c_master_dev_handle_t I2C_install(){
    //realistically, this function should accept thte master config below and only
    //create the add dev handle, since this is the only use of the i2c in the project
    //I'll leave as is for at least a little longer.
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = 0,
        .scl_io_num = 41,
        .sda_io_num = 40,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle;
    i2c_new_master_bus(&i2c_mst_config, &bus_handle);

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0b1001000, 
        .scl_speed_hz = 100000, //note to next dev, might be worth looking into the highspeed mode on this device
    };
    i2c_master_dev_handle_t adc_dev_handle;
    i2c_master_bus_add_device(bus_handle, &dev_cfg, &adc_dev_handle);

    return adc_dev_handle;
}

int16_t read_ADS1015(i2c_master_dev_handle_t handle){
    int16_t reading = 0;
    return reading;
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





