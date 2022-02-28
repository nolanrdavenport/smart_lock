#include <stdio.h>
#include <stdbool.h>
#include "HD44780.h"
#include "soc/gpio_reg.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

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

void blink_bitbang(){
    setup_LCD_pins();

    //while(read_data_pins() >= 128){
    //    printf("blocking\n");
    //}

    // initial setup, put in own function later.
    set_enable(1);
    while(read_data_pins() >= 128){
        printf("waiting\n");
    }

    uint8_t data = read_data_pins();
    printf("data: %d\n", data);
    set_enable(0);
    ets_delay_us(5);

    display_clear();

    function_set(1, 1, 0);

    display_on_off_control(1, 1, 0);
    
    
    for(;;){
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(200 / portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    
    
}

// 0 for input, 1 for output. 
void set_data_pin_direction(int direction){
    if(direction){
        REG_WRITE(GPIO_ENABLE_W1TS_REG, DATA_MASK);
    }else{
        REG_WRITE(GPIO_ENABLE_W1TC_REG, DATA_MASK);
    }
}

void set_data_pins(uint8_t data){
    uint32_t data_shifted = data << D0; // shift the data to begin at D7 which is BIT12

    uint32_t output = (REG_READ(GPIO_OUT_REG) & ~(DATA_MASK)) | (data_shifted);

    REG_WRITE(GPIO_OUT_REG, output);
}

uint8_t read_data_pins(){
    uint8_t input = (REG_READ(GPIO_IN_REG) & DATA_MASK) >> D0;

    return input;
}

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

// 1 for enabled, 0 for not enabled. 
void set_enable(int level){
    gpio_set_level(E, level);
}

// 0 for write, 1 for read. 
void set_rw(int level){
    gpio_set_level(RW, level);
}

void set_rs(int level){
    gpio_set_level(RS, level);
}

void display_on_off_control(int display, int cursor, int blink){
    uint8_t instruction = 0b00001000;

    if(display) instruction |= 0b00000100;
    if(cursor) instruction |= 0b00000010;
    if(blink) instruction |= 0b00000001;

    set_rs(0);
    set_rw(WRITE);

    set_data_pin_direction(OUTPUT);

    set_data_pins(instruction);

    set_enable(1);
    ets_delay_us(10); // wait for 5 us for now. 
    set_enable(0);

    finish_instruction();
    // done? 
}

void display_clear(){
    set_rs(0);
    set_rw(WRITE);
    set_data_pin_direction(OUTPUT);
    set_data_pins(0b00000001);

    set_enable(1);
    ets_delay_us(10); // wait for 5 us for now. 
    set_enable(0);

    finish_instruction();
}

void function_set(int data_length, int number_of_display_lines, int font){
    // TODO: test this function. Untested. 

    uint8_t instruction = 0b00100000;

    if(data_length) instruction |= 0b00010000;
    if(number_of_display_lines) instruction |= 0b00001000;
    if(font) instruction |= 0b00000100;

    set_rs(0);
    set_rw(WRITE);
    set_data_pin_direction(OUTPUT);

    set_data_pins(instruction);

    set_enable(1);
    ets_delay_us(10); // wait for 5 us for now. 
    set_enable(0);

    finish_instruction();

}

void finish_instruction(){
    set_rs(0);
    set_rw(READ);
    set_data_pin_direction(INPUT);

    while(lcd_busy()){
        ets_delay_us(5);
    }
}

// you need to set rw outside of this function
bool lcd_busy(){
    set_enable(1);
    ets_delay_us(5);
    uint8_t input = read_data_pins();
    set_enable(0);
    if(input >= 128){
        return true;
    }
    return false;
}
