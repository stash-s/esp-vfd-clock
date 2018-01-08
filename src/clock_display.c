
#include "osapi.h"
#include "gpio.h"
#include "mem.h"

#include "driver/mp_spi.h"

#include "hw_timer.h"

#include "clock_display.h"

uint8_t * clock_display_digit = NULL;

uint16_t lcd_mux[] = {
    0x8000,
    0x4000,
    0x2000
};

void clock_display_callback()
{
    static int m = 0;

    if (++m == DISPLAY_DIGIT_MAX) {
        m = 0;
    }

    spi_tx16 (HSPI, lcd_mux[m] | clock_display_digit[m]);
}

void ICACHE_FLASH_ATTR init_clock_display ()
{
    spi_init (HSPI);
    spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_LOW_TO_HIGH);
    spi_clock (HSPI, 5, 2);

    clock_display_digit = (uint8_t *) os_malloc (sizeof (uint8_t) * DISPLAY_DIGIT_MAX);

    hw_timer_init (FRC1_SOURCE, 1);
    hw_timer_set_func (clock_display_callback);
    hw_timer_arm (50);
}

/******************************************************************************
 * FunctionName : clock_display_enable
 * Description  : Enable shift registers by pulling 78HC595 OE' connected to
 *                D2 (GPIO4 on NodeMCU) to ground.
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR clock_display_enable () {
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    gpio_output_set(0, 0, (1 << 4), 0);
}
