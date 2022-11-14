#ifndef WEATHER_H
#define WEATHER_H

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*weather_data_callback)(uint32_t *args);

typedef struct weather_data {
    unsigned int humidity;
    float temperature;
    float pressure;
    char description[12];
    unsigned long retreival_period;
    weather_data_callback data_retreived_cb;
} weather_data;

#define ESP_ERR_WEATHER_BASE 0x50000
#define ESP_ERR_WEATHER_RETREIVAL_FAILED          (ESP_ERR_WEATHER_BASE + 1)

void on_weather_data_retrieval(weather_data_callback data_retreived_cb);
void initialise_weather_data_retrieval(unsigned long retreival_period);
void http_weather_request(QueueHandle_t Weather_Queue, SemaphoreHandle_t mutex);

/**
 * @brief Initialize Mailbox Queue
 * @returns QueueHandle_T: Handle to initialize queue  
 */
QueueHandle_t vQueueInit(void);

/**
 * @brief Write a value to queue
 * 
 * @param Queue : queue handle of type QueueHandle_t
 * @param ulNewValue : uin16_t value to write
 */
void vUpdateQueue(QueueHandle_t Queue, weather_data pxData, SemaphoreHandle_t mutex);


/**
 * @brief Read queue and pop value read, 
 * 
 * @param pxData : Pointer to struct of type weather_data to read into
 * @param Queue  : Queue handle of type QueueHandle_t to read from
 */
void vReadQueue(weather_data *pxData, QueueHandle_t Queue, SemaphoreHandle_t mutex);



#ifdef __cplusplus
}
#endif

#endif  // WEATHER_H