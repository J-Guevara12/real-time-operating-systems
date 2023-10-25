#include "nvs_flash.h"
#include "wifi_app.h"
#include "esp_sntp.h"



#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/queue.h"


#include "led.h"
#include "adc.h"
#include "uart.h"
#include "thermistor.h"

//-------------------------------------------------------------------------------
//              DECLARACION DE MEDIR TEMPERATURA

#define INTERRUPT_GPIO 13
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


//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------

//              DECLARACION E IMPLEMTACION NTP

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

char Current_Date_Time[100];


	void Get_current_date_time(char *date_time){
		char strftime_buf[64];
		time_t now;
			struct tm timeinfo;
			time(&now);
			localtime_r(&now, &timeinfo);

				
					setenv("TZ", "COT-5", 1);
					tzset();
					localtime_r(&now, &timeinfo);

					strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
					ESP_LOGI(TAG, "The current date/time in Delhi is: %s", strftime_buf);
					strcpy(date_time,strftime_buf);
	}



	static void initialize_sntp(void){
		ESP_LOGI(TAG, "Initializing SNTP");
		esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
		esp_sntp_setservername(0, "0.co.pool.ntp.org");
		sntp_set_time_sync_notification_cb(time_sync_notification_cb);
	#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
		sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
	#endif
		esp_sntp_init();
	}

	static void obtain_time(void){


		initialize_sntp();
		// wait for time to be set
		time_t now = 0;
		struct tm timeinfo = { 0 };
		int retry = 0;
		const int retry_count = 10;
		while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
			ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
		}
		time(&now);
		localtime_r(&now, &timeinfo);
	}




	void Set_SystemTime_SNTP()  {

		time_t now;
			struct tm timeinfo;
			time(&now);
			localtime_r(&now, &timeinfo);
		
			if (timeinfo.tm_year < (2023 - 1900)) {
				ESP_LOGI(TAG, "la Hora aun no esta configurada, conectadnod al Wi-Fi y obteniendo hora  por NTP.");
				obtain_time();
				
				time(&now);
			}
	}


//------------------------------------------------------------------------------



void app_main(void)
{
	//--------------------------------------------------------------------------
	//               CONEXION WIFI Y NTP


    // Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	// Start Wifi
	wifi_app_start();

    //star ntp
    //Set_SystemTime_SNTP();

    //----------------------------------------------------------------------------
	//            MEDIR TEMPERATURA

    // Inicialización de la cola para datos UART
    uartDataQueue = xQueueCreate(1, sizeof(Intensities));
    temperatureQueue = xQueueCreate(2, sizeof(double));
    printSecondsQueue = xQueueCreate(1,sizeof(bool));

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
    xTaskCreate(&measureTemperature, "measure Temperature NTC", 2048, NULL, 4, NULL);
    xTaskCreate(&printTemperature,"Imprimir temperatura cada x segundos",2048,NULL,5,NULL);
    

    ESP_LOGI(TAG,"Finished Task creation");
}

