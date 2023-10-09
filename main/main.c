
#include "nvs_flash.h"
#include "wifi_app.h"
#include "esp_sntp.h"



void  initializate_sntp(){
	ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}

void obtanin_time(){
    initialize_sntp();
    //Tiempo de espera para que se establezca el tiempo
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Espradno que se establesca la hora... (%d/%d)", retry, retry_count);
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

    //star ntp
    Set_SystemTime_SNTP();

    while(1)  {

		Get_current_date_time(Current_Date_Time);
		printf("Fecha y hora actual = %s\n",Current_Date_Time);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
}
}

