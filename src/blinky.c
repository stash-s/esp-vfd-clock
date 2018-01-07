#include <osapi.h>
#include <ets_sys.h>
#include <user_interface.h>
#include <gpio.h>
#include <os_type.h>
#include <driver/uart.h>
#include <mem.h>
#include "driver/dupa-spi.h"
//#include "driver/spi.h"

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
    //spi_mast_byte_write (SPI, 0x05);
    //spi_mast_byte_write (SPI, 0x5f);

    static uint16 j = 0;
    j++;
    if( (WDEV_NOW() - tick_now2) >= 1000000 ) {
         tick_now2 = WDEV_NOW();
         system_os_post (USER_TASK_PRIO_0, 0, j);
         //os_printf("b%u:%d\n",idx++,j);
         j = 0;
    }

  //Do blinky stuff
    //blink_pin (pin);
    //blink_pin (4);
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

    // spi_mast_byte_write(SPI, 0xff);
    // spi_mast_byte_write(SPI, 0xa5);

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

//os_task_t printer_task;

void spi_initialize()
{
    //Initialze Pins on ESP8266
//     PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_HSPIQ_MISO);
//     PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_HSPI_CS0);
//     PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_HSPID_MOSI);
//     PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_HSPI_CLK);
//
//     SpiAttr pAttr;   //Set as Master/Sub mode 0 and speed 10MHz
//     pAttr.mode = SpiMode_Master;
//     pAttr.subMode = SpiSubMode_0;
//     pAttr.speed = SpiSpeed_10MHz;
//     pAttr.bitOrder = SpiBitOrder_MSBFirst;
//     SPIInit(SpiNum_HSPI, &pAttr);
}

void ICACHE_FLASH_ATTR user_init()
//void user_init()
{
  //system_timer_reinit ();
  // init gpio sussytem
  gpio_init();

  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);//configure io to spi mode
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);//configure io to spi mode
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);//configure io to spi mode
  //PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);//configure io to spi mode

  // PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, 1);//configure io to spi mode
  // PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CMD_U, 1);//configure io to spi mode
  // PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA0_U, 1);//configure io to spi mode
  // PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA1_U, 1);//configure io to spi mode
  //
  // spi_master_init(SPI);
  // spi_set_clock_div (SPI, 8);
  // //spi_initialize();

  spi_init (HSPI);
  spi_clock (HSPI, 5, 2);
  //spi_master_init (SPI);

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
