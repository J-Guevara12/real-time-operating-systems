#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/queue.h"


#include "led.h"
#include "adc.h"
#include "uart.h"

#define BLINK_PERIOD 100
#define INTERRUPT_GPIO 13
#define R_PIN 15
#define G_PIN 2
#define B_PIN 4
#define UPDATE_PERIOD 50

static const char* TAG = "MAIN";

// Declaración de colas
QueueHandle_t currentChannel;
QueueHandle_t brightness;

Queues queuesStruct = {
    .channelQueue = &currentChannel,
    .brightnessQueue = &brightness
};

// Interuption
void isr(void *parameter){
    int msg = 10;
    xQueueSendFromISR(currentChannel,&msg,NULL);
}

void app_main()
{
    currentChannel = xQueueCreate(1,sizeof(int));
    brightness = xQueueCreate(1,sizeof(int));

    ESP_LOGI(TAG,"pointer %p",(void *)&brightness);

 // Inicialización de PWM para controlar LEDs  
    ledc_init_with_pin(R_PIN,0);
    ledc_init_with_pin(G_PIN,1);
    ledc_init_with_pin(B_PIN,2);

//Inicializacion del ADC
    adc_init();

//Configutacion de interrupciones (Pines)
    gpio_set_direction(INTERRUPT_GPIO,GPIO_MODE_INPUT);
    gpio_set_pull_mode(INTERRUPT_GPIO,GPIO_PULLUP_ONLY);


// Habilitar interrupcion  negatica / flaco de bajada
    gpio_intr_enable(INTERRUPT_GPIO);
    gpio_set_intr_type(INTERRUPT_GPIO,GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INTERRUPT_GPIO,isr,NULL);

// Tareas
    xTaskCreate(&readFromQueue, "read from queue", 2048,&queuesStruct,5,NULL );
    xTaskCreate(&write_queue,"write to queue",2048,&brightness,5,NULL);
    xTaskCreate(&echo_task,"UART",2048,NULL,5,NULL);
}