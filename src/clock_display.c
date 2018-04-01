
#include "osapi.h"
#include "gpio.h"
#include "mem.h"

#include "driver/mp_spi.h"

#include "hw_timer.h"

#include "clock_display.h"
#include "user_config.h"


uint8_t * clock_display_digit = NULL;
int clock_display_filament_divider=200;

uint32_t lcd_mux[] = {
     0x2000,
     0x1000,
     0x4000,
     0x8000
 };

// uint32_t lcd_mux[] = {
//     0x8000,
//     0x4000,
//     0x1000,
//     0x2000,
// };


#define CLOCK_DISPLAY_PWM_MAX 32
int clock_display_brightness = 30;


void clock_display_callback()
{
    static int filament_clock=0;
    static int filament_stat=0;

    static int pwm_slot=0;
    static int m = 0;

    if (++filament_clock > clock_display_filament_divider) {
        filament_clock = 0;

        if (filament_stat) {
            gpio_output_set(0, (1 << FILAMENT_PIN), (1 << FILAMENT_PIN), 0);

            filament_stat = FALSE;
        } else {
            gpio_output_set((1 << FILAMENT_PIN), 0, (1 << FILAMENT_PIN), 0);

            filament_stat = TRUE;
        }
    }

    if (pwm_slot > CLOCK_DISPLAY_PWM_MAX) {
        // reset counter, move to next digit
        if (++m == DISPLAY_DIGIT_MAX) {
            m = 0;
        }
        pwm_slot = 0;
    }

    if (pwm_slot == 0) {
        spi_txd (HSPI, 20, lcd_mux[m] | clock_display_digit[m]);
    }

    if (pwm_slot >= clock_display_brightness) {
        spi_txd (HSPI, 20, 0x0000);
    }

    ++pwm_slot;
}

void ICACHE_FLASH_ATTR init_clock_display ()
{
    spi_init (HSPI);
    spi_tx_byte_order(HSPI, SPI_BYTE_ORDER_LOW_TO_HIGH);
    spi_clock (HSPI, 10, 2);

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
    //PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
    //gpio_output_set(0, (1 << 4), (1 << 4), 0);
    //PIN_PULLUP_EN (PERIPHS_IO_MUX_GPIO0_U);

    // pull blank line of driver down to enable outputs
    GPIO_OUTPUT_SET(4, FALSE);
}

void clock_display_set_brigthness (int brightness) {
    clock_display_brightness = brightness;
}
