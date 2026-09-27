#include <stdio.h>
#include "driver/gpio.h"
#include "dht.h"
#include "dht11.h"
#include "common.h"
#include "sdkconfig.h"

//Definition
#define DHT_WAKETIME 1500
#define DHT_WAITTIME 500

//Initiation
static const char *TAG = "DHT11";
static dht_sensor_type_t sensor_type = DHT_TYPE_DHT11;
static bool initate_sensor = false;

//Private Functions
static esp_err_t data_success_int(int16_t * tempreture, int16_t * humidity, dht_int_data_t * data_struct, QueueHandle_t QueueHandler)
{
    data_struct->tempreture = *tempreture;
    data_struct->humidity = *humidity;
    data_struct->status = true;
    ESP_LOGI(TAG, "Get Integer [Temperature\\Humidity:%d\\%d]", data_struct->tempreture, data_struct->humidity);
    BaseType_t check = xQueueSend(QueueHandler, data_struct, pdMS_TO_TICKS(DHT_WAITTIME));
    if (check != pdTRUE) return ESP_ERR_QUEUE_FULL;
    return ESP_OK;
}

static esp_err_t data_failed(dht_int_data_t * data_struct, QueueHandle_t QueueHandler)
{
    data_struct->status = false;
    ESP_LOGW(TAG, "Sensor Read Failed, Sending Error Status");
    BaseType_t check = xQueueSend(QueueHandler, data_struct, pdMS_TO_TICKS(DHT_WAITTIME));
    if (check != pdTRUE) return ESP_ERR_QUEUE_FULL;
    return ESP_OK;
}

void read_dht_data(void* arg)
{
    QueueHandle_t dht_handler = (QueueHandle_t)arg;
    dht_int_data_t dht_int_datas;
    TickType_t lastwaketime;
    float tempreture = 0;
    float humidity = 0;
    for(;;)
    {
        if (!initate_sensor){
            dht_read_float_data(sensor_type, CONFIG_DHT_GPIO, &humidity, &tempreture); //Drop the first initiate data
            initate_sensor = true;
            lastwaketime = xTaskGetTickCount();
        } 
        xTaskDelayUntil(&lastwaketime, pdMS_TO_TICKS(DHT_WAKETIME)); //1s<X<2s
        esp_err_t check = dht_read_float_data(sensor_type, CONFIG_DHT_GPIO, &humidity, &tempreture);
        if (check == ESP_OK)
        {
            ESP_LOGI(TAG, "Get Temperature\\Humidity:%f\\%f", tempreture, humidity);
            int16_t I_tempreture = (int16_t)(tempreture * 10);
            int16_t I_humidity = (int16_t)(humidity * 10);
            data_success_int(&I_tempreture, &I_humidity, &dht_int_datas, dht_handler);
        }
        else
        {
            dht_int_datas.status = false;
            data_failed(&dht_int_datas, dht_handler);
            ESP_LOGI(TAG, "Sensor data read failed!");
        }
    }
}