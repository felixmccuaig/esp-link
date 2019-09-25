# esp-link

This project is a tiny router which routes packets from an enc28j60 ethernet port to the esp8266. The ETH is configured to use DHCP to aquire an ip address from the router which it is plugged into. This is not a NAT router, and as such you will need to configure a default route to network 192.168.4.0 from the router which it is plugged into. Speeds of .42 Mbps down and 0.57 Mbps up were achieved with one device connected to the access point. The esp8266 which was used in this case was the NODEMCU. Thanks to Martin-Ger for writing esp_wifi_repeater, I have borrowed some code from the project and used martin-ger/esp-open-lwip as a base for the LWIP. The ENC is connected to the ESP8266 using SPI. Thanks metalphreak for the SPI driver. To wire the ESP and ENC together use the (taken from Martin-Ger esp_wifi_repeater). 

NodeMCU/Wemos  ESP8266      ENC28J60

        D6     GPIO12 <---> MISO
        D7     GPIO13 <---> MOSI
        D5     GPIO14 <---> SCLK
        D8     GPIO15 <---> CS
        D1     GPIO5  <---> INT
	D2     GPIO4  <---> RESET
               Q3/V33 <---> 3.3V
               GND    <---> GND
               
Use a pulldown resistor for GPIO 15 https://esp8266hints.wordpress.com/category/ethernet/.
