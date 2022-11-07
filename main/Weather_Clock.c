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

#include "i2c-lcd1602.h"
#include "sys_resource.h"
#include "net_ctlr.h"
#include "ble_config.h"

static const char* TAG = "Weather Clock";
static SemaphoreHandle_t xTimeMutex;
QueueHandle_t xTimeSyncQueue();

static void i2c_master_init(void)
{
    int i2c_master_port = CONFIG_I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = CONFIG_I2C_MASTER_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.scl_io_num = CONFIG_I2C_MASTER_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_DISABLE;  // GY-2561 provides 10kΩ pullups
    conf.master.clk_speed = CONFIG_I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       CONFIG_I2C_MASTER_RX_BUF_LEN,
                       CONFIG_I2C_MASTER_TX_BUF_LEN, 0);
}

/**
 * @brief Display Task
 * 
 */
static void vdisplay_task(void *pvParameter) {
    // Init I2C Master
    i2c_master_init();
    i2c_port_t i2c_num = CONFIG_I2C_MASTER_NUM;
    uint8_t address = CONFIG_LCD1602_I2C_ADDRESS;

    // Init SMBus
    smbus_info_t * smbus_info = smbus_malloc();
    ESP_ERROR_CHECK(smbus_init(smbus_info, i2c_num, address));
    ESP_ERROR_CHECK(smbus_set_timeout(smbus_info, 1000 / portTICK_RATE_MS));

    // Init LCD1602 device
    i2c_lcd1602_info_t * lcd_info =  i2c_lcd1602_malloc();
    ESP_ERROR_CHECK(i2c_lcd1602_init(lcd_info, smbus_info, true,
                                CONFIG_LCD_NUM_ROWS, CONFIG_LCD_NUM_COLUMNS, CONFIG_LCD_NUM_VISIBLE_COLUMNS));
    ESP_ERROR_CHECK(i2c_lcd1602_reset(lcd_info));
    i2c_lcd1602_set_backlight(lcd_info, true);

    i2c_lcd1602_write_string(lcd_info, "Hello");
    i2c_lcd1602_move_cursor(lcd_info, 0, 1);
    i2c_lcd1602_write_string(lcd_info, "World");

    for (;;)
    {

        
    }
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
    ESP_LOGI(TAG, "********ESP32 Weather Clock Application********\n");

    wifi_init_sta();
    xTaskCreatePinnedToCore(&vdisplay_task,
                            "xTask_Display",
                            3000,
                            NULL,
                            1, 
                            NULL,
                            1);
}
