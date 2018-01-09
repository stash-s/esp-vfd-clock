#include <ip_addr.h>
#include <ets_sys.h>
#include <osapi.h>
#include <espconn.h>
#include <user_interface.h>
#include <mem.h>
#include <ip_addr.h>
#include <sntp.h>
#include <time.h>

#include "clock_update.h"

void ICACHE_FLASH_ATTR clock_update_start_sntp ();

void wifi_handle_event_cb (System_Event_t *evt)
{
    os_printf ("event %x\n", evt->event);

    switch (evt->event) {

        case EVENT_STAMODE_CONNECTED:
            os_printf("connect to ssid %s, channel %d\n",
                evt->event_info.connected.ssid,
                evt->event_info.connected.channel);
            break;

        case EVENT_STAMODE_DISCONNECTED:
            os_printf("disconnect from ssid %s, reason %d\n",
                evt->event_info.disconnected.ssid,
                evt->event_info.disconnected.reason);
            break;

        case EVENT_STAMODE_AUTHMODE_CHANGE:
            os_printf("mode: %d -> %d\n",
                evt->event_info.auth_change.old_mode,
                evt->event_info.auth_change.new_mode);
            break;

        case EVENT_STAMODE_GOT_IP:
            os_printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
                IP2STR(&evt->event_info.got_ip.ip),
                IP2STR(&evt->event_info.got_ip.mask),
                IP2STR(&evt->event_info.got_ip.gw));
            os_printf("\n");
            clock_update_start_sntp ();
            break;

        case EVENT_SOFTAPMODE_STACONNECTED:
            os_printf("station: " MACSTR "join, AID = %d\n",
                MAC2STR(evt->event_info.sta_connected.mac),
                evt->event_info.sta_connected.aid);
            break;

        case EVENT_SOFTAPMODE_STADISCONNECTED:
            os_printf("station: " MACSTR "leave, AID = %d\n",
                MAC2STR(evt->event_info.sta_disconnected.mac),
                evt->event_info.sta_disconnected.aid);
            break;

        default:
            break;
    }
}

static os_timer_t sntp_timer;

void ICACHE_FLASH_ATTR user_check_sntp_stamp(void *arg)
{
    uint32 current_stamp;
    current_stamp = sntp_get_current_timestamp();
    if(current_stamp == 0){
        os_timer_arm(&sntp_timer, 1000, 0);
    } else {
        os_timer_disarm(&sntp_timer);
        os_printf("sntp: %d, %s \n",current_stamp,
                sntp_get_real_time(current_stamp));

        time_t stamp = current_stamp;
        struct tm * timeinfo = localtime (&stamp);

        os_memcpy (&(my_clock_timer->tm), timeinfo, sizeof(struct tm));

        os_timer_arm(&sntp_timer, 3600000, 0);
    }
}

void ICACHE_FLASH_ATTR clock_update_start_sntp () {
    sntp_setservername (0, (char *) "91.233.70.230");
    sntp_set_timezone (+1);
    sntp_init();

    os_timer_disarm(&sntp_timer);
    os_timer_setfn(&sntp_timer, (os_timer_func_t *)user_check_sntp_stamp, NULL);
    os_timer_arm(&sntp_timer, 1000, 0);
}

void ICACHE_FLASH_ATTR clock_update_init ()
{
    struct station_config station_conf;
    wifi_set_opmode (STATION_MODE);
    //wifi_station_set_hostname ((char *)"zegar");
    //os_memset (&station_conf, 0, sizeof (station_conf));

    //os_sprintf (station_conf.ssid, "ssid");
    //os_sprintf (station_conf.password, "password");

    //wifi_station_set_config (&station_conf);
    wifi_set_event_handler_cb (wifi_handle_event_cb);
    wifi_station_connect ();
}
