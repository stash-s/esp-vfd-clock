#include <mem.h>

#include "clock_timer.h"


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
    0b00000011, // 0
    0b10011111, // 1
    0b00100101, // 2
    0b00001101, // 3
    0b10011001, // 4
    0b01001001, // 5
    0b01000001, // 6
    0b00011111, // 7
    0b00000001, // 8
    0b00001001  // 9
};


void  clock_timer_display (my_clock_timer_t * clock) {
    clock->digits[0] = lcd_seg[clock->tm.tm_hour % 10];
    clock->digits[1] = lcd_seg[clock->tm.tm_min / 10];
    clock->digits[2] = lcd_seg[clock->tm.tm_min % 10];
}

void seconds_tick (void * _clock) {
    my_clock_timer_t * clock = (my_clock_timer_t *) _clock;

    increment_second    (clock);
    clock_timer_display (clock);
}

my_clock_timer_t *my_clock_timer = NULL;

void ICACHE_FLASH_ATTR clock_timer_init (uint8_t *digits, int digits_max)
{
    my_clock_timer = (my_clock_timer_t* ) os_zalloc (sizeof(my_clock_timer_t));

    my_clock_timer->digits = digits;
    my_clock_timer->digits_max = digits_max;

    clock_timer_display (my_clock_timer);

    os_timer_disarm  (&my_clock_timer->timer);
    os_timer_setfn   (&my_clock_timer->timer, seconds_tick, (void *) my_clock_timer);
    os_timer_arm     (&my_clock_timer->timer, 1000, 1);
}
