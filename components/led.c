#include "led.h"
#include "termios.h"

//Periodo de actulizacion para las tareas , 50 milisegundos
#define UPDATE_PERIOD 50

static const char *TAG = "LED";


// llamado de colas para  la comunicacion entre tareas
    ;
QueueHandle_t currentChannel;
extern QueueHandle_t brightness;
extern QueueHandle_t temperatureQueue; 
extern QueueHandle_t uartDataQueue; // cola uart

ledc_timer_config_t ledc_timer = {
    .speed_mode       = LEDC_MODE,
    .timer_num        = LEDC_TIMER,
    .duty_resolution  = LEDC_DUTY_RES,
    .freq_hz          = LEDC_FREQUENCY,  // Frecuencia de salida de 5 kHz
    .clk_cfg          = LEDC_AUTO_CLK
};


// Inicalizacion  del control del LED por PWM
void ledc_init(void)
{
    // Configuracion del temporizador del PWM
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configuration del canal PMM
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Ciclo de trabajo al 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}




//Inicializacion del controlador PWM en el pin especifico
void ledc_init_with_pin(int PIN,int channel)
{   //Configuracion del temporizador del PWM
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configuracion del canal PWM en el pin especifico
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = channel,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PIN,
        .duty           = 8192, // Ciclo de trabajo a100%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(TAG,"LED initializated");


}



//Funcion:  Realizar desvanecimiento  de LEDs
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


// Funcion: Controlar los LEDs basdods en los valores de su respectiva cola
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

// Funcion : Ajustar LEDs segun la temperatura tipo semaforo
void temperatureSemaphore(void* px_queues){
    double temp;
    Intensities intensities = {8191, 8191, 8191};

    // Rangos de temperatura para controlar LEDs 
    int r_min = LED_R_MIN;
    int r_max = LED_R_MAX;
    int g_min = LED_G_MIN;
    int g_max = LED_G_MAX;
    int b_min = LED_B_MIN;
    int b_max = LED_B_MAX;


    while(true){
        if(xQueueReceive(temperatureQueue,&temp,(TickType_t)0)){
            if(r_min <= temp && temp <= r_max){
                // Se ajusta los LEDs según los rangos de temperatura definidos
                ledc_set_duty(LEDC_MODE,0,8191-intensities.r);
                ledc_update_duty(LEDC_MODE,0);
            }
            else{ 
                ledc_set_duty(LEDC_MODE,0,8191);
                ledc_update_duty(LEDC_MODE,0);
            }

            if(g_min <= temp && temp <= g_max){
                ledc_set_duty(LEDC_MODE,1,8191-intensities.g);
                ledc_update_duty(LEDC_MODE,1);
            }
            else{ 
                ledc_set_duty(LEDC_MODE,1,8191);
                ledc_update_duty(LEDC_MODE,1);
            }

            if(b_min <= temp && temp <= b_max){
                ledc_set_duty(LEDC_MODE,2,8191-intensities.b);
                ledc_update_duty(LEDC_MODE,2);
            }
            else{ 
                ledc_set_duty(LEDC_MODE,2,8191);
                ledc_update_duty(LEDC_MODE,2);
            }
        }

        //tarea que recibe datos de la  cola
        xQueueReceive(uartDataQueue,&intensities,(TickType_t)10); //esperan  10 milisegundos

        //vTaskDelay(pdMS_TO_TICKS(UPDATE_PERIOD));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
