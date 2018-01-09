#ifndef __CLOCK_TIMER_H__
#define __CLOCK_TIMER_H__

#include "osapi.h"
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t   *digits;
    int        digits_max;
    struct tm  tm;

    os_timer_t timer;

} my_clock_timer_t;

extern my_clock_timer_t *my_clock_timer;
void clock_timer_init (uint8_t *digits, int digits_max);

#ifdef __cplusplus
}
#endif

#endif // __CLOCK_TIMER_H__
