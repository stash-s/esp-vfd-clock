
#include "clock_timer.h"



void increment_second (clock_t * clock) {
    if ( 60 == ++ (clock->seconds) ) { // Yoda it is
        clock->seconds = 0;
        if (60 == ++ (clock->minutes)) {
            clock->minutes = 0;
            if (24 == ++ (clock->hours)) {
                clock->hours = 0;
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


void  clock_timer_display (clock_t * clock) {
    clock->digits[0] = lcd_seg[clock->minutes % 10];
    clock->digits[1] = lcd_seg[clock->seconds / 10];
    clock->digits[2] = lcd_seg[clock->seconds % 10];
}

void seconds_tick (void * _clock) {
    clock_t * clock = (clock_t *) _clock;

    increment_second (clock);
    clock_timer_display (clock);
}

void ICACHE_FLASH_ATTR clock_timer_init (clock_t * clock, uint8_t *digits, int digits_max)
{
    os_memset (clock, 0, sizeof(clock_t));

    clock->digits = digits;
    clock->digits_max = digits_max;

    clock_timer_display (clock);

    os_timer_disarm  (&clock->timer);
    os_timer_setfn   (&clock->timer, seconds_tick, (void *) clock);
    os_timer_arm     (&clock->timer, 1000, 1);
}
