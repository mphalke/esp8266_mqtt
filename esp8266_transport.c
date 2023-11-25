#include "esp8266_transport.h"
#include "platform.h"

esp8266_wifi_status esp8266_uart_read(char *buffer, uint16_t bytes)
{
	if (uart_read(ESP8266_UART, buffer, bytes) != PLATFORM_SUCCESS)
		return ESP8266_WIFI_FAILURE;
	
	return ESP8266_WIFI_SUCCESS;
}

esp8266_wifi_status esp8266_uart_write(const char *buffer, uint16_t bytes)
{
	if (uart_write(ESP8266_UART, buffer, bytes) != PLATFORM_SUCCESS)
		return ESP8266_WIFI_FAILURE;
	
	return ESP8266_WIFI_SUCCESS;
}

void esp8266_uart_rx_callback_register(void *callback)
{
	uart_rx_callback_register(ESP8266_UART, callback);
}
