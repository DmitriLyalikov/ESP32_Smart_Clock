/**
 * @file ntp_sync.c
 * @author Dmitri Lyalikov (Dlyalikov01@manhattan.edu)
 * @brief 
 * @version 0.1
 * @date 2022-11-12
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <esp_sntp.h>
#include <time.h>

void timezone_set(const char *timezone)
{
	setenv("TZ", timezone, 1);
	tzset();
}

void ntp_start()
{
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "us.pool.ntp.org");
	sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
	sntp_init();
}

void ntp_wait_for_sync()
{
	while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET) {
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}