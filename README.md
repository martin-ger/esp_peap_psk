# esp_peap_psk
A WPA2-PEAP to WPA2-PSK converter

Connect your IoT devices to the Internet via an WPA2-PEAP Enterprise network.

This small application of the ESP8266 solves a common problem especially in university environments: the local WiFi network is a WPA2 Enterprise network with PEAP-MSCHAPv2 authentication. A very prominent example is the "eduroam"-network that is available at many universities around the world. The problem is, that many IoT devices cannot handle WPA2 Enterprise authentication. So development and demos are difficult. What is very helpful is a "converter" that logs into the WPA2 Enterprise network and offers a simpler WPA-PSK network to its clients.

Togehter with  my "nat" versio of the esp-open-lwip library this minimal app provides such a converter. It again acts as a WiFi NAT router like the esp_wifi_repeater (https://github.com/martin-ger/esp_wifi_repeater), but is is much simpler and has no user interface. Once compiled it is just plug-and-pray! ;-)

All configuration has to be done at compile time in user/user_config.h.

# Building
To build this binary you download and install the esp-open-sdk version 2.1(!) (https://github.com/pfalcon/esp-open-sdk). Make sure, you can compile and download the included "blinky" example.

Then download this source tree in a separate directory and adjust the BUILD_AREA variable in the Makefile and the settings and user credentials in user/user_config.h. Build the esp_peap_psk firmware with "make". "make flash" flashes it onto an esp8266.

The source tree includes a binary version of the liblwip_open plus the required additional includes from my fork of esp-open-lwip. *No additional install action is required for that.* Only if you don't want to use the precompiled library, checkout the sources from https://github.com/martin-ger/esp-open-lwip . Use it to replace the directory "esp-open-lwip" in the esp-open-sdk tree. "make clean" in the esp_open_lwip dir and once again a "make" in the upper esp_open_sdk directory. This will compile a liblwip_open.a that contains the NAT-features. Replace liblwip_open_napt.a with that binary.

# Usage
Go to your office, start it up and check via serial terminal (115200 baud) whether it connects (status LED also stopps flashing). If so, you are done and it will offer its own WPA-PSK network as SoftAP where you can connect e.g. with other ESP8266 IoT devices. As usual on the esp clients are configured via DHCP in the network 192.168.4.0/24 starting from 192.168.4.2. Max 8 clients are supported by the esp. They receive the DNS server address from the enterprise network.

# Security
This simple application does not check the certificate of the RADIUS-Server. It is vulnerable to MITM-attacks, when somebody sets up a rouge AP and RADIUS server. While the password is not send in plaintext, MSCHAPv2 is known to be broken. For serious usage, the root certificate has to be provided in the code as well. Basic hooks in the code are provided... 

Also, be aware of the fact that the ESP8266 now contains your enterprise network password. All traffic that is forwarded by it can now be related by the network admin to your account. Do not missuse it and offer it to untrusted others, eg. by configuring an open network. And while there is no user interface function to read your enterprise network password on ESP8266, it can be extracted from the flash in plain text. Do not leave your ESP unattended - at least if anybody else knows, what is does... ;-)
