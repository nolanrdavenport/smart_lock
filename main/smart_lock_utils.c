/**
 * @file smart_lock_utils.c
 * @author Nolan Davenport (nolanrdavenport@gmail.com)
 * @brief Some useful utilities.
 * @version 0.1
 * @date 2022-05-28
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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "smart_lock_utils.h"

void delay_ms(int ms){
    vTaskDelay(ms / portTICK_PERIOD_MS);
}