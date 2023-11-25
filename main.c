#include <stdint.h>
#include "esp8266_interface.h"
#include "esp8266_mqtt_parser.h"

/* ESP8266 and MQTT connection settings 
 * Note: Modify SSID and passowrd as per your
 * wi-fi access point */
const char *ssid = "ssid";
const char *password = "password";
const char *mqtt_broker_ip = "192.168.1.3";
const char *mqtt_broker_port = "1883";

int main()
{
	/* Setup esp8266 */
	if (esp8266_wifi_setup(ssid, password, mqtt_broker_ip,
			       mqtt_broker_port) != ESP8266_WIFI_SUCCESS) {
		// TODO
	}

	/* Create and send MQTT connect message */
	if (esp8266_mqtt_msg_connect() != ESP8266_WIFI_SUCCESS) {
		// TODO
	}

	while (1) {
		esp8266_parse_mqtt_msgs();
	}
}
