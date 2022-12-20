#include "button_irq.h"

static const char *TAG = "LCD_PWM_Controller";
// Default LCD brightness initialized to 100%
static volatile BaseType_t lcd_duty_cycle = 100;

void IRAM_ATTR gpio_brightness_inc_isr_handler(void* arg){

    if (lcd_duty_cycle < 100) {
        lcd_duty_cycle += 10;
    }
    ESP_LOGI(TAG, "IRQ INC Fired");
    vTaskNotifyGiveFromISR(vPWM_Control_Task, (int *)lcd_duty_cycle);
}

/**
 * @brief 
 *   Decrease index of weather data by 1
 *   If index = 0 set to len(weather_data - 1) (Cycle)
 * @param arg 
 */
void IRAM_ATTR gpio_brightness_dec_isr_handler(void* arg){

    if (lcd_duty_cycle > 0) {
        lcd_duty_cycle -= 10;
    }
    ESP_LOGI(TAG, "IRQ DEC Fired");
    vTaskNotifyGiveFromISR(vPWM_Control_Task, (int *)lcd_duty_cycle);
}

/**
 * @brief Low priority, low resource task that sets LCD brightness and
 *  blocks until a new value is set with the button ISRs.
 * 
 * @param pvParameters 
 */
void vPWM_Control_Task(void *pvParameters){
    // Disable interrupts to access volatile global variable safely
    taskDISABLE_INTERRUPTS();
    // Critical Section 
    BaseType_t ulDuty_Cycle = lcd_duty_cycle;
    // Reenable Interrupts
    taskENABLE_INTERRUPTS();
    // Non-Blocking check if duty cycle changed 
    ulDuty_Cycle = ulTaskNotifyTake(pdTRUE, 0);
    ESP_ERROR_CHECK(mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, ulDuty_Cycle));
    while(1){
        // Block until the brightness value is changed by user
        ulDuty_Cycle = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // ESP_LOGI(TAG, "Setting PWM Duty Cycle: %d", ulDutyCycle);
        ESP_ERROR_CHECK(mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, ulDuty_Cycle));
    }
}

/**
 * @brief Initialize PWM Module and assign GPIO_PWM_CONTROL to UNIT 0
 * and start Control Task
 */
void vPWM_Control_Init(void){
    // Configure button GPIO INC/DEC as an input with internal pull-up resistor
    gpio_config_t button_config = {
        .pin_bit_mask = (1 << GPIO_BRIGHTNESS_INC) | (1 << GPIO_BRIGHTNESS_DEC),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,  // Using internal pull ups
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE     // buttons are pulled up, trigger on falling edge
    };
    gpio_config(&button_config);

    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_IRAM));
    gpio_isr_handler_add(GPIO_BRIGHTNESS_INC, gpio_brightness_inc_isr_handler, NULL);
    gpio_isr_handler_add(GPIO_BRIGHTNESS_DEC, gpio_brightness_dec_isr_handler, NULL);

    // Enable HW Interrupts on the pins
    gpio_intr_enable(GPIO_BRIGHTNESS_INC);
    gpio_intr_enable(GPIO_BRIGHTNESS_DEC);

    ESP_LOGI(TAG, "Registered PWM ISRs");

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

