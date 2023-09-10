#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "drever/adc.h"

#define BLINK_GPIO 2
#define INTERRUPT_GPIO 13
#define LED1_GPIO 2
#define LED2_GPIO 4
#define LED3_GPIO 16
#define POTENCIOMETER_ADC_CHANNEL ADC1_CHANNEL_0
#define RESOLUTION 1023

QueueHandle_t ledControl;

void ledControlAdcTask(void *pvParameter){
    
    uint32_t potentiometerValue = 0;
    
    while(1) {
        adc_read(&potentiometerValue);

        uint32_t scaledValue = (potentiometerValue * 3) / ADC_RESOLUTION;

        gpio_set_level(LED1_GPIO,(scaledValue >=1)? 1 : 0);
        gpio_set_level(LED2_GPIO,(scaledValue >=2)? 1 : 0);
        gpio_set_level(LED3_GPIO,(scaledValue >=1)? 1 : 0);

        vTaskDelay(pdMS_TO_TICKS(100));
    
    }
}


void app_main()
{
    ledControl =xTaskCreate(1,sizeof(uint32_t));

    gpio_set_direction(LED1_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED2_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED3_GPIO, GPIO_MODE_OUTPUT);

    adc1_config_width(ADC_WIDTH_BIT_10);
    adc1_config_channel_atten(POTENCIOMETER_ADC_CHANNEL, ADC_ATTEN_DB_0);

    

    xTaskCreate(&ledControlAdcTask, "led_control_task", 2048,NULL,5,NULL );
}