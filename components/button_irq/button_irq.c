#include "button_irq.h"

static const char *TAG = "LCD_PWM_Controller";
static volatile BaseType_t lcd_duty_cycle;

void IRAM_ATTR gpio_brightness_inc_isr_handler(void* arg){

    if (lcd_duty_cycle < 100) {
        lcd_duty_cycle += 10;
    }
    vTaskNotifyGiveFromISR(vPWM_Control_Task, lcd_duty_cycle);
}

void IRAM_ATTR gpio_brightness_dec_isr_handler(void* arg){

    if (lcd_duty_cycle > 0) {
        lcd_duty_cycle -= 10;
    }
    vTaskNotifyGiveFromISR(vPWM_Control_Task, lcd_duty_cycle);
}

void vPWM_Control_Task(void *pvParameters){
    // Disable interrupts to access volatile global variable safely
    taskDISABLE_INTERRUPTS();
    // Critical Section 
    uint8_t ulDuty_Cycle = lcd_duty_cycle;
    // Reenable Interrupts
    taskENABLE_INTERRUPTS();
    // Non-Blocking check if duty cycle changed 
    ulDuty_Cycle = ulTaskNotifyTake(pdTRUE, 0);
    ESP_ERROR_CHECK(mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, ulDuty_Cycle));
    while(1){
        ulDuty_Cycle = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        ESP_ERROR_CHECK(mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, ulDuty_Cycle));
    }
}

/**
 * @brief Initialize PWM Module and assign GPIO_PWM_CONTROL to UNIT 0
 * and start Control Task
 */
void vPWM_Control_Init(void){
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, GPIO_PWM_CONTROL);
    mcpwm_config_t pwm_config = {
        .frequency     = 30000,
        .cmpr_a        = 0,
        .counter_mode  = MCPWM_UP_COUNTER,
        .duty_mode     = MCPWM_DUTY_MODE_0,
    };
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_start(MCPWM_UNIT_0, MCPWM_TIMER_0);
    ESP_LOGI(TAG, "Starting PWM Control Task");
    xTaskCreatePinnedToCore(&vPWM_Control_Task,
                            "xPWM_Control",
                            2048,
                            NULL,
                            1, 
                            NULL,
                            1);
    
}

