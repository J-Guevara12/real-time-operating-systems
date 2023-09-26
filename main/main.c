#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/queue.h"


#include "led.h"
#include "adc.h"
#include "uart.h"
#include "thermistor.h"


#define INTERRUPT_GPIO 13
#define R_PIN 15
#define G_PIN 2
#define B_PIN 4


int rang_max_R = 60;
int rang_min_R = 45;
int rang_max_B = 55;
int rang_min_B = 40;
int rang_max_G = 45;
int rang_min_G = 30;
int threshold_temp = 20;
int threshold_freq = 1;

static const char* TAG = "MAIN";

// Declaración de colas
QueueHandle_t currentChannel;
QueueHandle_t brightness;
QueueHandle_t temperatureQueue; 

// Interuption
void isr(void *parameter){
    int msg = 10;
    xQueueSendFromISR(currentChannel,&msg,NULL);
}

// Medir temperatura y enviar a la cola
void measureTemperature(void *parameter){
    while(1){
        double resistance = adc_init()*10000.0/ 4096.0;
        double temperature =  calculateTemperature(resistance);


        xQueueSend(temperatureQueue, &temperature, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(5000)); //Esperar 5 segundos

    }
}

void app_main()
{
    currentChannel = xQueueCreate(1,sizeof(int));
    brightness = xQueueCreate(1,sizeof(int));

 // Inicialización de PWM para controlar LEDs  
    ledc_init_with_pin(R_PIN,0);
    ledc_init_with_pin(G_PIN,1);
    ledc_init_with_pin(B_PIN,2);

//Inicializacion del ADC
    adc_init();

//Inicialización UART
    uart_init();

//Configutacion de interrupciones (Pines)
    gpio_set_direction(INTERRUPT_GPIO,GPIO_MODE_INPUT);
    gpio_set_pull_mode(INTERRUPT_GPIO,GPIO_PULLUP_ONLY);


// Habilitar interrupcion  negatica / flaco de bajada
    gpio_intr_enable(INTERRUPT_GPIO);
    gpio_set_intr_type(INTERRUPT_GPIO,GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INTERRUPT_GPIO,isr,NULL);

// Tareas
    xTaskCreate(&readFromQueue, "read from queue", 2048,NULL,5,NULL );
    xTaskCreate(&write_queue,"write to queue",2048,NULL,5,NULL);
    xTaskCreate(&echo_task,"UART",4096,NULL,5,NULL);
    xTaskCreate(&measureTemperature, "measure Temperature NTC", 2048, NULL, 5, NULL);

    ESP_LOGI(TAG,"Finished Task creation");
}
