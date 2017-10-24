#ifndef _USER_CONFIG_
#define _USER_CONFIG_

//
// Define this for your network environment 
//

#define WIFI_SSID            "eduroam"
#define WIFI_PASSWORD        ""

#define WIFI_AP_SSID         "WPA-PSK-NET"
#define WIFI_AP_PASSWORD     "password123"

#define EDUROAM_IDENTITY     "user1@uni1.edu"
#define EDUROAM_USERNAME     "user1"
#define EDUROAM_PW	     "good_password123"

#define MAX_CLIENTS	     8



//
// Define this if you have a status LED connected to GPIO LED_NO
//

// GPIO 2
#define WIFI_LED_IO_MUX     PERIPHS_IO_MUX_GPIO2_U
#define WIFI_LED_IO_NUM     2
#define WIFI_LED_IO_FUNC    FUNC_GPIO2

// GPIO 1
//#define WIFI_LED_IO_MUX     PERIPHS_IO_MUX_U0TXD_U
//#define WIFI_LED_IO_NUM     1
//#define WIFI_LED_IO_FUNC    FUNC_GPIO1

#endif
