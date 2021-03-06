/**
 * @file HD44780.h
 * @author Nolan Davenport (nolanrdavenport@gmail.com)
 * @brief This is a HD44780 LCD driver for the esp32
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

#pragma once

#include<stdbool.h>

#define LED_PIN 23
#define RS 21
#define RW 22
#define E 25
#define D0 12
#define D1 13
#define D2 14
#define D3 15
#define D4 16
#define D5 17
#define D6 18
#define D7 19
#define DATA_MASK (BIT12 | BIT13 | BIT14 | BIT15 | BIT16 | BIT17 | BIT18 | BIT19)
#define RS_MASK BIT21
#define RW_MASK BIT22
#define E_MASK BIT25

#define OUTPUT 1
#define INPUT 0
#define READ 1
#define WRITE 0

#define INVALID_STRING 1

/* Public API */
void blink_bitbang();
int lcd_init(int num_lines, int cursor_on_off, int cursor_blink);
void lcd_clear_display();
int lcd_print_string(char* string);
int lcd_set_cursor_location(int row, int column);