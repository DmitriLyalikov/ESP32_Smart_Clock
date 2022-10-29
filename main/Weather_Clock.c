/**
 * @file Weather_Clock.c
 * @author Dmitri Lyalikov (Dlyalikov01@manhattan.edu)
 * @brief  Entry Point to ESP32 Weather Clock Application for EECE 321 Final Project 
 * @version 0.1
 * @date 2022-10-28
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "perfmon.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"

#include "display.h"
#include "sys_resource.h"
#include "net_ctlr.h"

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    printf("********ESP32 Weather Clock Application********\n");
    wifi_init_sta();
    
}
