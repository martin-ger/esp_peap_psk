# esp_peap_psk
A WPA2-PEAP to WPA2-PSK converter

Connect your IoT devices to the Internet via an WPA2-PEAP Enterprise network.

This small application of the ESP8266 solves a common problem especially in university environments: the local WiFi network is a WPA2 Enterprise network with PEAP-MSCHAPv2 authentication. A very prominent example is the "eduroam"-network that is available at many universities around the world. The problem is, that many IoT devices cannot handle WPA2 Enterprise authentication. So development and demos are difficult. Also the ESP8266 has a lot of problems getting a connect here. Thanks to https://github.com/joostd/esp8266-eduroam/tree/master/wpa2e-v20 we have at least a template for that.

What is very helpful is a "converter" that logs into the WPA2 Enterprise network and offers a simpler WPA-PSK network to its clients.

Togehter with  my "nat" branch of the esp-open-lwip library (https://github.com/martin-ger/esp-open-lwip/tree/nat) this minimal app provides such a converter. It again acts as a WiFi NAT router like the esp_wifi_repeater (https://github.com/martin-ger/esp_wifi_repeater), but is is much simpler and has no user interface. Once compiled it is just plug-and-pray! ;-)

All configuration has to be done at compile time. This is not a bug, but a necessity, as the so called "outer identification" has to be patched into the binary libwpa2.a. Ugly, but the only known way to get WPA2-PEAP connectivity (at least with a PEAP-network with many realms (RADIUS servers) like the eduroam).

# Building
To build this binary you have to download and install the esp-open-sdk (https://github.com/pfalcon/esp-open-sdk) and my "nat" branch of the esp-open-lwip library (https://github.com/martin-ger/esp-open-lwip/tree/nat). The easiest way to get it is to download:
https://github.com/martin-ger/esp-open-lwip/archive/nat.zip . Unpack and replace that in the esp-open-sdk tree. "make clean" in the esp_open_lwip dir and once again a "make" in the upper esp_open_sdk will do the job. This installs a new version of the liblwip_open.a that contains the NAT-features.

Now the important step of patching the libwpa2.a:
- install hexedit (or another tool that can do binary editing)
- cd to your esp_open_sdk directory
- cd sdk/lib/
- cp libwpa2.a libwpa3.a
- hexedit libwpa3.a
- search (by typing tab and then "/") for "anonymous@espressif.com"
- overwrite it with your username, e.g. "user1@uni1.edu", fill the remaining bytes of the original string with 00 (by switching back to hex mode with tab)
- if your username is longer than "anonymous@espressif.com" use a fake username, but the correct domain. (e.g. "anon@uni1.edu" instead of "student01928.computerscience@uni1.edu"). Should work as well. This string is only important for routing the following authentication protocol to the correct RADIUS server.
- once you have done the patching, save the libwpa3.a 

Then go to your esp_peap_psk and adjust the BUILD_AREA variable in the Makefile. Enter the corrct network names, passwords and credentials in user/user_config.h.

Build the esp_peap_psk firmware with "make". It will use the patched. libwpa3.a "make flash" flashes it onto an ESP8266.

I know this is a ugly hack - but I am not aware of another option as long as the sdk offers no API for setting the outer identification.

# Usage
Go to your office, start it up and check via serial terminal (115200 baud) whether it connects (status LED also stopps flashing). If so, you are done and it will offer its own WPA-PSK network as SoftAP where you can connect e.g. with other ESP8266 IoT devices. As usual on the esp clients are configured via DHCP in the network 192.168.4.0/24 starting from 192.168.4.2. Max 8 clients are supported by the esp. They receive the DNS server address from the enterprise network.

# Security
This simple application does not check the certificate of the RADIUS-Server. It is vulnerable to MITM-attacks, when somebody sets up a rouge AP and RADIUS server. While the password is not send in plaintext, MSCHAPv2 is known to be broken. For serious usage, the root certificate has to be provided in the code as well. Basic hooks in the code are provided... 

Also, be aware of the fact that the ESP8266 now contains your enterprise network password. All traffic that is forwarded by it can now be related by the network admin to your account. Do not missuse it and offer it to untrusted others, eg. by configuring an open network. And while there is no user interface function to read your enterprise network password on ESP8266, it can be extracted from the flash in plain text. Do not leave your ESP unattended - at least if anybody else knows, what is does... ;-)
