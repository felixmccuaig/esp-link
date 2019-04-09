#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"

#include "lwip/raw.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"

#include "netif/espenc.h"

#include "driver/uart.h"

static struct raw_pcb *raw_pcb_tcp = NULL;
static struct raw_pcb *raw_pcb_udp = NULL;

static u8_t ICACHE_FLASH_ATTR
raw_receiver(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{
	os_printf("WLan IP packet of size %d\n", p->tot_len);
	return 0;
}

void ICACHE_FLASH_ATTR 
init_raw_sockets() 
{
    raw_pcb_tcp = raw_new(6);
	raw_pcb_udp = raw_new(17);
    if (!raw_pcb_tcp || !raw_pcb_udp) {
		os_printf("\nFailed to init raw sockets\n");
	} else {
        os_printf("\ninitialized raw sockets\n");
        raw_bind(raw_pcb_tcp, IP_ADDR_ANY);
		raw_bind(raw_pcb_udp, IP_ADDR_ANY);
		raw_recv(raw_pcb_tcp, raw_receiver, NULL);
		raw_recv(raw_pcb_udp, raw_receiver, NULL);
	}
}

void ICACHE_FLASH_ATTR 
init_enc()
{
    uint8_t mac_address[] = {0x98, 0xB6, 0xE9, 0x98, 0x8B, 0xAD};
    //uint8_t *arr_pointer = os_malloc(6); //0x98, 0xB6, 0xE9, 0x98, 0x8B, 0xAD
    //os_memcpy(arr_pointer, &mac_address, 6);
    struct netif* new_netif;
    //new_netif = 
    espenc_init(NULL, NULL, NULL, NULL, true);
}

void ICACHE_FLASH_ATTR user_init()
{
    uart_div_modify(UART0, UART_CLK_FREQ / BIT_RATE_9600);
    os_delay_us(65535);
    init_raw_sockets();
    init_enc();
}