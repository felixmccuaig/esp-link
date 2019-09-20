#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"
#include "driver/uart.h"

#define log(s, ...) os_printf ("[%s:%s:%d] " s "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

/*
#include "netif/espenc.h"
#include "lwip/netif.h"

#include "lwip/lwip_napt.h"

#define IP_NAPT_MAX 512
#define IP_PORTMAP_MAX 32

static struct netif *eth_if;

void ICACHE_FLASH_ATTR init_enc()
{
    //ETHERNET WILL GET AN IP ASSIGNED
    //log("INFO", "Enc initializing");
    uint8_t mac_addr[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    eth_if = espenc_init( mac_addr, NULL, NULL, NULL, true );
    log("Enc was successfully initialized");
    log("interface %c%c with mac %X%X%X%X%X%X is up", eth_if->name[0], eth_if->name[1], 
        eth_if->hwaddr[0], eth_if->hwaddr[1], eth_if->hwaddr[2], eth_if->hwaddr[3], eth_if->hwaddr[4], eth_if->hwaddr[5]);
}

*/

void ICACHE_FLASH_ATTR user_init()
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    log("");
    log("Successful boot");

    //gpio_init();
    //init_enc();

    //ip_napt_init(IP_NAPT_MAX, IP_PORTMAP_MAX);

    //ip_napt_enable_no(eth_if->num, 1);

    
}