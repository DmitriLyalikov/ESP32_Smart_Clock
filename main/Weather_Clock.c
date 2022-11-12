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
#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "perfmon.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "smbus.h"
#include "i2c-lcd1602.h"
#include "sys_resource.h"
#include "net_ctlr.h"


static const char* TAG = "Weather Clock";
static SemaphoreHandle_t xTimeMutex;
QueueHandle_t xTimeSyncQueue();

#define LCD_NUM_ROWS               2
#define LCD_NUM_COLUMNS            32
#define LCD_NUM_VISIBLE_COLUMNS    16
#define I2C_MASTER_NUM           I2C_NUM_0
#define I2C_MASTER_TX_BUF_LEN    0                     // disabled
#define I2C_MASTER_RX_BUF_LEN    0                     // disabled
#define I2C_MASTER_FREQ_HZ       100000
#define I2C_MASTER_SDA_IO        21
#define I2C_MASTER_SCL_IO        22
#define LCD1602_I2C_ADDRESS 0x27


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

void lcd1602_task(void * pvParameter)
{
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

        // turn off backlight
        printf("backlight off");
        vTaskDelay(1000);
        i2c_lcd1602_set_backlight(lcd_info, false);

        // turn on backlight
        printf("backlight on");
        vTaskDelay(1000);
        i2c_lcd1602_set_backlight(lcd_info, true);

        printf("cursor on");
        vTaskDelay(1000);
        i2c_lcd1602_set_cursor(lcd_info, true);

        printf("display A at 0,0");
        vTaskDelay(1000);
        i2c_lcd1602_move_cursor(lcd_info, 0, 0);
        i2c_lcd1602_write_string(lcd_info, "hola");

        printf("scroll display left 8 places slowly");

        vTaskDelete(NULL);
}


void app_main(void)
{
    i2c_master_init();
    xTaskCreate(&lcd1602_task, "lcd1602_task", 4096, NULL, 5, NULL);

    while(1) {
        printf("Doing nothing in app_main");
        vTaskDelay(1000);
    }
}
