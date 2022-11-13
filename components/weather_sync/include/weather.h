#ifndef WEATHER_H
#define WEATHER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*weather_data_callback)(uint32_t *args);

typedef struct {
    unsigned int humidity;
    float temperature;
    float pressure;
    unsigned long retreival_period;
    weather_data_callback data_retreived_cb;
} weather_data;

#define ESP_ERR_WEATHER_BASE 0x50000
#define ESP_ERR_WEATHER_RETREIVAL_FAILED          (ESP_ERR_WEATHER_BASE + 1)

void on_weather_data_retrieval(weather_data_callback data_retreived_cb);
void initialise_weather_data_retrieval(unsigned long retreival_period);
void http_weather_request(void);

#ifdef __cplusplus
}
#endif

#endif  // WEATHER_H