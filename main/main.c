#include <stdio.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "freertos/queue.h"

#define BLINK_GPIO 2
#define INTERRUPT_GPIO 13

void blinky(void *pvParameter){
    
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(200));
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void isr(void *parameter){
    gpio_set_level(BLINK_GPIO,1);
}
void app_main()
{
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(INTERRUPT_GPIO,GPIO_MODE_INPUT);
    gpio_set_pull_mode(INTERRUPT_GPIO,GPIO_PULLUP_ONLY);

    gpio_intr_enable(INTERRUPT_GPIO);
    gpio_set_intr_type(INTERRUPT_GPIO,GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INTERRUPT_GPIO,isr,NULL);


    //xTaskCreate(&blinky, "blinky", 1024,NULL,5,NULL );
}