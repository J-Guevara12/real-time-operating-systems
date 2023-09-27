#include <stdio.h>
#include "string.h"

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

static const char* TAG = "MAIN";

// Declaración de colas
QueueHandle_t brightness;
QueueHandle_t temperatureQueue; 
QueueHandle_t uartDataQueue; // cola uart
QueueHandle_t printSecondsQueue;

// Interuption
void isr(void *parameter){
    int msg = false;
    xQueueSendFromISR(printSecondsQueue,&msg,NULL);
}

void printTemperature(){
    bool firstTime = true;
    int wait = 1000;
    double temp;
    while(1){
        if(xQueueReceive(printSecondsQueue,&firstTime,(TickType_t)0)){
            wait = (wait + 1000)%5000; // Para que mantenga entre 1000 y 5000
        }
        xQueueReceive(temperatureQueue,&temp,(TickType_t)10);
        ESP_LOGI(TAG,"La temperatura es: %f",temp);
        vTaskDelay(pdMS_TO_TICKS(wait+1000));
    }
}

void app_main()
{ 
      
    // Inicialización de la cola para datos UART
    uartDataQueue = xQueueCreate(10, sizeof(char) * BUF_SIZE);
    temperatureQueue = xQueueCreate(2, sizeof(double));
    printSecondsQueue = xQueueCreate(1,sizeof(bool));

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
    //xTaskCreate(&echo_task,"UART",4096,NULL,5,NULL);
    xTaskCreate(&measureTemperature, "measure Temperature NTC", 2048, NULL, 4, NULL);
    xTaskCreate(&printTemperature,"Imprimir temperatura cada x segundos",2048,NULL,5,NULL);
    //xTaskCreate(&controlLEDs, "Control LEDs", 2048, NULL, 5, NULL);
    //xTaskCreate(&controlLEDs ,"Asses Control LEDs", 2048,NULL,5, &controlLEDsTasksHandle);
    

    ESP_LOGI(TAG,"Finished Task creation");
}
