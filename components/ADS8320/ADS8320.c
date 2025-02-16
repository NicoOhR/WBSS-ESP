#include "ADS8320.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#ifndef ADS8320_CS

#define ADS8320_CLK 10
#define ADS8320_DOUT 11
#define ADS8320_CS 12

#endif

void init_ext_adc() {
  gpio_set_level(ADS8320_CS, 1);
  gpio_set_level(ADS8320_CLK, 1);
}

long read_ext_adc() {
  int i, dout;
  long data;

  data = 0;
  // output_low(ADS8320_CS);
  gpio_set_level(ADS8320_CS, 0);
  for (i = 0; i <= 5; i++) // take sample and send start bit
  {
    gpio_set_level(ADS8320_CLK, 0);
    esp_rom_delay_us(1);
    gpio_set_level(ADS8320_CLK, 1);
    esp_rom_delay_us(1);
  }
  for (i = 0; i < 16; ++i) { // send sample over spi
    gpio_set_level(ADS8320_CLK, 0);
    esp_rom_delay_us(1);
    dout = gpio_get_level(ADS8320_DOUT);
    data = data << 1 | dout;
    gpio_set_level(ADS8320_CLK, 1);
    esp_rom_delay_us(1);
  }

  gpio_set_level(ADS8320_CS, 1);
  return (data);
}

float convert_to_volts(long data) { return ((float)data * 5.0 / 0xffff); }
