/**
 * @file ntp_sync.c
 * @author Dmitri Lyalikov (Dlyalikov01@manhattan.edu)
 * @brief Sync and Timing functions to get local time from NTP server and set in RTC
 * @version 0.1
 * @date 2022-11-12
 */

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <esp_sntp.h>
#include <time.h>

void timezone_set(const char *timezone);

void ntp_start();

void ntp_wait_for_sync();