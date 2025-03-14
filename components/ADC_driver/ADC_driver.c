#include "ADC_driver.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/adc_types.h"

const static char *TAG = "ADC";
static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t adc_cali_handle;
static bool do_calibration = false;

static bool _adc_calibration_init(adc_unit_t unit, adc_channel_t channel,
                                  adc_atten_t atten,
                                  adc_cali_handle_t *out_handle) {
  adc_cali_handle_t handle = NULL;
  esp_err_t ret = ESP_FAIL;
  bool calibrated = false;
  if (!calibrated) {
    ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit,
        .chan = channel,
        .atten = atten,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
    if (ret == ESP_OK) {
      calibrated = true;
    }
  }

  *out_handle = handle;
  if (ret == ESP_OK) {
    ESP_LOGI(TAG, "Calibration Success");
  } else if (ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
    ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
  } else {
    ESP_LOGE(TAG, "Invalid arg or no memory");
  }

  return calibrated;
}
void init_adc() {
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = ADC_UNIT_1,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc_handle));

  //-------------ADC1 Config---------------//
  adc_oneshot_chan_cfg_t config = {
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(
      adc_oneshot_config_channel(adc_handle, ADC_CHANNEL_4, &config));

  //-------------ADC1 Calibration Init---------------//
  adc_cali_handle_t adc1_cali_chan0_handle = NULL;
  do_calibration = _adc_calibration_init(ADC_UNIT_1, ADC_CHANNEL_4,
                                         ADC_ATTEN_DB_12, &adc_cali_handle);
}

void read_adc(int *raw, int *voltage) {
  ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL_4, raw));
  ESP_LOGI(TAG, "ADC Raw Data: %d", *raw);

  if (do_calibration) {
    ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, *raw, voltage));
    ESP_LOGI(TAG, "Calibrated Voltage: %d mV", *voltage);
  }
}
