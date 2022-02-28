#include<stdio.h>
#include<stdint.h>

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
#define DATA_MASK 0b00000000000011111111000000000000

int main(){
    printf("%d\n", DATA_MASK);

    uint32_t data = 215;
    data = data << 12;
    uint32_t fake_register = 0b00101001110010111000010101001000;

    uint32_t expected_result = 0b00101001110011010111010101001000;

    fake_register = (fake_register & ~(DATA_MASK)) | (data);

    printf("expected result: %d\n", expected_result);
    printf("result: %d\n", fake_register);

    return 0;
}