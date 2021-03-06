#ifndef __CLOCK_UPDATE__
#define __CLOCK_UPDATE__

#include <osapi.h>
#include <mqtt.h>

#include "clock_timer.h"

#ifdef __cplusplus
extern "C" {
#endif

extern MQTT_Client mqtt_client;

void ICACHE_FLASH_ATTR clock_update_init ();

#ifdef __cplusplus
}
#endif


#endif // __CLOCK_UPDATE__
