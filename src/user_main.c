#include <osapi.h>
#include <ets_sys.h>
#include <user_interface.h>
#include <gpio.h>
#include <os_type.h>
#include <driver/uart.h>
#include <mem.h>

#include "clock_display.h"
#include "clock_timer.h"
#include "clock_update.h"
#include "light_sensor.h"

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;

        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;

         default:
            rf_cal_sec = 0;
            break;
    }
    return rf_cal_sec;
}

// inline
// void blink_pin (const int pin) {
//     if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << pin))
//     {
//       // set gpio low
//       gpio_output_set(0, (1 << pin), 0, 0);
//     }
//     else
//     {
//       // set gpio high
//       gpio_output_set((1 << pin), 0, 0, 0);
//     }
// }

void sys_init_cb () {
    os_printf("system init callback called\n");


    init_clock_display ();

    clock_timer_init (clock_display_digit, DISPLAY_DIGIT_MAX);

    clock_display_enable ();

    clock_update_init ();

    light_sensor_init ();
}


void ICACHE_FLASH_ATTR user_init()
{
  //system_timer_reinit ();
  // init gpio sussytem
  gpio_init();

  uart_init(BIT_RATE_115200, BIT_RATE_115200);
  os_printf("SDK version:%s\n", system_get_sdk_version());

  system_init_done_cb (sys_init_cb);
}
