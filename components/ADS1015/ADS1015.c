#include "ADS1015.h"
#include "driver/i2c_master.h"
#include <stdint.h>

void I2C_install(){}

int16_t read_ADS1015(){
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



