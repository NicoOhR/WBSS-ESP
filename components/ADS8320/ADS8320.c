#include "ADS8320.h"
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "hal/gpio_types.h"
#ifndef ADS8320_CS

#define ADS8320_CLK 2
#define ADS8320_DOUT 42
#define ADS8320_CS 41

#endif

void init_ext_adc() {
  gpio_reset_pin(41);
  gpio_reset_pin(2);
  gpio_reset_pin(42);

  gpio_set_direction(ADS8320_CLK, GPIO_MODE_OUTPUT);
  gpio_set_direction(ADS8320_CS, GPIO_MODE_OUTPUT);
  gpio_set_direction(ADS8320_DOUT, GPIO_MODE_INPUT);
  gpio_set_pull_mode(42, GPIO_PULLDOWN_ENABLE);
  gpio_set_pull_mode(41, GPIO_PULLDOWN_ENABLE);
  gpio_set_pull_mode(ADS8320_CLK, GPIO_PULLDOWN_ENABLE);
  gpio_set_level(ADS8320_CS, 1);
  gpio_set_level(ADS8320_CLK, 1);
}

long read_ext_adc() {
  int i, dout;
  long data;

  data = 0;
  gpio_set_level(ADS8320_CS, 0);
  esp_rom_delay_us(1000);
  for (i = 0; i <= 5; i++) // take sample and send start bit
  {
    gpio_set_level(ADS8320_CLK, 0);
    esp_rom_delay_us(1000);
    gpio_set_level(ADS8320_CLK, 1);
    esp_rom_delay_us(1000);
  }
  for (i = 0; i < 16; ++i) { // send sample over spi
    gpio_set_level(ADS8320_CLK, 0);
    esp_rom_delay_us(1000);
    dout = gpio_get_level(ADS8320_DOUT);
    /*printf("%d", dout);*/
    data <<= 1;
    data |= dout;
    gpio_set_level(ADS8320_CLK, 1);
    esp_rom_delay_us(1000);
  }

  gpio_set_level(ADS8320_CS, 1);
  return (data);
}

float convert_to_volts(long data) { return ((float)data * 5.0 / 0xffff); }
