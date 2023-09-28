#include "uart.h"
#include "led.h"

static const char *TAG = "UART";

extern QueueHandle_t uartDataQueue; // cola uart

void uart_init(){
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM,BUF_SIZE*2,0,0,NULL,0));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, TXD, RXD, RTS, CTS));

    ESP_LOGI(TAG,"Initializated UART");
}

void echo_task(void *arg){
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        // Write data back to the UART
        uart_write_bytes(UART_PORT_NUM, (const char *) data, len);
        if (len) {
            data[len] = '\0';
            ESP_LOGI(TAG, "Recv str: %s", (char *) data);
        }
    }
}

void send_to_queue(void *arg){
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    Intensities intensities = {8191, 8191, 8191};

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(UART_PORT_NUM, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        if (len) {
            data[1] = '\0';
            if(strcmp((char *)&data[0],"r")==0){
                intensities.r = atoi((char *) &data[2]);
            }
            if(strcmp((char *)&data[0],"g")==0){
                intensities.g = atoi((char *) &data[2]);
            }
            if(strcmp((char *)&data[0],"b")==0){
                intensities.b = atoi((char *) &data[2]);
            }
            xQueueSend(uartDataQueue,&intensities,(TickType_t)10);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}