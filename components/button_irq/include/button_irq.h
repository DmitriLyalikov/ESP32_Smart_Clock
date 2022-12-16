/**
 * @file button_irq.h
 * @author Dmitri Lyalikov (Dlyalikov01@manhattan.edu)
 * @brief  Button Inerrupt Service Routines and Initialization Functions
 * @version 0.1
 * @date 2022-11-14
 * 
 * @copyright Copyright (c) 2022
 */

#ifndef BUTTON_IRQ_H
#define BUTTON_IRQ_H

#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_intr_alloc.h"
#include "driver/mcpwm.h"

#define GPIO_PWM_CONTROL    CONFIG_GPIO_PWM_CONTROL
#define GPIO_BRIGHTNESS_INC CONFIG_GPIO_BRIGHTNESS_INC
#define GPIO_BRIGHTNESS_DEC CONFIG_GPIO_BRIGHTNESS_DEC
#define GPIO_WEATHER_INC    CONFIG_GPIO_WEATHER_INC
#define GPIO_WEATHER_DEC    CONFIG_GPIO_WEATHER_DEC

/**
 * @brief
 *  Increment Display Brightness PWM duty cycle by 10%.
 *  Do nothing if Duty = 100
 * @param arg 
 */
void IRAM_ATTR gpio_brightness_inc_isr_handler(void* arg);

/**
 * @brief 
 *  Decrease Display Brightness PWM duty cycle by 10%
 *  Do nothing if Duty = 0
 * @param arg 
 */
void IRAM_ATTR gpio_brightness_dec_isr_handler(void* arg);

/**
 * @brief 
 *   Increase index of weather data by 1
 *   If index = len(weather_data - 1) set to 0 (Cycle)
 * @param arg 
 */
void IRAM_ATTR gpio_weather_inc_isr_handler(void* arg);

/**
 * @brief 
 *   Decrease index of weather data by 1
 *   If index = 0 set to len(weather_data - 1) (Cycle)
 * @param arg 
 */
void IRAM_ATTR gpio_weather_dec_isr_handler(void* arg);

void vPWM_Control_Task(void *pvParameters);

void vPWM_Control_Init(void);


#ifdef __cplusplus
}
#endif

#endif  // BUTTON_IRQ_H

