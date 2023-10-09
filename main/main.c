
#include "nvs_flash.h"
#include "wifi_app.h"
#include "esp_sntp.h"



void  initializate_sntp(){
	ESP_LOGI(TAG, "Inicializar la  SNTP");
     sntp_setoperatingmode(SNTP_OPMODE_POLL);
     sntp_setservername (0, "pool.ntp.org");
     sntp_init();
}

void obtanin_time(){
    initialize_sntp();

    time_t now = 0;
    struct tm timeinfo {0};
	while (timeinfo.tm_year < (2023 - 2000)) {
        ESP_LOGI(TAG, "Esperando  hora del servidor NTP.");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    // Muestra la Hora
    ESP_LOGI(TAG, "Hora actual: %s", asctime(&timeinfo));
    
    
}


void app_main(void)
{
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
}

