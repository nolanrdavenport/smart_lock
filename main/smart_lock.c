#include <stdio.h>

#include "HD44780.h"

void app_main(void)
{
    int number = get_number();
    for(int i = 0; i < 20; i++)
        printf("hello world number %d: %d\n", i, number);
}
