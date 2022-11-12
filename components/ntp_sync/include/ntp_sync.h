#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <esp_sntp.h>
#include <time.h>

void timezone_set(const char *timezone);

void ntp_start();

void ntp_wait_for_sync();