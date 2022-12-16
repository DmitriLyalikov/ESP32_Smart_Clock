/**
 * @file Weather_Clock.c
 * @author Dmitri Lyalikov (Dlyalikov01@manhattan.edu)
 * @brief  Entry Point to ESP32 Weather Clock Application for EECE 321 Final Project 
 * @version 1.2
 * @date 2022-11-14
 */

#include <stdio.h>
#include <string.h>
#include <time.h> 

#include <sys/time.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "perfmon.h"
#include "sdkconfig.h"
#include "driver/i2c.h"

#include "ble_config.h"
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "net_ctlr.h"
#include "ntp_sync.h"
#include "weather.h"
#include "button_irq.h"

#define TIMEZONE              CONFIG_TIMEZONE
#define CITY                  CONFIG_CITY
#define NTP_RESYNC_PERIOD     CONFIG_NTP_RESYNC_PERIOD
#define WEATHER_RESYNC_PERIOD CONFIG_NTP_RESYNC_PERIOD

static const char* TAG = "Weather Clock";

// FreeRTOS Task Shared Resources
static TimerHandle_t xNTP_SYNC_TIMER;
static TimerHandle_t xWEATHER_SYNC_TIMER;
static QueueHandle_t xWeatherSyncQueue;

/**
 * @brief Initialise I2C Master Bus before display communication
 */
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
 * Configure I2C, SMbus, and LCD device
 * Set time zone, get weather data
 * Updates time every 500 ms, and writes to Display
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
     
    weather_data weather_now;
    uint32_t ulWeatherReady;
    vReadQueue(&weather_now, xWeatherSyncQueue);
    char float_read[50];
    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;
    // Set timezone to CONFIG_TIMEZONE
    time(&now);
    setenv("TZ", TIMEZONE, 1);
    tzset();
    i2c_lcd1602_move_cursor(lcd_info, 0, 1);
    i2c_lcd1602_write_string(lcd_info, CONFIG_CITY);
    i2c_lcd1602_move_cursor(lcd_info, sizeof(CONFIG_CITY), 1);
    i2c_lcd1602_write_string(lcd_info, weather_now.description);
    i2c_lcd1602_move_cursor(lcd_info, 0, 3);
    i2c_lcd1602_write_string(lcd_info, "Temp(C):");
    i2c_lcd1602_move_cursor(lcd_info, 8, 3);
    sprintf(float_read, "%d", (int)weather_now.temperature);
    i2c_lcd1602_write_string(lcd_info, float_read);
    i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_DEGREE);
    for (;;)
    {
      // Get Current Time from RTC 
      now = time(NULL);
      // Non-Blocking check if new Weather data is ready 
      ulWeatherReady = ulTaskNotifyTake(pdTRUE, 0);
      if (ulWeatherReady) {
          vReadQueue(&weather_now, xWeatherSyncQueue);
          i2c_lcd1602_move_cursor(lcd_info, sizeof(CONFIG_CITY), 1);
          i2c_lcd1602_write_string(lcd_info, weather_now.description);
          i2c_lcd1602_move_cursor(lcd_info, 0, 3);
          i2c_lcd1602_write_string(lcd_info, "Temp(C):");
          i2c_lcd1602_move_cursor(lcd_info, 8, 3);
          sprintf(float_read, "%d", (int)weather_now.temperature);
          i2c_lcd1602_write_string(lcd_info, float_read);
          i2c_lcd1602_write_char(lcd_info, I2C_LCD1602_CHARACTER_DEGREE);
      }

      localtime_r(&now, &timeinfo);
      strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

      i2c_lcd1602_move_cursor(lcd_info, 0, 2);
      i2c_lcd1602_write_string(lcd_info, strftime_buf);
      ESP_LOGI(TAG, "Got current date/time in %s: %s", CITY, strftime_buf);   
      i2c_lcd1602_move_cursor(lcd_info, 0, 1);
      i2c_lcd1602_write_string(lcd_info, CONFIG_CITY);
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief NTP Synchronization Timer Callback
 *     Must be used after WiFI connection is established.
 *     Syncs RTC clock time with ntp server
 * @param xTimerHandle
 */
void vTimeSync_Callback(TimerHandle_t xTimer) {
  ESP_LOGI(TAG, "Re-Synchronizing RTC time from NTP Server\n");
  ntp_wait_for_sync();
}

/**
 * @brief Thread-Safe Weather Synchronization Timer Callback
 *      Must be used after WiFi connection is established.
 *      Sends Weather request to weather server api and processes JSON response
 *      write Weather_Payload_t to xWeatherQueue mailbox 
 * @param xTimerHandle 
 */
void vWeatherSync_Callback(TimerHandle_t xTimer) {
  ESP_LOGI(TAG, "Starting Weather Request...");
  http_weather_request(xWeatherSyncQueue);
  xTaskNotifyGive(vdisplay_task);
}

/**
 * @brief Entry Point to Application
 *      Display task is very active and resource needy, so it is put on Core 1
 *      Software timers: xNTP_SYNC_TIMER and WEATHER_SYNC_TIMER are on Core 0,
 *      they do not fire often and can generally be neglected
 *      Networking functionality is kept on core 0
 */
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
    ESP_LOGI(TAG, "********ESP32 Weather Clock Application********");
    // Initialise WiFI station
    wifi_init_sta();
    // Start RTC sync with NTP 
    ntp_start();
    ntp_wait_for_sync();
  
    xNTP_SYNC_TIMER = xTimerCreate("NTP Resync Timer",
                                  (NTP_RESYNC_PERIOD * 1000) / portTICK_PERIOD_MS,
                                  pdTRUE,
                                  ( void * ) 0,
                                  vTimeSync_Callback);
    if( xTimerStart(xNTP_SYNC_TIMER, 0 ) != pdPASS ) {
      ESP_LOGI(TAG, "Could not start xNTP_SYNC_TIMER");
    }

    xWeatherSyncQueue = vQueueInit();
    ESP_LOGI(TAG, "Starting Weather Request...");
    initialise_weather_data_retrieval(WEATHER_RESYNC_PERIOD);
    http_weather_request(xWeatherSyncQueue);
    xWEATHER_SYNC_TIMER = xTimerCreate("Weather Resync Timer",
                                      (WEATHER_RESYNC_PERIOD * 1000) / portTICK_PERIOD_MS,
                                      pdTRUE,
                                      (void * ) 0,
                                      vWeatherSync_Callback);
    if( xTimerStart(xWEATHER_SYNC_TIMER, 0 ) != pdPASS ) {
      ESP_LOGI(TAG, "Could not start xWEATHER_SYNC_TIMER");
    }
    // Start display task on Core 1
    xTaskCreatePinnedToCore(&vdisplay_task,
                            "xTask_Display",
                            2048,
                            NULL,
                            1, 
                            NULL,
                            1);
    vPWM_Control_Init();
}
