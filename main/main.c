#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/queue.h"

#include "led.h"
#include "adc.h"

#define BLINK_GPIO 2
#define BLINK_PERIOD 100
#define INTERRUPT_GPIO 13

QueueHandle_t brightness;

void isr(void *parameter){
    // Inverts the current state held in the buffer
    int var = 4096;
    xQueueSendFromISR(brightness,&var,NULL);
}

void app_main()
{
    brightness = xQueueCreate(1,sizeof(int));

    example_ledc_init();
    adc_init();

    gpio_set_direction(INTERRUPT_GPIO,GPIO_MODE_INPUT);
    gpio_set_pull_mode(INTERRUPT_GPIO,GPIO_PULLUP_ONLY);

    gpio_intr_enable(INTERRUPT_GPIO);
    gpio_set_intr_type(INTERRUPT_GPIO,GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INTERRUPT_GPIO,isr,NULL);

    xTaskCreate(&readFromQueue, "read from queue", 1024,&brightness,5,NULL );
    xTaskCreate(&write_queue,"write to queue",1024,&brightness,5,NULL);
}