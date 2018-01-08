#ifndef __CLOCK_TIMER_H__
#define __CLOCK_TIMER_H__

#include "osapi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t   *digits;
    int        digits_max;
    os_timer_t timer;

    int seconds;
    int minutes;
    int hours;

} clock_t;

void clock_timer_init (clock_t * clock, uint8_t *digits, int digits_max);

#ifdef __cplusplus
}
#endif

#endif // __CLOCK_TIMER_H__
