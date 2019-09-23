#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "mem.h"
#include "user_interface.h"

#include "driver/uart.h"
#include "driver/easygpio.h"

#include "netif/espenc.h"

#include "lwip/netif.h"
#include "lwip/lwip_napt.h"
#include "lwip/ip_addr.h"
#include "lwip/app/ping.h"

#include <string.h>


#define log(s, ...) os_printf ("[%s:%s:%d] " s "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define IP_NAPT_MAX 512
#define IP_PORTMAP_MAX 32
#define ENC28J60_HW_RESET 4

static struct netif *eth_if;
static struct netif *ap_if;
os_timer_t buff_timer_t;

void ICACHE_FLASH_ATTR init_enc()
{
    //ETHERNET WILL GET AN IP ASSIGNED
    log("INFO", "Enc initializing");
    uint8_t mac_addr[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
    eth_if = espenc_init( mac_addr, NULL, NULL, NULL, true, true);
    log("Enc was successfully initialized");
    log("interface %c%c with mac %X%X%X%X%X%X is up", eth_if->name[0], eth_if->name[1], 
        eth_if->hwaddr[0], eth_if->hwaddr[1], eth_if->hwaddr[2], eth_if->hwaddr[3], eth_if->hwaddr[4], eth_if->hwaddr[5]);
}

struct ping_option ping_opt;
uint8_t ping_success_count;

void ICACHE_FLASH_ATTR user_ping_sent(void *arg, void *pdata)
{
    /*
    char response[128];

    os_sprintf(response, "ping finished (%d/%d)\r\n", ping_success_count, ping_opt.count);
    to_console(response);
    system_os_post(0, SIG_CONSOLE_TX, (ETSParam)currentconn);
    */
}

void ICACHE_FLASH_ATTR user_ping_recv(void *arg, void *pdata)
{
    struct ping_resp *ping_resp = pdata;
    struct ping_option *ping_opt = arg;
    char response[128];

    if (ping_resp->ping_err == -1)
    {
        os_printf("ping failed\r\n");
    }
    else
    {
        os_printf("ping recv bytes: %d time: %d ms\r\n", ping_resp->bytes, ping_resp->resp_time);
        ping_success_count++;
    }

    //to_console(response);
    //system_os_post(0, SIG_CONSOLE_TX_RAW, (ETSParam)currentconn);
}

void ICACHE_FLASH_ATTR perform_ping(const char *name, ip_addr_t *ipaddr, void *arg)
{
    ping_opt.count = 4;
    ping_opt.coarse_time = 1;
    ping_opt.ip = ipaddr->addr;
    ping_success_count = 0;

    ping_regist_recv(&ping_opt, user_ping_recv);
    ping_regist_sent(&ping_opt, user_ping_sent);

    ping_start(&ping_opt);
}

void ICACHE_FLASH_ATTR uart_rx()
{
    uint8 uart_buf[128]={0};
    uint16 len = 0;
    len = rx_buff_deq(uart_buf, 128 );

    if(len > 0) {
        char *delim = " ";
        char *ptr = strtok(uart_buf, delim);
        char *tokens[10] = {};
        char *response;
        
        tokens[0] = ptr;

        int i = 1;
        while(ptr != NULL)
        {
            ptr = strtok(NULL, delim);
            tokens[i] = ptr;
            i++;
        }
       
        if(strstr(tokens[0], "version")) {
            response = "version is v1.0 \n";
        } else if(strstr(tokens[0], "show")) {
            if(strstr(tokens[1], "ip")) {
                if(strstr(tokens[2], "en")) {
                    u32_t ip = eth_if->ip_addr.addr; 
                    os_printf("ip address of int en is: %d.%d.%d.%d \n", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
                    response = "";
                } else if(strstr(tokens[2], "wi")) {
                    
                } else {
                    response = "show ip en | wi\n";
                }
            } if(strstr(tokens[1], "netif")) {
                if(strstr(tokens[2], "all")) {
                    struct netif *netif = netif_list;
                    while (netif != NULL) {
                        u32_t ip = netif->ip_addr.addr; 
                        os_printf("ip address of int %c%c is: %d.%d.%d.%d \n", netif->name[0], netif->name[1], ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
                        netif = netif->next;
                    }
                    response = "";
                } else {
                    response = "show netif all | en | wi\n";
                }
            } else {
                response = "A list of commands that can be entered are: \nversion\nshow\nhelp\n";
            }
        } else if(strstr(tokens[0], "ping")) {
            os_printf("Pinging %s\n", tokens[1]);


            ip_addr_t ip;
            ip.addr = ipaddr_addr(tokens[1]);

            perform_ping(tokens[1], &ip, NULL);

            response = "";
        } else {
            response = "unrecognised command \n";
        }
        tx_buff_enq(response, os_strlen(response));
    }
}

void ICACHE_FLASH_ATTR create_timer()
{
    os_timer_disarm(&buff_timer_t);
    os_timer_setfn(&buff_timer_t, uart_rx , NULL);
    os_timer_arm(&buff_timer_t,500,1);
}

void ICACHE_FLASH_ATTR user_init()
{
    uart_init(BIT_RATE_115200, BIT_RATE_115200);
    os_delay_us(65535 / 2);
    log("");
    log("Successful boot");

    bool result = wifi_set_opmode(SOFTAP_MODE);
    if(!result) {
        log("Setting wifi to AP failed\n");
    }

    struct softap_config access_point_conf;
    
    char *ssid = "FELIX AP";	
    char password[64] = "55354004";	
    access_point_conf.authmode = AUTH_WPA2_PSK;
    access_point_conf.ssid_len = os_strlen(ssid);
    access_point_conf.max_connection = 4;
    access_point_conf.channel = 8;
    os_memcpy(&access_point_conf.ssid, ssid, os_strlen(ssid));	
    os_memcpy(&access_point_conf.password, password, 64);	
    wifi_softap_set_config(&access_point_conf);

/*
    gpio_init();

    easygpio_pinMode(ENC28J60_HW_RESET, EASYGPIO_PULLUP, EASYGPIO_OUTPUT);
    easygpio_outputSet(ENC28J60_HW_RESET, 0);
    os_delay_us(500);
    easygpio_outputSet(ENC28J60_HW_RESET, 1);
    os_delay_us(1000);

    init_enc();

    ip_napt_init(IP_NAPT_MAX, IP_PORTMAP_MAX);
    ip_napt_enable_no(eth_if->num, 1);

    create_timer();
    */
}