/**
 * @file lock_actuation.c
 * @author Nolan Davenport (nolanrdavenport@gmail.com)
 * @brief Functions for performing lock acuation.
 * @version 0.1
 * @date 2022-05-29
 * 
 * @copyright Copyright (c) 2022
 * 
 *  This file is part of smart_lock.
 *  
 *  smart_lock is free software: you can redistribute it and/or modify it under the terms of the 
 *  GNU General Public License as published by the Free Software Foundation, either version 3 of 
 *  the License, or (at your option) any later version.
 *  
 *  smart_lock is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without 
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 *  General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with Foobar. If not, 
 *  see <https://www.gnu.org/licenses/>. 
 */
#include <stdio.h>

#include "driver/mcpwm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "HD44780.h"
#include "smart_lock_utils.h"
#include "lock_actuation.h"

void set_lock_state(lock_state_t state){
    if(state == OPEN){
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, DUTY_CYCLE_OPEN_STATE);
    }else if(state == CLOSED){
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, DUTY_CYCLE_CLOSED_STATE);
    }
}

void init_lock_motor(){
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, 33);
    mcpwm_config_t config;
    config.frequency = 50;
    config.cmpr_a = 10;
    config.cmpr_b = 10;
    config.duty_mode = MCPWM_DUTY_MODE_0;
    config.counter_mode = MCPWM_UP_COUNTER;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &config);

    set_lock_state(CLOSED);
}

void unlock(){
    lcd_clear_display();

    lcd_set_cursor_location(0, 4);

    lcd_print_string("unlocked");
    
    set_lock_state(OPEN);

    delay_ms(4000);

    set_lock_state(CLOSED);

    lcd_clear_display();

    lcd_set_cursor_location(0, 5);

    lcd_print_string("locked");
}