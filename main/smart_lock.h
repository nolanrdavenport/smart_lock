/**
 * @file smart_lock.h
 * @author Nolan Davenport (nolanrdavenport@gmail.com)
 * @brief The main file for the smart lock
 * @version 0.1
 * @date 2022-05-28
 * 
 * @copyright Copyright (c) 2022
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

#pragma once

#define DUTY_CYCLE_OPEN_STATE 9.5
#define DUTY_CYCLE_CLOSED_STATE 2.5

typedef enum{
    OPEN,
    CLOSED
} lock_state_t;