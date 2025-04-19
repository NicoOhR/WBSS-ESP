#ifndef INTERNALADC
#define INTERNALADC

#define ADC_PIN 32 //need to check on that

#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"


void init_adc(void);

void read_adc(int *raw, int *voltage);

#endif // !INTERNALADC
