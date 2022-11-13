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
#include <string.h>
#include <time.h>                       
#include <sys/time.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "perfmon.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "driver/i2c.h"

#include "ntp_sync.h"
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "sys_resource.h"
#include "net_ctlr.h"
#include "ble_config.h"

#define TIMEZONE CONFIG_TIMEZONE
#define CITY     CONFIG_CITY

static const char* TAG = "Weather Clock";
static SemaphoreHandle_t xTimeMutex;
QueueHandle_t xTimeSyncQueue;

static void i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    memset(&conf, 0, sizeof(i2c_config_t));
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_MASTER_RX_BUF_LEN,
                       I2C_MASTER_TX_BUF_LEN, 0);
}

/**
 * @brief Display Task 
 * 
 * Low (1) Priority FreeRTOS Task that will block until mailbox is full 
 * 
 * 
 */
static void vdisplay_task(void *pvParameter) {
    i2c_master_init();
    i2c_port_t i2c_num = I2C_MASTER_NUM;
    uint8_t address = LCD1602_I2C_ADDRESS;

    // Set up the SMBus
    smbus_info_t * smbus_info = smbus_malloc();
    ESP_ERROR_CHECK(smbus_init(smbus_info, i2c_num, address));
    ESP_ERROR_CHECK(smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS));

    // Set up the LCD1602 device with backlight off
    i2c_lcd1602_info_t * lcd_info = i2c_lcd1602_malloc();
    ESP_ERROR_CHECK(i2c_lcd1602_init(lcd_info, smbus_info, true,
                                    LCD_NUM_ROWS, LCD_NUM_COLUMNS, LCD_NUM_VISIBLE_COLUMNS));

    ESP_ERROR_CHECK(i2c_lcd1602_reset(lcd_info));

    i2c_lcd1602_write_string(lcd_info, "    Hello Dmitri");
    i2c_lcd1602_move_cursor(lcd_info, 0, 1);
    i2c_lcd1602_write_string(lcd_info, CONFIG_CITY);

    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;
    // Set timezone to CONFIG_TIMEZONE
    time(&now);
    setenv("TZ", TIMEZONE, 1);
    tzset();
    for (;;)
    {
      // Get Current Time from RTC 
      localtime_r(&now, &timeinfo);
      strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
      i2c_lcd1602_move_cursor(lcd_info, 0, 2);
      i2c_lcd1602_write_string(lcd_info, strftime_buf);
      i2c_lcd1602_move_cursor(lcd_info, 0, 1);
      i2c_lcd1602_write_string(lcd_info, CONFIG_CITY);
      ESP_LOGI(TAG, "Got current date/time in %s: %s\n", CITY, strftime_buf);   
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief NTP Synchronization Task 
 *     Must be used after WiFI connection is established.
 *     
 * 
 * @param pvParameter 
 */
void vTimeSync_Task(void *pvParameter) {

}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    timezone_set(TIMEZONE);
    ESP_LOGI(TAG, "********ESP32 Weather Clock Application********\n");
    wifi_init_sta();
    ntp_start();
    ntp_wait_for_sync();
    xTaskCreatePinnedToCore(&vdisplay_task,
                            "xTask_Display",
                            3000,
                            NULL,
                            1, 
                            NULL,
                            1);
}
