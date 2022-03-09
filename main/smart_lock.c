#include <stdio.h>

#include "driver/mcpwm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "HD44780.h"

void init_lock_motor(){
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 33);
    mcpwm_config_t config;
    config.frequency = 50;
    config.cmpr_a = 10;
    config.cmpr_b = 10;
    config.duty_mode = MCPWM_DUTY_MODE_0;
    config.counter_mode = MCPWM_UP_COUNTER;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &config);

    mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 2.5);
}

void toggle_lock(){
    static bool open = false;
    if(open == false){
        open = true;
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 9.5);
    }else{
        open = false;
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 2.5);
    }
}

void unlock(){
    lcd_clear_display();

    lcd_set_cursor_location(0, 4);

    lcd_print_string("unlocked");
    
    toggle_lock();

    vTaskDelay(4000 / portTICK_PERIOD_MS);

    toggle_lock();

    lcd_clear_display();

    lcd_set_cursor_location(0, 5);

    lcd_print_string("locked");
}

void app_main(void)
{
    init_lock_motor();

    gpio_reset_pin(36);

    gpio_set_direction(36, GPIO_MODE_INPUT);

    printf("got here\n");

    lcd_init(1, 0, 0);

    lcd_set_cursor_location(0, 5);

    lcd_print_string("locked");

    for(;;){
        if(gpio_get_level(36) == 1){
            unlock();
        }
        vTaskDelay(25 / portTICK_PERIOD_MS);
    }
}
