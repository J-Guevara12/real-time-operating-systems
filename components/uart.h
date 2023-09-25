#ifndef UART_H
#define UAR_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

#define TXD (1)
#define RXD (3)
#define RTS (UART_PIN_NO_CHANGE)
#define CTS (UART_PIN_NO_CHANGE)

#define UART_PORT_NUM      (UART_NUM_0)
#define UART_BAUD_RATE     (115200)
#define TASK_STACK_SIZE    (4096)
#define BUF_SIZE (1024)

void echo_task(void *arg);

#endif