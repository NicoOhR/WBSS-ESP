#ifndef ADS8320
#define ADS8320
void init_ext_adc();

long read_ext_adc();

float convert_to_volts();
#endif // !ADS8320
