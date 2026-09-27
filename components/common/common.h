#ifndef __COMMON_H__
#define __COMMON_H__
#include "inttypes.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_err.h"
#include "esp_log.h"

#define NORMAL_STACKSIZE 2048
#define NETWORK_STACKSIZE 8192
#define JSON_BUFFER_SIZE 128

typedef struct {
    int16_t tempreture;
    int16_t humidity;
    bool status;
} dht_int_data_t;

#define WORKFLOW_ERR_BASE               0x10000
#define ESP_ERR_QUEUE_FULL              (WORKFLOW_ERR_BASE + 1)
#define ESP_ERR_QUEUE_EMPTY             (WORKFLOW_ERR_BASE + 2)
#define ESP_ERR_JSON_BUILD_FAILED       (WORKFLOW_ERR_BASE + 3)

typedef struct {
    QueueHandle_t worker_dht_json_handler;
    QueueHandle_t worker_json_net_handler;
} QueueGroupHandle_t;

#endif /*__COMMON_H__*/