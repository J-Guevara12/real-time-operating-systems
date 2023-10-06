#ifndef LED_H
#define LED_H

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/queue.h"


// Definiciones de configuración LEDC
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (2) // Define el GPIO de salida
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Configura la resolución del ciclo de trabajo a 13 bits
#define LEDC_FREQUENCY          (5000) // Frecuencia en 5 kHz



// Rangos de temperatura para controlar los LEDs
#define LED_R_MIN 50
#define LED_R_MIN 50
#define LED_R_MAX 100
#define LED_G_MIN 30
#define LED_G_MAX 50
#define LED_B_MIN 0
#define LED_B_MAX 30

// Pines utilizados para controlar los LEDs
#define R_PIN 23
#define G_PIN 2
#define B_PIN 4


void ledc_init(void);  // Inicialización del controlador LEDC
void ledc_init_with_pin(int PIN,int channel); // Inicialización del controlador LEDC con un pin específico
void fade(void *pvParameter);  // Tarea para realizar un efecto de desvanecimiento de LEDs
void readFromQueue(void* queueToRead); // Tarea para leer datos de una cola y controlar los LEDs
void temperatureSemaphore(void* px_queues); // Tarea para ajustar los LEDs según la temperatura
void setupSemaphore(void);// Función para configurar el semáforo 

// Estructura para almacenar intensidades de colores de los LEDs
typedef struct {
    int r;
    int g;
    int b;
} Intensities;

#endif