#ifndef __MQTT_CONFIG_H__
#define __MQTT_CONFIG_H__

#define CFG_HOLDER	0x00FF55A4	/* Change this value to load default configurations */
#define CFG_LOCATION	0x79	/* Please don't change or if you know what you doing */
#define MQTT_SSL_ENABLE

/*DEFAULT CONFIGURATIONS*/

#define MQTT_HOST			"io.adafruit.com" //or "mqtt.yourdomain.com"
#define MQTT_PORT			1883
#define MQTT_BUF_SIZE		1024
#define MQTT_KEEPALIVE		120	 /*second*/

#define MQTT_CLIENT_ID		"DVES_%08X"
#define MQTT_USER			"stash_s"
//#define MQTT_PASS			"dupabe438e0475fa4084bcce9961372f0bd0"
#define MQTT_PASS			"be438e0475fa4084bcce9961372f0bd0"

// #define STA_SSID "DVES_HOME"
// #define STA_PASS "yourpassword"
// #define STA_TYPE AUTH_WPA2_PSK

#define MQTT_RECONNECT_TIMEOUT 	5	/*second*/

#define DEFAULT_SECURITY	0
#define QUEUE_BUFFER_SIZE		 		2048

#define PROTOCOL_NAMEv31	/*MQTT version 3.1 compatible with Mosquitto v0.15*/
//PROTOCOL_NAMEv311			/*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/

#define BRIGHTNESS_TOPIC "stash_s/feeds/brightness"
#define OUTPUT_TOPIC "stash_s/feeds/output-log"
#define AMBIENT_LIGHT_TOPIC "stash_s/feeds/ambient-light"
#define SENSOR_VALUE_TOPIC "stash_s/feeds/sensor-value"

#endif // __MQTT_CONFIG_H__
