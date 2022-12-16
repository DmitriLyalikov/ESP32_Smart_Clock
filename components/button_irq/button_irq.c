#include "button_irq.h"

static volatile uint8_t lcd_duty_cycle;


void IRAM_ATTR gpio_brightness_inc_isr_handler(void* arg){

    if (lcd_duty_cycle < 100) {
        lcd_duty_cycle += 10;
    }
}

void IRAM_ATTR gpio_brightness_dec_isr_handler(void* arg){

    if (lcd_duty_cycle > 0) {
        lcd_duty_cycle -= 10;
    }
}

