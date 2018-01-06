#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"


#define US_TO_RTC_TIMER_TICKS(t)          \
    ((t) ?                                   \
     (((t) > 0x35A) ?                   \
      (((t)>>2) * ((APB_CLK_FREQ>>4)/250000) + ((t)&0x3) * ((APB_CLK_FREQ>>4)/1000000))  :    \
      (((t) *(APB_CLK_FREQ>>4)) / 1000000)) :    \
     0)

#define FRC1_ENABLE_TIMER  BIT7
#define FRC1_AUTO_LOAD  BIT6

//TIMER PREDIVED MODE
typedef enum {
    DIVDED_BY_1 = 0,		//timer clock
    DIVDED_BY_16 = 4,	//divided by 16
    DIVDED_BY_256 = 8,	//divided by 256
} TIMER_PREDIVED_MODE;

typedef enum {			//timer interrupt mode
    TM_LEVEL_INT = 1,	// level interrupt
    TM_EDGE_INT   = 0,	//edge interrupt
} TIMER_INT_MODE;

typedef enum {
    FRC1_SOURCE = 0,
    NMI_SOURCE = 1,
} FRC1_TIMER_SOURCE_TYPE;


static const int pin = 1;
static volatile os_timer_t some_timer;

void test_timerfunc()
{
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

void ICACHE_FLASH_ATTR user_init()
{
  // init gpio sussytem
  gpio_init();

  // configure UART TXD to be GPIO1, set as output
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1);
  gpio_output_set(0, 0, (1 << pin), 0);

  // setup timer (500ms, repeating)
  //os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);
  //os_timer_arm(&some_timer, 1, 1);

  hw_timer_init (NMI_SOURCE, 1);
  hw_timer_set_func (test_timerfunc);
  hw_timer_arm (US_TO_RTC_TIMER_TICKS(50));
}
