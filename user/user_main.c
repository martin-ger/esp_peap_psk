#include "c_types.h"
#include "user_interface.h"
#include "mem.h"
#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/app/dhcpserver.h"

#include "user_config.h"

struct softap_config apConfig;
bool once_connected;

void wifi_handle_event_cb(System_Event_t *evt)
{
    ip_addr_t dns_ip;
    struct netif *nif;

    switch (evt->event) {
        case EVENT_STAMODE_CONNECTED:
            os_printf("[connect to ssid %s, channel %d]\n",
                evt->event_info.connected.ssid,
                evt->event_info.connected.channel);
	    once_connected = true;
            break;
        case EVENT_STAMODE_DISCONNECTED:
            os_printf("[disconnect from ssid %s, reason %d]\n",
                evt->event_info.disconnected.ssid,
                evt->event_info.disconnected.reason);
// This is a hack - but it works best
// Once the PEAP-Network is lost, we cannot recover... only by resetting!
	    if (once_connected)
		system_restart();
            break;
        case EVENT_STAMODE_AUTHMODE_CHANGE:
            os_printf("[mode: %d -> %d]\n",
                evt->event_info.auth_change.old_mode,
                evt->event_info.auth_change.new_mode);
            break;
        case EVENT_STAMODE_GOT_IP:

            os_printf("[ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR "]\n",
                IP2STR(&evt->event_info.got_ip.ip),
                IP2STR(&evt->event_info.got_ip.mask),
                IP2STR(&evt->event_info.got_ip.gw));

	    // Now start the AP
	    os_printf("Starting AP\r\n");

	    if( !wifi_set_opmode(STATIONAP_MODE) ) {
	        os_printf("[failed setting stationap mode]\n");
	    }
	    wifi_softap_set_config(&apConfig);

	    dns_ip = dns_getserver(0);
	    dhcps_set_DNS(&dns_ip);

            break;

	case EVENT_SOFTAPMODE_STACONNECTED:
	    nif = netif_list;
	    if (nif->num == 1) {
		nif->napt = 1;
	        os_printf("AP-Interface napted\r\n");
	    } else 
	        os_printf("AP-Interface not found\r\n");
	    break;

	case EVENT_SOFTAPMODE_STADISCONNECTED:
	    break;

        default:
            break;
    }
}

void ICACHE_FLASH_ATTR user_init(void)
{
    struct station_config wifi_config;

    gpio_init();

    uart_div_modify(0, UART_CLK_FREQ/115200);
    os_delay_us(1000000);

    wifi_status_led_install(WIFI_LED_IO_NUM, WIFI_LED_IO_MUX, FUNC_GPIO0); 

    os_printf("\r\n[SDK version:%s]\n", system_get_sdk_version());

    // Set ESP8266 softap config
    wifi_softap_get_config(&apConfig); // Get config first.
    
    os_memset(apConfig.ssid, 0, 32);
    os_memset(apConfig.password, 0, 64);
    os_memcpy(apConfig.ssid, WIFI_AP_SSID, os_strlen(WIFI_AP_SSID));
    os_memcpy(apConfig.password, WIFI_AP_PASSWORD, os_strlen(WIFI_AP_PASSWORD));
    apConfig.authmode = AUTH_WPA_WPA2_PSK;
    apConfig.ssid_len = 0;// or its actual length
    apConfig.max_connection = MAX_CLIENTS;
 
    wifi_softap_set_config(&apConfig);

    // first station mode: 
    if( !wifi_set_opmode(STATION_MODE) ) {
        os_printf("[failed setting station mode]\n");
    }
    once_connected = false;
    os_printf("Starting STA\r\n");
    // set station configuration, save to flash
    os_memset(&wifi_config,0,sizeof(wifi_config));
    os_strcpy(wifi_config.ssid, WIFI_SSID, os_strlen(WIFI_SSID));
    os_strcpy(wifi_config.password, WIFI_PASSWORD, WIFI_PASSWORD);
    wifi_station_set_config(&wifi_config);

    // WPA2-Enterprise
    wifi_station_set_wpa2_enterprise_auth(1);
    // ignore CA Certificate for now
    //wifi_station_set_enterprise_ca_cert(ca_pem, ca_pem_len + 1);
    wifi_station_set_enterprise_username(EDUROAM_ID, os_strlen(EDUROAM_ID));
    wifi_station_set_enterprise_password(EDUROAM_PW, os_strlen(EDUROAM_PW));

    wifi_station_connect();

    wifi_set_event_handler_cb(wifi_handle_event_cb);
}
