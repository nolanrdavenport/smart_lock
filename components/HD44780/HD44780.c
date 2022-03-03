/**
 * @file HD44780.c
 * @author Nolan Davenport (nolandavenport.tech)
 * @brief This is a HD44780 LCD driver for the esp32
 * @version 0.1
 * @date 2022-03-03
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdbool.h>
#include "HD44780.h"
#include "soc/gpio_reg.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

void blink_bitbang(){
    setup_LCD_pins();

    //while(read_data_pins() >= 128){
    //    printf("blocking\n");
    //}

    // initial setup, put in own function later.
    set_enable(1);
    ets_delay_us(5);
    while(read_data_pins() >= 128){
        printf("waiting\n");
    }

    uint8_t data = read_data_pins();
    printf("data: %d\n", data);
    set_enable(0);
    ets_delay_us(5);

    clear_display();

    function_set(1, 1, 0);

    display_on_off_control(1, 1, 0);
    
    entry_mode_set(1, 0);

    for(uint8_t c = 'a'; c <= 'p'; c++){
        write_to_ram(c);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }

    for(;;){
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);        
    }
    
    
}

/**
 * @brief   Sets up all of the pins and sets a few defaults. The defaults are as follows:
 *          RW = READ, E = 0, RS = 0. These are set as output pins. D0-D7 are set as input pins. 
 * 
 */
void setup_LCD_pins(){
    gpio_reset_pin(LED_PIN);
    gpio_reset_pin(RS);
    gpio_reset_pin(RW);
    gpio_reset_pin(E);

    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(RS, GPIO_MODE_OUTPUT);
    gpio_set_direction(RW, GPIO_MODE_OUTPUT);
    gpio_set_direction(E, GPIO_MODE_OUTPUT);

    // defaults
    set_rw(READ);
    set_enable(0);
    set_rs(0);

    // this works only if D0-D7 are contiguous
    for(int i = D0; i <= D7; i++){ // start with D7 because D7 is the higher order bit. 
        gpio_reset_pin(i);
        gpio_set_direction(i, GPIO_MODE_INPUT);
    }
}


/**
 * @brief Sets the direction of the data pins.
 * 
 * @param direction 0 for input and 1 for output. 
 */
void set_data_pin_direction(int direction){
    if(direction){
        REG_WRITE(GPIO_ENABLE_W1TS_REG, DATA_MASK);
    }else{
        REG_WRITE(GPIO_ENABLE_W1TC_REG, DATA_MASK);
    }
}

/**
 * @brief Sets the data gpio pins according to the parameter. 
 * 
 * @param data The 8 data pins will be set to the value of this byte of data. 
 */
void set_data_pins(uint8_t data){
    uint32_t data_shifted = data << D0; // shift the data to begin at D7 which is BIT12

    uint32_t output = (REG_READ(GPIO_OUT_REG) & ~(DATA_MASK)) | (data_shifted);

    REG_WRITE(GPIO_OUT_REG, output);
}

/**
 * @brief Reads the data pins and returns it in a byte. 
 * 
 * @return uint8_t The data on the data gpio pins. 
 */
uint8_t read_data_pins(){
    uint8_t input = (REG_READ(GPIO_IN_REG) & DATA_MASK) >> D0;

    return input;
}

// 1 for enabled, 0 for not enabled. 

/**
 * @brief Sets the enable pin. 
 * 
 * @param level 1 for enabled and 0 for not enabled. 
 */
void set_enable(int level){
    gpio_set_level(E, level);
}

/**
 * @brief Sets the Read/Write pin. 
 * 
 * @param level 0 for write and 1 for read. 
 */
void set_rw(int level){
    gpio_set_level(RW, level);
}

/**
 * @brief Sets the register select pin.
 * 
 * @param level 
 */
void set_rs(int level){
    gpio_set_level(RS, level);
}

/**
 * @brief Checks whether the LCD is busy with an internal operation. 
 * 
 * @return true if the LCD is currently busy (busy flag = 1).
 * @return false if the LCD is not busy (busy flag = 0).
 */
bool lcd_busy(){
    // Read the data pins.

    // You can read the data pins when RW=1 and E=1. 
    set_enable(1);
    ets_delay_us(1);
    uint8_t input = read_data_pins();
    set_enable(0);

    // The most significant bit should be the busy flag. So, any value over 127 would mean that BF = 1.
    if(input >= 128){
        return true;
    }else{
        return false;
    }
}

/**
 * @brief   After an instruction is setup by setting the data lines and other accompanying control pins, this
 *          function will execute the instruction on the LCD. On completion of this function, the LCD will be
 *          finished with any internal operations and another instruction can be executed. 
 * 
 */
void execute_instruction(){
    // Turn on the enable pin for 10us. 
    set_enable(1);
    ets_delay_us(1);
    set_enable(0);

    // After the enable pin falls, the execution in the LCD should have started. Keep checking BF (busy flag) until it isn't busy any longer. 
    set_rs(0);
    set_rw(READ);
    set_data_pin_direction(INPUT);

    while(lcd_busy()){
        ets_delay_us(5);
    }
}

/**
 * @brief Clears the display and sets the DDRAM address to 0. Unshifts the display. Sets display to increment mode. 
 * 
 */
