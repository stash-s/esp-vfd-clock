#ifndef __CLOCK_DISPLAY_H__
#define __CLOCK_DISPLAY_H__

#include "osapi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DISPLAY_DIGIT_MAX 4

extern uint8_t * clock_display_digit;

void ICACHE_FLASH_ATTR init_clock_display ();
void ICACHE_FLASH_ATTR clock_display_enable ();


// void clock_display_callback (void *arg);
void clock_display_callback();
void clock_display_set_brigthness (int);

#ifdef __cplusplus
}
#endif

#endif //  __CLOCK_DISPLAY_H__
