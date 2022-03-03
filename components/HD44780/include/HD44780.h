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


void blink_bitbang();

/* Helper functions */
void setup_LCD_pins();
void set_data_pin_direction(int direction);
void set_data_pins(uint8_t data);
uint8_t read_data_pins();
void set_enable(int level);
void set_rw(int level);
void set_rs(int level);
bool lcd_busy();
void execute_instruction();

/* LCD instructions */
void clear_display();
void return_home();
void entry_mode_set(int increment_decrement, int accompanies_display_shift);
void display_on_off_control(int display, int cursor, int blink);
void cursor_or_display_shift(int display_or_cursor, int right_or_left);
void function_set(int data_length, int number_of_display_lines, int font);
bool set_cgram_address(uint8_t address);
bool set_ddram_address(uint8_t address);
void write_to_ram(uint8_t data);
uint8_t read_from_ram();