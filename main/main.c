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


#define INTERRUPT_GPIO 13 //  Numero del Pin de interrupcion 

static const char* TAG = "MAIN";  //Etiqueta de depuracion 

// Declaración de colas para comunicacion entre tareas
QueueHandle_t brightness;
QueueHandle_t temperatureQueue; 
QueueHandle_t uartDataQueue; // cola uart
QueueHandle_t printSecondsQueue;


// Tarea: interrupcion para  el pin definido
void isr(void *parameter){
    int msg = false;
    xQueueSendFromISR(printSecondsQueue,&msg,NULL);
}


// Tarea: imprime la temperatura en intervalos varibales definidos
void printTemperature(){
    bool firstTime = true;
    int wait = 1000;
    double temp;
    while(1){
        if(xQueueReceive(printSecondsQueue,&firstTime,(TickType_t)0)){
            wait = (wait + 1000)%5000; //  Ajusta el teimepo de espara entre  impresiones
                                       // Para que mantenga entre 1000 y 5000
        }
        xQueueReceive(temperatureQueue,&temp,(TickType_t)10);
        ESP_LOGI(TAG,"La temperatura es: %f",temp);
        vTaskDelay(pdMS_TO_TICKS(wait+1000));
    }
}

void app_main()
{ 
      
    // Inicialización de la cola para datos UART
    uartDataQueue = xQueueCreate(1, sizeof(Intensities));
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
   // en el pin de interrupcion 
    gpio_intr_enable(INTERRUPT_GPIO);
    gpio_set_intr_type(INTERRUPT_GPIO,GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(INTERRUPT_GPIO,isr,NULL);

   //  Creacion de las Tareas
    xTaskCreate(&measureTemperature, "measure Temperature NTC", 2048, NULL, 4, NULL);
    xTaskCreate(&printTemperature,"Imprimir temperatura cada x segundos",2048,NULL,5,NULL);
    xTaskCreate(&temperatureSemaphore,"Semaforo de temperatura",2048,NULL,6,NULL);
    xTaskCreate(&send_to_queue,"Recibe data del UART",2048,NULL,3,NULL);
    

    ESP_LOGI(TAG,"Finished Task creation");
}
