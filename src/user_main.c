#include <osapi.h>
#include <ets_sys.h>
#include <user_interface.h>
#include <gpio.h>
#include <os_type.h>
#include <driver/uart.h>
#include <mem.h>

#include "clock_display.h"
#include "clock_timer.h"


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

void ICACHE_FLASH_ATTR user_init()
{
  //system_timer_reinit ();
  // init gpio sussytem
  gpio_init();

  uart_init(BIT_RATE_115200, BIT_RATE_115200);
  os_printf("SDK version:%s\n", system_get_sdk_version());

  init_clock_display ();
  static clock_t clock;
  clock_timer_init (&clock, clock_display_digit, DISPLAY_DIGIT_MAX);

  // Disable WiFi
  //wifi_set_opmode(NULL_MODE);

  // configure UART TXD to be GPIO1, set as output
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
  //gpio_output_set(0, 0, (1 << pin), 0);
  gpio_output_set(0, 0, (1 << 4), 0);
}