void clear_display(){
    set_rs(0);
    set_rw(WRITE);
    set_data_pin_direction(OUTPUT);
    set_data_pins(0b00000001);

    execute_instruction();
}

/**
 * @brief Sets DDRAM address to 0 and unshifts display. 
 * 
 */
void return_home(){
    set_rs(0);
    set_rw(WRITE);
    set_data_pin_direction(OUTPUT);
    set_data_pins(0b00000010);

    execute_instruction();
}

/**
 * @brief   Sets the display to increment or decrement DDRAM address after each character code is written/read, and sets
 *          whether the display will shift on a write. 
 * 
 * @param increment_decrement 1 for increment and 0 for decrement. 
 * @param accompanies_display_shift If set to 1, the cursor will seemingly not move, and the display will instead shfit.
 *                                  If increment_decrement is set to 0, the display will shift to the right. 
 *                                  If increment_decrement is set to 1, the display will shift to the left. 
 */
void entry_mode_set(int increment_decrement, int accompanies_display_shift){
    // TODO: test this function.

    // Base instruction
    uint8_t instruction = 0b00000100;

    // Edit base instruction based on the flags. 
    if(increment_decrement) instruction |= 0b00000010;
    if(accompanies_display_shift) instruction |= 0b00000001;

    // Set the RS and RW pins. 
    set_rs(0);
    set_rw(WRITE);

    // The data pins are sending data to the LCD. 
    set_data_pin_direction(OUTPUT);

    // Set the pins to the value of the instruction. 
    set_data_pins(instruction);

    execute_instruction();
}

/**
 * @brief Performs the display on/off control instruction. 
 * 
 * @param display 1 for display on and 0 for display off. 
 * @param cursor 1 to display the cursor and 0 to not display the cursor. 
 * @param blink 1 to blink the cursor and 0 to not blink the cursor. 
 */
void display_on_off_control(int display, int cursor, int blink){
    uint8_t instruction = 0b00001000;

    if(display) instruction |= 0b00000100;
    if(cursor) instruction |= 0b00000010;
    if(blink) instruction |= 0b00000001;

    set_rs(0);
    set_rw(WRITE);

    set_data_pin_direction(OUTPUT);

    set_data_pins(instruction);

    execute_instruction();
    // done? 
}

/**
 * @brief Moves cursor and shifts display without changing DDRAM contents. 
 * 
 * @param display_or_cursor 1 to shift display and 0 to move cursor. 
 * @param right_or_left 1 to shift/move to the right and 0 to shift/move to the left. 
 */
void cursor_or_display_shift(int display_or_cursor, int right_or_left){
    // TODO: test this function.

    // Base instruction
    uint8_t instruction = 0b00010000;

    // Edit base instruction based on the flags. 
    if(display_or_cursor) instruction |= 0b00001000;
    if(right_or_left) instruction |= 0b00000100;

    // Set the RS and RW pins. 
    set_rs(0);
    set_rw(WRITE);

    // The data pins are sending data to the LCD. 
    set_data_pin_direction(OUTPUT);

    // Set the pins to the value of the instruction. 
    set_data_pins(instruction);

    execute_instruction();
}

/**
 * @brief Sets the interface data length, number of display lines, and sets the character font. 
 * 
 * @param data_length 1 for 8 bit data length and 0 for 4 bit data length. 
 * @param number_of_display_lines 1 for 2 lines and 0 for 1 line. 
 * @param font 1 for 5x10 dots and 0 for 5x8 dots. 
 */
void function_set(int data_length, int number_of_display_lines, int font){
    // TODO: fully test this function. It is only partially tested. 

    // Base instruction
    uint8_t instruction = 0b00100000;

    // Edit base instruction based on the flags. 
    if(data_length) instruction |= 0b00010000;
    if(number_of_display_lines) instruction |= 0b00001000;
    if(font) instruction |= 0b00000100;

    // Set the RS and RW pins. 
    set_rs(0);
    set_rw(WRITE);

    // The data pins are sending data to the LCD. 
    set_data_pin_direction(OUTPUT);

    // Set the pins to the value of the instruction. 
    set_data_pins(instruction);

    execute_instruction();

}

/**
 * @brief Sets the CGRAM address. The data should then be read/written after this command. 
 * 
 * @param address The CGRAM address.
 * @return true If the address was valid. 
 * @return false If the address was not valid. 
 */
bool set_cgram_address(uint8_t address){
    // TODO: implement
    return true;
}

/**
 * @brief Sets the DDRAM address. The data should then be read/written after this command. 
 * 
 * @param address The DDRAM address.
 * @return true If the address was valid. 
 * @return false If the address was not valid. 
 */
bool set_ddram_address(uint8_t address){
    // TODO: implement

    return true;
}

/**
 * @brief Write to either the CG or DDRAM depending on a previous CG or DDRAM address write. 
 * 
 * @param data The data that is to be written to the ram. 
 */
void write_to_ram(uint8_t data){
    set_rs(1);
    set_rw(0);
    
    set_data_pin_direction(OUTPUT);
    set_data_pins(data);

    execute_instruction();
}

/**
 * @brief Read from either the CG or DDRAM dependong on a previous CG or DDRAM address write. 
 * 
 * @return uint8_t The data located at the current address. 
 */
uint8_t read_from_ram(){
    // TODO: implement

    return 0;
}
