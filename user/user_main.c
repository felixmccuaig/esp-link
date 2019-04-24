#include "user_main.h"

static struct netif *eth_if;

void ICACHE_FLASH_ATTR init_enc()
{
    //ETHERNET WILL GET AN IP ASSIGNED
    log("INFO", "Enc initializing");
    eth_if = espenc_init();
    log("INFO", "Enc was successfully initialized");
    log("INFO", "interface %c%c with mac %X %X %X %X %X %X is up", eth_if->name[0], eth_if->name[1], 
        eth_if->hwaddr[0], eth_if->hwaddr[1], eth_if->hwaddr[2], eth_if->hwaddr[3], eth_if->hwaddr[4], eth_if->hwaddr[5]);
}

err_t
eth_input(struct pbuf *p, struct netif *netif)
{
    log("INFO", "RECEIVED A FRAME");
}

void ICACHE_FLASH_ATTR user_init()
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    log("INFO", "Successful boot");

    gpio_init();
    init_enc();
}