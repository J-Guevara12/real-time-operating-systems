#include "led.h"
#define UPDATE_PERIOD 50

static const char *TAG = "LED";

extern QueueHandle_t currentChannel;
extern QueueHandle_t brightness;

ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_MODE,
    .timer_num        = LEDC_TIMER,
    .duty_resolution  = LEDC_DUTY_RES,
    .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
    .clk_cfg          = LEDC_AUTO_CLK
};

void ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void ledc_init_with_pin(int PIN,int channel)
{
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = channel,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PIN,
        .duty           = 8192, // Set duty to 100%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(TAG,"LED initializated");


}

void fade(void *pvParameter){
    int brightness = 0;
    int delta = 256;
    while(true){
        ledc_set_duty(LEDC_MODE,LEDC_CHANNEL,brightness);
        ledc_update_duty(LEDC_MODE,LEDC_CHANNEL);
        brightness += delta;
        if(brightness >= 8192 || brightness <= 0)
            delta *= -1;

        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD));
    }
}

void readFromQueue(void* px_queues){
    int brightness_val = 0;
    int channel = 0;
    int rcv;
    while(true){
        xQueueReceive(brightness,&brightness_val,(TickType_t)10);
        if(xQueueReceive(currentChannel,&rcv,(TickType_t)10)){
            channel = (channel + 1)%3;
        }

        ledc_set_duty(LEDC_MODE,channel,2*brightness_val);
        ledc_update_duty(LEDC_MODE,channel);

        vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD));
    }
}