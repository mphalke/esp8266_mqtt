# esp8266_mqtt
ESP8266 Wi-Fi MQTT Application

# Introduction
This repository contains the source code for configuring [ESP8266 Wi-Fi](https://www.sparkfun.com/products/17146) module,
establishing MQTT broker connection and then communicating data over MQTT
subscribe and publish messaging model.

[ESP8266 Wi-Fi SOC](https://www.espressif.com/en/products/socs/esp8266) uses the UART 
bridge to communicate with Microcontrollers. The ESP8266 SOC communicates 
using [AT Commands](https://www.espressif.com/sites/default/files/4a-esp8266_at_instruction_set_en_v1.5.4_0.pdf), 
which are sent over UART peripheral from Microcontroller. For this tutorial,
we are using [Mbed-OS](https://os.mbed.com/mbed-os/) and any [Mbed compatible MCU board](https://os.mbed.com/platforms/) 
to interface with ESP8266 Wi-Fi module. However, the source code can easily be
modified to support any other MCU platform of your choice by updating platform.cpp
file. Rest of the source files are platform agnostic.

For MQTT services, we are using Eclipse Paho MQTT library. The source code is [here](https://github.com/eclipse/paho.mqtt.embedded-c/tree/29ab2aa29c5e47794284376d7f8386cfd54c3eed/MQTTPacket/src)

Mosquitto MQTT broker is used to route the publish and subscribe messages between
MQTT clients. Node-Red is used to display messages onto PC side.

# Build Support

Clone the repository with the --recursive flag (not needed if building with web IDE for Mbed platform):

> git clone --recursive https://github.com/mphalke/esp8266_mqtt

If however you've already cloned the repository without the --recursive flag, you 
may initialize all the submodules in an existing cloned repo with:

> git submodule update --recursive --init

Use Mbed CLI to build the project. Build guide is [here](https://os.mbed.com/docs/mbed-os/v6.16/build-tools/compile.html)

**Note**
We are not going into details of building the project here. Mbed has extensive
documentation on build pre-requisites, building tools, compiler configurations,
compiling project for selected target board, etc. Suggestion for users is to
go through mbed documentation for more details. Also users have a choice to select
platform and software toolchain of there own to build the project with certain
platform specific modifications in the software.

# Hardware Interface
![Hardware Interface](https://github.com/mphalke/esp8266_mqtt/blob/main/resources/hw_interface.png)

![ESP8266 and MCU Pin Mapping](https://github.com/mphalke/esp8266_mqtt/blob/main/resources/esp8266_pin_map.PNG)

# Software APIs

#### Configuring ESP8266 Wi-Fi Module

1. Reset Wi-Fi module by sending "AT+RST\r\n" UART command. Verify if Wi-Fi module
responses/acknowledges with string "OK".
2. Test Wi-Fi module connection by sending "AT\r\n" UART command. Verify if Wi-Fi module
responses/acknowledges with string "OK".
3. Set Wi-Fi mode by sending "AT+CWMODE=MODE\r\n" command, where MODE can be:

    a) 1: Station Mode
    b) 2: SoftAP Mode
    c) 3: Station + SoftAP Mode

    Verify if Wi-Fi module responses/acknowledges with string "OK".

4. Join to Wi-Fi access point by sending "AT+CWJAP=ssid,password\r\n" command.
Verify if Wi-Fi module responses/acknowledges with string "OK".
5. Set Wi-Fi single connection mode by sending command "AT+CIPMUX=0\r\n" command.
Verify if Wi-Fi module responses/acknowledges with string "OK".
6. Connect to Wi-Fi remote server (MQTT broker) by sending "AT+CIPSTART=conn_type,
ip_addr, port\r\n" command, where conn_type is TCP.
Verify if Wi-Fi module responses/acknowledges with string "OK".

```
/* ESP8266 and MQTT connection settings 
 * Note: Modify SSID and password as per your
 * wi-fi access point */
const char *ssid = "ssid";
const char *password = "password";
const char *mqtt_broker_ip = "192.168.1.6";
const char *mqtt_broker_port = "1883";

int main()
{
	/* Setup esp8266 */
	if (esp8266_wifi_setup(ssid, password, mqtt_broker_ip,
			       mqtt_broker_port) != ESP8266_WIFI_SUCCESS) {
		// Handle error code
	}
}

/*
 *@brief ESP8266 WiFi setup function
 *@return Wifi setup status (success/failure)
 **/
esp8266_wifi_status esp8266_wifi_setup(const char *ssid, const char *password,
				       const char *mqtt_broker_ip, const char *mqtt_broker_port)
{
	/* Register UART callback to receive client events */
	esp8266_uart_rx_callback_register(esp8266_uart_callback);

	printf("%s", "Resetting esp8266\r\n");
	if (esp8266_wifi_reset() == ESP8266_WIFI_SUCCESS) {
		printf("%s", "esp8266 reset success...\r\n");
	} else {
		printf("%s", "esp8266 reset failure!!\r\n");
		return ESP8266_WIFI_FAILURE;
	}

	printf("%s", "\r\nTesting esp8266 AT command\r\n");
	if (esp8266_wifi_at_test() == ESP8266_WIFI_SUCCESS) {
		printf("%s", "esp8266 AT test success...\r\n");
	} else {
		printf("%s", "esp8266 AT test failure!!\r\n");
		return ESP8266_WIFI_FAILURE;
	}

	printf("%s", "\r\nSetting esp8266 wifi mode\r\n");
	if (esp8266_wifi_mode_set(ESP8266_WIFI_STATION_MODE) == ESP8266_WIFI_SUCCESS) {
		printf("%s", "esp8266 wifi mode set success...\r\n");
	} else {
		printf("%s", "esp8266 wifi mode set failure!!\r\n");
		return ESP8266_WIFI_FAILURE;
	}

	printf("%s", "\r\nSetting esp8266 wifi AP\r\n");
	if (esp8266_wifi_set_ap(ssid, password) == ESP8266_WIFI_SUCCESS) {
		printf("%s", "esp8266 wifi AP set success...\r\n");
	} else {
		printf("%s", "esp8266 wifi AP set failure!!\r\n");
		return ESP8266_WIFI_FAILURE;
	}

	printf("%s", "\r\nSetting esp8266 single mode\r\n");
	if (esp8266_wifi_single_conn() == ESP8266_WIFI_SUCCESS) {
		printf("%s", "esp8266 wifi single mode set success...\r\n");
	} else {
		printf("%s", "esp8266 wifi single mode failure!!\r\n");
		return ESP8266_WIFI_FAILURE;
	}

	printf("%s", "\r\nConnecting to esp8266 wifi remote server\r\n");
	if (esp_8266_wifi_connect_server("TCP", mqtt_broker_ip,
					 mqtt_broker_port) == ESP8266_WIFI_SUCCESS) {
		printf("%s", "esp8266 wifi server connect success...\r\n");
	} else {
		printf("%s", "esp8266 wifi server connect failure!!\r\n");
		return ESP8266_WIFI_FAILURE;
	}

	printf("esp8266 wifi connect success...\r\n");
	return ESP8266_WIFI_SUCCESS;
}
```

#### Establishing MQTT Broker Connection from esp8266_mqtt application

1. Form the MQTT connection data.
2. Send MQTT connection data using "AT+CIPSEND=data_len\r\n" command.

##### What is MQTT and MQTT Broker?

MQTT stands for Message Query Telemetry Transfer. It is a protocol created to
transfer data when multiple IOT devices are connected together. MQTT uses central
server called MQTT broker to route the messages to appopriate IOT device using
publish and subscibe policy. IOT devices are published and subscribed to 'Topics'.
Each device can subscribe and publish to multiple topics and MQTT broker takes
care of message routing to each device. Topics can be temperature, voltage, logger,
database, etc.

MQTT broker is a software that runs on either PC/mobile, target SOC or cloud.
It can be open-source or propriatory.

##### Advantages of MQTT Broker:

1. Easily scalable from 1 device to many devices
2. Manage client connect states
3. Eliminates insecure connections
4. Reduce network strain

Example of Complex IOT system is shown below. MQTT broker plays an essential role
while transfering messages between multiple IOT devices in such a complex system.

![Complex IOT System](https://github.com/mphalke/esp8266_mqtt/blob/main/resources/complex_iot_system.PNG)

MQTT messaging model can be "One to Many" Or "Many to One".

##### One To Many:

One device publishing on topic. MQTT broker routes same message to multiple
devices which have subscribed to that topic.

![One to Many](https://github.com/mphalke/esp8266_mqtt/blob/main/resources/one_to_many_pub_sub_model.PNG)

##### Many to One:

Many devices publishing on same topic. MQTT broker routes each message sequentially
to one device which has subscribed to that topic.

![Many to One](https://github.com/mphalke/esp8266_mqtt/blob/main/resources/many_to_one_pub_sub_model.PNG)

The MQTTPacket_connectData structure is defined in MQTTConnect.h file. Application
code must define the required connection data parameters as shown below:

```
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
}

/* MQTT connection configurations (MQTTConnect.h) */
typedef struct
{
	/** The eyecatcher for this structure.  must be MQTC. */
	char struct_id[4];
	/** The version number of this structure.  Must be 0 */
	int struct_version;
	/** Version of MQTT to be used.  3 = 3.1 4 = 3.1.1
	  */
	unsigned char MQTTVersion;
	MQTTString clientID;
	unsigned short keepAliveInterval;
	unsigned char cleansession;
	unsigned char willFlag;
	MQTTPacket_willOptions will;
	MQTTString username;
	MQTTString password;
} MQTTPacket_connectData;

/* MQTT application connect configurations (esp8266_mqtt_parser.c) */
typedef struct {
	uint8_t link_id;
	uint8_t version;
	uint8_t *name;
	uint16_t keep_alive;
	uint8_t *username;
	uint8_t *password;
} esp8266_wifi_mqtt_connect_config;

/* Define application connect parameters (esp8266_mqtt_parser.c) */
esp8266_wifi_mqtt_connect_config mqtt_connect_confg = {
	.link_id = 1,
	.version = 4,
	.name = (uint8_t *)"esp8266",
	.keep_alive = 1000,
	.username = (uint8_t *)"",
	.password = (uint8_t *)"",
};

static char mqtt_msg_buffer[500];

/* Create and send MQTT connect message */
esp8266_wifi_status esp8266_mqtt_msg_connect(void)
{
	int data_len;
	esp8266_wifi_status status;

    /* Define the connection data variable and assign default values */
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    /* Define rest of the connect parameters */
	data.MQTTVersion = mqtt_connect_confg.version;
	data.clientID.cstring = (char *)mqtt_connect_confg.name;
	data.keepAliveInterval = mqtt_connect_confg.keep_alive;
	data.cleansession = 1u;
	data.username.cstring = (char *)mqtt_connect_confg.username;
	data.password.cstring = (char *)mqtt_connect_confg.password;

	/* Serialize the mqtt connection packet */
	data_len = MQTTSerialize_connect((unsigned char *)mqtt_msg_buffer,
					 sizeof(mqtt_msg_buffer), &data);

    /* Send connection packet over Wi-Fi using "AT+CIPSEND=data_len\r\n" command */
	status = esp8266_send_connect_msg(mqtt_msg_buffer, data_len);
	if (status != ESP8266_WIFI_SUCCESS) {
		return status;
	}

	return ESP8266_WIFI_SUCCESS;
}
```

#### Start MQTT broker server on computer
For this tutorial, we are going to use Eclipse Mosquitto MQTT broker. It can be
downloaded from [here](https://mosquitto.org/download/).

To run mosquitto broker service, go to the directory where it is installed and first
modify the mosquitto.config file with below changes:

> allow_anonymous true

> listener 1883

Now type "mosquitto.exe -v -c mosquitto.config" in command prompt window to start broker in verbose mode, 
enabling debug messages. Keep command window open to continue running broker services.

**Note**
In case you get 1883 port opening error, check if that port is open and if yes, kill
it using task manager. Some help [here](https://stackoverflow.com/questions/41393767/mosquitto-error-only-one-usage-of-each-socket-address-is-normally-permitted)

```
c:\Program Files\mosquitto>mosquitto.exe -v -c mosquitto.config
C:\Program Files\mosquitto>mosquitto -v -c mosquitto.conf
1701196839: mosquitto version 2.0.18 starting
1701196839: Config loaded from mosquitto.conf.
1701196839: Opening ipv6 listen socket on port 1883.
1701196839: Opening ipv4 listen socket on port 1883.
1701196839: mosquitto version 2.0.18 running
1701196889: New connection from 192.168.1.5:49200 on port 1883.
```

#### Subscribe and Publish MQTT Messages from esp8266_mqtt application

Once MQTT connection is established and MQTT broker service is started on client,
the server (ESP8266) can subscribe and publish the MQTT messages using 'esp8266_parse_mqtt_msgs()'
function which runs in a while(1) loop of single threaded main function.

Application can create multiple MQTT topics to which server can subcribe and then
publish the data. Client can send event on subscibed topics,
which server can receive and then in response, publish topics data. The code that
handles these services is defined below.

**NOTE**
This tutorial only covers publishing of single topic (event/temperature) using node-red. 
User can create complete UI (e.g. using node-red or any other custom application) on PC
side to handle subscribe and publish services by modifying this code according to
requirements.

```
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

/* Parse MQTT subscribe and publish messages (esp8266_mqtt_parser.c) */
void esp8266_parse_mqtt_msgs(void)
{
	switch (mqtt_msg_type) {
	case MQTT_MSG_SUBSCRIBE:
		esp8266_mqt_msg_subscribe();
		mqtt_msg_type = MQTT_MSG_PUBLISH;
		break;

	case MQTT_MSG_PUBLISH:
		esp8266_mqt_msg_publish();
		mqtt_msg_type = MQTT_MSG_SUBSCRIBE;
		break;

	default:
		break;
	}
}

/* Subscribed topics information */
esp8266_mqtt_subscribe_config event_subscribe = {
	.mqtt_data = NULL,
	.mqtt_data_len = 0,
	.packet_id = 1,
	.dup_flag = 0,
	.qos = { 0, 0, 0 },	// quality of service
	.topic = { (uint8_t *)"event/logger", (uint8_t *)"event/temperature", (uint8_t *)"event/voltage" },
	.counts = 3,
};

/* Subscribe the MQTT topics */
esp8266_wifi_status esp8266_mqt_msg_subscribe(void)
{
	int data_len;
	char resp_payload[20];
	esp8266_wifi_status status;

	MQTTString topic_filter[] = {
		{ (char *)event_subscribe.topic[0], {0, NULL} },
		{ (char *)event_subscribe.topic[1], {0, NULL} },
		{ (char *)event_subscribe.topic[2], {0, NULL} },
	};

	memset(mqtt_msg_buffer, 0, sizeof(mqtt_msg_buffer));

	/* Serialize the mqtt packet */
	data_len = MQTTSerialize_subscribe((unsigned char *)mqtt_msg_buffer, 500,
					   event_subscribe.dup_flag, event_subscribe.packet_id,
					   event_subscribe.counts, topic_filter, event_subscribe.qos);

	/* Send subscribe message using "AT+CIPSEND=msg_len\r\n" command */
	status = esp8266_send_subscribe_msg(mqtt_msg_buffer, data_len, resp_payload,
					    sizeof(resp_payload));
	if (status != ESP8266_WIFI_SUCCESS) {
		return status;
	}

	/* Parse the response and determine the subscribed topic */
	if (strstr(resp_payload, "logger")) {
		publish_topic = LOGGER;
	} else if (strstr(resp_payload, "temperature")) {
		publish_topic = TEMPERATURE;
	} else if (strstr(resp_payload, "voltage")) {
		publish_topic = VOLTAGE;
	} else {
		// do nothing
	}

	return ESP8266_WIFI_SUCCESS;
}

/* Publish data for subscribed topics */
char *my_data[5] = { "100", "200", "300", "400", "500", };
u_int8_t my_data_indx;
MQTTString topicString;

esp8266_wifi_status esp8266_mqt_msg_publish(void)
{
	int data_len;
	esp8266_wifi_status status;

	switch (publish_topic) {
	case LOGGER:
		for (uint8_t i=0; i< 200; i++) {
			memset(mqtt_msg_buffer, 0, sizeof(mqtt_msg_buffer));

			/* Serialize the mqtt packet */
			topicString.cstring = (char *)logger_publish.topic;
			logger_publish.mqtt_data = (uint8_t *)my_data[my_data_indx];
			logger_publish.mqtt_data_len = strlen(my_data[my_data_indx]);
			my_data_indx++;
			if (my_data_indx > 5)
				my_data_indx = 0;

			data_len = MQTTSerialize_publish((unsigned char *)mqtt_msg_buffer,
							 500, 0u, logger_publish.qos, 0u,
							 logger_publish.packet_id, topicString,
							 (unsigned char *)logger_publish.mqtt_data,
							 logger_publish.mqtt_data_len);

			status = esp8266_send_publish_msg(mqtt_msg_buffer, data_len);
			if (status != ESP8266_WIFI_SUCCESS) {
				return status;
			}
		}
		break;

	case TEMPERATURE:
		break;

	case VOLTAGE:
		break;
	}

	return ESP8266_WIFI_SUCCESS;
}
```

#### Send MQTT subscribed topic events from PC client
Finally we need to have some client application running on computer/mobile that
would send publish events to topics. These events are then forwarded to ESP8266
Wi-Fi module via MQTT broker. The esp8266_mqtt firmware application running on
microcontroller (which has subscribed to these events) receives them and in
response publishes the response data. This is already described in section above.

For this tutorial, we are going to use Node-RED. The installation guide is [here](https://nodered.org/docs/getting-started/local).
It can be installed using command "npm install -g --unsafe-perm node-red".
To start node-red, just run command "node-red" in command prompt. Keep command prompt
open to continue running node-js services.

```
C:\Users\MPhalke>node-red
27 Nov 23:10:35 - [info]

Welcome to Node-RED
===================

27 Nov 23:10:35 - [info] Node-RED version: v3.1.0
27 Nov 23:10:35 - [info] Node.js  version: v16.17.1
27 Nov 23:10:35 - [info] Windows_NT 10.0.19045 x64 LE
27 Nov 23:10:36 - [info] Loading palette nodes
27 Nov 23:10:37 - [info] Settings file  : C:\Users\MPhalke\.node-red\settings.js
27 Nov 23:10:37 - [info] Context store  : 'default' [module=memory]
27 Nov 23:10:37 - [info] User directory : C:\Users\MPhalke\.node-red
27 Nov 23:10:37 - [warn] Projects disabled : editorTheme.projects.enabled=false
27 Nov 23:10:37 - [info] Flows file     : C:\Users\MPhalke\.node-red\flows.json
27 Nov 23:10:37 - [info] Creating new flow file
27 Nov 23:10:37 - [warn]

---------------------------------------------------------------------
Your flow credentials file is encrypted using a system-generated key.

If the system-generated key is lost for any reason, your credentials
file will not be recoverable, you will have to delete it and re-enter
your credentials.

You should set your own key using the 'credentialSecret' option in
your settings file. Node-RED will then re-encrypt your credentials
file using your chosen key the next time you deploy a change.
---------------------------------------------------------------------

27 Nov 23:10:37 - [info] Server now running at http://127.0.0.1:1880/
27 Nov 23:10:37 - [warn] Encrypted credentials not found
27 Nov 23:10:37 - [info] Starting flows
27 Nov 23:10:37 - [info] Started flows
```

To start creating MQTT workflow on local dashboard, type "http://127.0.0.1:1880/" 
address into webpage. This server address is mentioned in log messages when
node-js service has started.

You can create a simple flow using 'mqtt_in' and 'debug' node to receive publish
messages from esp8266 and then display them into debug tab. Just configure
your nodes to subscribe for a 'event/temperature' topic.

Refer this great [tutorial](https://www.youtube.com/watch?v=ccKAsXGTELc&list=PLKYvTRORAnx6a9tETvF95o35mykuysuOw&index=11) 
on node-red for creating various control flows.

#### mqtt_in node properties:

![mqtt_in node properties](https://github.com/mphalke/esp8266_mqtt/blob/main/resources/mqtt_in_node_properties.PNG)

#### debug node properties:

![debug node properties](https://github.com/mphalke/esp8266_mqtt/blob/main/resources/debug_node_properties.PNG)

#### Debug output:

![Node red debug output](https://github.com/mphalke/esp8266_mqtt/blob/main/resources/node_red_debug_output.PNG)
