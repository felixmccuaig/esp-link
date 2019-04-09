#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"

#include "lwip/raw.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/icmp.h"

#include "driver/espenc.h"
#include "driver/uart.h"

static struct raw_pcb *raw_pcb_tcp = NULL;
static struct raw_pcb *raw_pcb_udp = NULL;
static struct raw_pcb *raw_pcb_icmp = NULL;
static struct raw_pcb *raw_pcb_igmp = NULL;

static struct netif *eth_if;

static u8_t ICACHE_FLASH_ATTR
raw_receiver_udp(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{
    os_printf("UDP packet rxed of size %d\n", p->tot_len);
    u8_t protocol = pcb->protocol;
    os_printf("Packet rxed of protocol %d\n", protocol);
	

	return 0;
}

static u8_t ICACHE_FLASH_ATTR
raw_receiver_tcp(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{
    os_printf("TCP packet rxed of size %d\n", p->tot_len);
    u8_t protocol = pcb->protocol;
    os_printf("Packet rxed of protocol %d\n", protocol);
	

	return 0;
}

static u8_t ICACHE_FLASH_ATTR
raw_receiver_icmp(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{
    os_printf("ICMP packet rxed of size %d\n", p->tot_len);
    u8_t protocol = pcb->protocol;
    os_printf("Packet rxed of protocol %d\n", protocol);


    icmp_input(p, eth_if);

    
	return 0;
}

static u8_t ICACHE_FLASH_ATTR
raw_receiver_igmp(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
{
    os_printf("IGMP packet rxed of size %d\n", p->tot_len);
    u8_t protocol = pcb->protocol;
    os_printf("Packet rxed of protocol %d\n", protocol);

    

	return 0;
}

void ICACHE_FLASH_ATTR 
init_raw_sockets() 
{
    raw_pcb_tcp = raw_new(IP_PROTO_TCP);
	raw_pcb_udp = raw_new(IP_PROTO_UDP);
    raw_pcb_icmp = raw_new(IP_PROTO_ICMP);
    raw_pcb_igmp = raw_new(IP_PROTO_IGMP);

    if (!raw_pcb_tcp || !raw_pcb_udp || !raw_pcb_icmp || !raw_pcb_igmp) {
		os_printf("\nFailed to init raw sockets\n");
	} else {
        os_printf("\n\n\n\n\ninitialized raw sockets\n\n\n\n\n");
        raw_bind(raw_pcb_tcp, IP_ADDR_ANY);
		raw_bind(raw_pcb_udp, IP_ADDR_ANY);
        raw_bind(raw_pcb_icmp, IP_ADDR_ANY);
		raw_bind(raw_pcb_igmp, IP_ADDR_ANY);

		raw_recv(raw_pcb_tcp, raw_receiver_tcp, NULL);
		raw_recv(raw_pcb_udp, raw_receiver_udp, NULL);
        raw_recv(raw_pcb_icmp, raw_receiver_icmp, NULL);
		raw_recv(raw_pcb_igmp, raw_receiver_igmp, NULL);
	}
}

void ICACHE_FLASH_ATTR 
init_enc()
{
    eth_if = espenc_init();
}

void ICACHE_FLASH_ATTR user_init()
{
    uart_div_modify(UART0, UART_CLK_FREQ / BIT_RATE_9600);
    os_delay_us(65535);
    init_raw_sockets();
    init_enc();
}