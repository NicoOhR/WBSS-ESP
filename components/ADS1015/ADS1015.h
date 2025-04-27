#ifndef ADS1015
#define ADS1015

#include "driver/i2c_types.h"

#define I2C_SCL 41
#define I2C_SDA 40

i2c_master_dev_handle_t ADS1015_I2C_install();
int16_t read_ADS1015(i2c_master_dev_handle_t);

#endif


