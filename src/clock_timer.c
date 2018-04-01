#include <mem.h>
#include "osapi.h"
#include <gpio.h>

#include "clock_timer.h"

#include "clock_display.h"

static int clock_display_invert=0;

void increment_second (my_clock_timer_t * clock) {
    if ( 60 == ++ (clock->tm.tm_sec) ) { // Yoda it is
        clock->tm.tm_sec = 0;
        if (60 == ++ (clock->tm.tm_min)) {
            clock->tm.tm_min = 0;
            if (24 == ++ (clock->tm.tm_hour)) {
                clock->tm.tm_hour = 0;
            }
        }
    }
}

uint16_t lcd_seg[] = {
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};

//
// uint16_t lcd_seg[] = {
//     0b00000011, // 0
//     0b10011111, // 1
//     0b00100101, // 2
//     0b00001101, // 3
//     0b10011001, // 4
//     0b01001001, // 5
//     0b01000001, // 6
//     0b00011111, // 7
//     0b00000001, // 8
//     0b00001001  // 9
// };

void  clock_timer_display (my_clock_timer_t * clock) {
    int digit=0;


    int ten = clock->tm.tm_hour / 10;
    clock->digits[digit++] = ten ? lcd_seg[clock->tm.tm_hour / 10] : 0xff;
    clock->digits[digit++] = lcd_seg[clock->tm.tm_hour % 10];
    clock->digits[digit++] = lcd_seg[clock->tm.tm_min / 10];
    clock->digits[digit++] = lcd_seg[clock->tm.tm_min % 10];

    clock->digits[clock->dot_position] &= 0xff7f;

    //clock_display_invert = GPIO_INPUT_GET(2);

    if (clock_display_invert) {
        for (int i=0; i < DISPLAY_DIGIT_MAX; ++i) {
            clock->digits[i] ^= 0xffff; //=  ~clock->digits[i];
        }
    }
}

void seconds_tick (void * _clock) {
    static int dot_=0;

    my_clock_timer_t * clock = (my_clock_timer_t *) _clock;

    if (++ (clock->dot_position) == DISPLAY_DIGIT_MAX) {
        clock->dot_position = 0;
        increment_second    (clock);
    }

    clock_timer_display (clock);
}

my_clock_timer_t *my_clock_timer = NULL;

void ICACHE_FLASH_ATTR clock_timer_init (uint8_t *digits, int digits_max)
{
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA3_U, FUNC_GPIO10);
    PIN_PULLUP_EN (PERIPHS_IO_MUX_SD_DATA3_U);

    clock_display_invert = GPIO_INPUT_GET(10);

    my_clock_timer = (my_clock_timer_t* ) os_zalloc (sizeof(my_clock_timer_t));

    my_clock_timer->digits = digits;
    my_clock_timer->digits_max = digits_max;

    clock_timer_display (my_clock_timer);

    os_timer_disarm  (&my_clock_timer->timer);
    os_timer_setfn   (&my_clock_timer->timer, seconds_tick, (void *) my_clock_timer);
    os_timer_arm     (&my_clock_timer->timer, 1000 / 4, 1);
}
