#pragma once

#include<stdbool.h>

void blink_bitbang();
void setup_LCD_pins();
void set_data_pin_direction(int direction);
void set_data_pins(uint8_t data);
uint8_t read_data_pins();
void set_enable(int level);
void set_rw(int level);
void set_rs(int level);
void display_on_off_control(int display, int cursor, int blink);
bool lcd_busy();