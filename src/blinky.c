#include <osapi.h>
#include <ets_sys.h>
#include <user_interface.h>
#include <gpio.h>
#include <os_type.h>
#include <driver/uart.h>
#include <mem.h>
#include "driver/mp_spi.h"

#include "hw_timer.h"

static const int pin = 4;

#define REG_READ(_r) (*(volatile uint32 *)(_r))
#define WDEV_NOW() REG_READ(0x3ff20c00)
uint32 tick_now2 = 0;

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

inline
void blink_pin (const int pin) {
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << pin))
    {
      // set gpio low
      gpio_output_set(0, (1 << pin), 0, 0);
    }
    else
    {
      // set gpio high
      gpio_output_set((1 << pin), 0, 0, 0);
    }
}


void test_timerfunc()
{
    spi_tx16 (HSPI, 0x5f05);

    static uint16 j = 0;
    j++;
    if( (WDEV_NOW() - tick_now2) >= 1000000 ) {
         tick_now2 = WDEV_NOW();
         system_os_post (USER_TASK_PRIO_0, 0, j);
         j = 0;
    }

  //Do blinky stuff
    if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << pin))
    {
      // set gpio low
      gpio_output_set(0, (1 << pin), 0, 0);
    }
    else
    {
      // set gpio high
      gpio_output_set((1 << pin), 0, 0, 0);
    }

}

void some_timerfunc(void *arg)
{
    test_timerfunc();
}

#define PRINTER_QUEUE_LEN 4

os_event_t * printer_queue;

void printer_task (os_event_t * ev) {
    static u32 idx = 1;
    int j = (int) ev->par;
    os_printf("b%u:%d\n",idx++,j);
}

void task_init (void)
{
    printer_queue = (os_event_t *) os_malloc (sizeof (os_event_t)
                                                * PRINTER_QUEUE_LEN);

    system_os_task (printer_task, USER_TASK_PRIO_0,
        printer_queue, PRINTER_QUEUE_LEN);
}

void ICACHE_FLASH_ATTR user_init()
{
  //system_timer_reinit ();
  // init gpio sussytem
  gpio_init();

  spi_init (HSPI);
  spi_clock (HSPI, 5, 2);

  uart_init(BIT_RATE_115200, BIT_RATE_115200);
  os_printf("SDK version:%s\n", system_get_sdk_version());

  // Disable WiFi
  wifi_set_opmode(NULL_MODE);

  // configure UART TXD to be GPIO1, set as output
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
  //gpio_output_set(0, 0, (1 << pin), 0);
  gpio_output_set(0, 0, (1 << 4), 0);

  task_init ();

  hw_timer_init (FRC1_SOURCE, 1);
  hw_timer_set_func (test_timerfunc);
  hw_timer_arm (50);
}
