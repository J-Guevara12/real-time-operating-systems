#include <adc.h>
#include "esp_log.h"
#include "thermistor.h"

#define UPDATE_PERIOD 20

static const char *TAG = "ADC";

extern QueueHandle_t brightness;
extern QueueHandle_t temperatureQueue;

adc_oneshot_unit_handle_t adc_handler;

void adc_init(void){
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT,
        .ulp_mode = ADC_ULP,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc_handler));

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handler, ADC_CHAN, &config));

    ESP_LOGI(TAG,"ADC intialized");
}

void write_queue(){
    int val;
    while (true){
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handler, ADC_CHAN, &val));
        double temperature = calculateTemperature((double)val);
        xQueueSend(brightness,&val,(TickType_t)10);
        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD));
    }
}

void measureTemperature(void *parameter){
    while(1){
        int adc_value;
        double temperature;

        ESP_ERROR_CHECK(adc_oneshot_read(adc_handler, ADC_CHAN, &adc_value));

        double V = ((double)adc_value * 3.9) / 4096.0;
        temperature =  calculateTemperature(V);

        xQueueSend(temperatureQueue, &temperature, (TickType_t)10);
        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD)); 
    }
}