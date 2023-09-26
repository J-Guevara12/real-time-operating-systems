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


int rang_max_R = 60     ;
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
QueueHandle_t uartDataQueue; // cola uart

// Interuption
void isr(void *parameter){
    int msg = 10;
    xQueueSendFromISR(currentChannel,&msg,NULL);
}

// Medir temperatura y enviar a la cola
void measureTemperature(void *parameter){
    while(1){
        int adc_value;
        double temperature;

        ESP_ERROR_CHECK(adc_oneshot_read(adc_handler, ADC_CHAN, &adc_value));

        double resistance = ((double)adc_value * 10000.0) / 4096.0;
        double temperature =  calculateTemperature(resistance);


        xQueueSend(temperatureQueue, &temperature, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(5000)); //Esperar 5 segundos

    }
}


// Encender o apagar led Respecto a la temperatura
void controlLEDs(void *parameter) {
    while (1) {
        double temperature;
        if (xQueueReceive(temperatureQueue, &temperature, portMAX_DELAY)) {
            // Verificar si la temperatura está dentro del rango deseado
            if (temperature >= rang_min_R && temperature <= rang_max_R) {
                // Encender LED Rojo
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 8192);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            } else {
                // Apagar LED Rojo
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            }

            if (temperature >= rang_min_B && temperature <= rang_max_B) {
                // Encender LED Azul
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 8192);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            } else {
                // Apagar LED Azul
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            }

            if (temperature >= rang_min_G && temperature <= rang_max_G) {
                // Encender LED Verde
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 8192);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            } else {
                // Apagar LED Verde
                ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, 0);
                ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            }
        }
    }
}






  //Envio de datos del uart para comparacion
void uartDataHandler(void *parameter) {
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1) {
        int len = uart_read_bytes(UART_PORT_NUM, data, (BUF_SIZE - 1), portMAX_DELAY);

        if (len) {
            data[len] = '\0';

            // Envía los datos UART a la cola
            xQueueSend(uartDataQueue, data, portMAX_DELAY);

            // Realiza la comparación con los datos de la cola de los Leds
            char ledData[BUF_SIZE];
            if (xQueueReceive(brightness, ledData, portMAX_DELAY)) {
               
                if (strcmp((char *)data, ledData) == 0) {
                    
                     
                
                } else {
                    
                }
            }
        }
    }
}

void app_main()
{ 
      
    // Inicialización de la cola para datos UART
    uartDataQueue = xQueueCreate(10, sizeof(char) * BUF_SIZE);
    currentChannel = xQueueCreate(1,sizeof(int));
    brightness = xQueueCreate(1,sizeof(int));
    temperatureQueue = xQueueCreate(1, sizeof(double));

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
    xTaskCreate(&controlLEDs, "Control LEDs", 2048, NULL, 5, NULL);
    

    ESP_LOGI(TAG,"Finished Task creation");
}
