#include "light_sensor.h"
#include "clock_update.h"
#include "clock_display.h"

#include "osapi.h"
#include "user_interface.h"
#include <mqtt.h>

static os_timer_t sensor_timer;

const size_t light_level_max = 11;
size_t current_light_level=0;

uint16_t light_level_bounds[]={200,300,400,500,600,700,785,830,900,990,1023};

size_t pwm_level[]={32,32,32,32,32,22,16,8,6,4,2};

size_t map_adc_to_level (size_t adc) {
    for (int i=0; i < light_level_max; ++i) {
        if (light_level_bounds[i] >= adc) {
            return i;
        }
    }
    return light_level_max;
}


void publish_value (const char * topic, size_t value) {
    char buf[256];
    os_sprintf (buf, "%d", value);
    MQTT_Publish (&mqtt_client, topic, (char *) buf, os_strlen (buf), 0, 0);
}

static void light_sensor_tick (void * data) {

    static int cycle=0;

    size_t sensor_value = system_adc_read ();
    size_t level = map_adc_to_level(sensor_value);
    //os_printf ("light sensor value: %d\n", level);

    if (current_light_level != level) {
        current_light_level = level;

        const size_t brightness = pwm_level[level];

        //os_printf ("setting brighness: %d\n", brightness);

        clock_display_set_brigthness (brightness);

        publish_value (AMBIENT_LIGHT_TOPIC, level);
        publish_value (BRIGHTNESS_TOPIC, brightness);
    }

    if (++cycle == 60) {
        cycle = 0;
        publish_value (SENSOR_VALUE_TOPIC, sensor_value);
    }


}

void light_sensor_init () {
    os_timer_disarm  (&sensor_timer);
    os_timer_setfn   (&sensor_timer, light_sensor_tick, (void *) NULL);
    os_timer_arm     (&sensor_timer, 2000, 1);
}
