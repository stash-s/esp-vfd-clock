#include <ip_addr.h>
#include <ets_sys.h>
#include <osapi.h>
#include <espconn.h>
#include <user_interface.h>
#include <mem.h>
#include <ip_addr.h>
#include <sntp.h>
#include <time.h>
#include <mqtt.h>

#include "clock_update.h"

MQTT_Client mqtt_client;


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
            MQTT_Disconnect (&mqtt_client);
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

            MQTT_Disconnect (&mqtt_client);

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

        MQTT_Connect (&mqtt_client);

        time_t stamp = current_stamp;
        struct tm * timeinfo = localtime (&stamp);

        os_memcpy (&(my_clock_timer->tm), timeinfo, sizeof(struct tm));
        os_timer_arm(&sntp_timer, 3600000, 0);
    }
}

void ICACHE_FLASH_ATTR mqtt_init ();


void ICACHE_FLASH_ATTR clock_update_start_sntp () {

    mqtt_init ();

    int ntp_index=0;

    sntp_setservername (ntp_index++, (char *) "pool.ntp.org");

    struct ip_info info;
    if (wifi_get_ip_info(0, &info)) {
        char addr_buffer[16];
        os_sprintf (addr_buffer, IPSTR, IP2STR(&(info.gw)));
        //os_printf ("got gateway ip: %s\n", addr_buffer);
        //sntp_setserver (ntp_index++, &(info.gw));
        sntp_setservername (ntp_index++, addr_buffer);
    } else {
        os_printf ("error getting ip_info");
    }

    sntp_set_timezone (+2);
    sntp_init();

    os_timer_disarm(&sntp_timer);
    os_timer_setfn(&sntp_timer, (os_timer_func_t *)user_check_sntp_stamp, NULL);
    os_timer_arm(&sntp_timer, 1000, 0);
}

void ICACHE_FLASH_ATTR clock_update_init ()
{
    struct station_config station_conf;
    wifi_set_opmode (STATION_MODE);
    wifi_station_set_hostname ((char *)"zegar");
    os_memset (&station_conf, 0, sizeof (station_conf));

    //os_sprintf (station_conf.ssid, "open.t-mobile.pl");
    //os_sprintf (station_conf.password, "password");
    os_sprintf (station_conf.ssid, "zoo-rozciapkow");
    os_sprintf (station_conf.password, "biszkopty");

    wifi_station_set_config (&station_conf);
    wifi_set_event_handler_cb (wifi_handle_event_cb);
    wifi_station_connect ();
}


void mqtt_connected_cb (uint32_t * args) {

    os_printf ("MQTT: Connected\n");

    MQTT_Client * client = (MQTT_Client *) args;

    MQTT_Subscribe (client, BRIGHTNESS_TOPIC, 0);

    //MQTT_Publish (client, "stash_s/feeds/luminosity", "6", 1, 0, 0);
    MQTT_Publish (client, OUTPUT_TOPIC, "started", 9, 0, 0);

}

void mqtt_data_cb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
    os_printf ("MQTT: Data");

    char *topicBuf = (char*)os_zalloc(topic_len+1),
            *dataBuf = (char*)os_zalloc(data_len+1);

    MQTT_Client* client = (MQTT_Client*)args;

    os_memcpy(topicBuf, topic, topic_len);
    topicBuf[topic_len] = 0;

    os_memcpy(dataBuf, data, data_len);
    dataBuf[data_len] = 0;

    os_printf("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);

    if ( 0 == strcmp (topicBuf, BRIGHTNESS_TOPIC)) {
        int brightness = atoi (dataBuf);
        clock_display_set_brigthness (brightness);

        char buf[256];
        os_sprintf (buf, "brightness set to %d", brightness);
        MQTT_Publish (client, OUTPUT_TOPIC, (char *) buf, os_strlen (buf), 0, 0);

    }

    os_free(topicBuf);
    os_free(dataBuf);
}

void mqttDisconnectedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    os_printf("MQTT: Disconnected\r\n");
}

void mqttPublishedCb(uint32_t *args)
{
    MQTT_Client* client = (MQTT_Client*)args;
    os_printf("MQTT: Published\r\n");
}

void ICACHE_FLASH_ATTR mqtt_init () {
    os_printf ("connecting to MQTT\n");
    MQTT_InitConnection (&mqtt_client, MQTT_HOST, MQTT_PORT, 0);
    MQTT_InitClient (&mqtt_client, MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, 120, 1);
    MQTT_InitLWT(&mqtt_client, "/lwt", "offline", 0, 0);

    os_printf ("initialized all\n");

    MQTT_InitLWT(&mqtt_client, "/lwt", "offline", 0, 0);
    MQTT_OnConnected(&mqtt_client, mqtt_connected_cb);
    MQTT_OnDisconnected(&mqtt_client, mqttDisconnectedCb);
    MQTT_OnPublished(&mqtt_client, mqttPublishedCb);
    MQTT_OnData(&mqtt_client, mqtt_data_cb);

}
