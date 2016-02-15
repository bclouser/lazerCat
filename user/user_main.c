#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "ip_addr.h"
#include "espconn.h"
#include "user_interface.h"
#include "user_config.h"
#include "messageHandler.h"

/* non sdk includes */
#include "servo.h"
#include "secrets.h"


char buffer[ 2048 ];

void user_rf_pre_init( void )
{
}

void server_received( void *arg, char *pdata, unsigned short len )
{
    struct espconn *conn = arg;
    
    os_printf( "%s: %s\n", __FUNCTION__, pdata );

    // parse message and process requests
    if( !handleMessage(pdata) ){
        os_printf("Well, we failed to handle message. bummer\n\r");
    }

    if( isLazerOn() ){
        turnLazerOff();
    }
    else{
        turnLazerOn();
    }

    espconn_disconnect( conn );
}

void server_reconn( void *arg, sint8 err ){
    os_printf( "%s:\n", __FUNCTION__ );
}

void server_disconn( void *arg ){
    os_printf( "%s:\n", __FUNCTION__ );
    //wifi_station_disconnect();
}


LOCAL void ICACHE_FLASH_ATTR
tcp_connected( void *arg )
{
    struct espconn *conn = arg;
    
    os_printf( "%s\n", __FUNCTION__ );
    espconn_regist_recvcb(conn, server_received);
    espconn_regist_reconcb(conn, server_reconn);
    espconn_regist_disconcb(conn, server_disconn);

    os_sprintf( buffer, "Ben Says message received");
    
    espconn_sent( conn, buffer, os_strlen( buffer ) );
}

void tcp_disconnected( void *arg )
{
    struct espconn *conn = arg;
    os_printf( "%s\n", __FUNCTION__ );
    //wifi_station_disconnect();
}

void initTcpServer(uint32 port)
{
    os_printf( "%s\n", __FUNCTION__ );
    
    os_printf("Starting tcp server on port %d...\n", port);
    LOCAL struct espconn esp_conn;
    LOCAL esp_tcp esptcp;

    esp_conn.type = ESPCONN_TCP;
    esp_conn.state = ESPCONN_NONE;
    esp_conn.proto.tcp = &esptcp;
    esp_conn.proto.tcp->local_port = port;
    espconn_regist_connectcb(&esp_conn, tcp_connected);
    espconn_regist_disconcb( &esp_conn, tcp_disconnected );
    espconn_accept(&esp_conn);
    os_printf("Well, there is no error checking so i guess we assume it worked :/\n");
}

// We aren't requesting a dns lookup so this will never be called
void dns_done( const char *name, ip_addr_t *ipaddr, void *arg )
{
    os_printf( "%s\n", __FUNCTION__ );
}


void wifi_callback( System_Event_t *evt )
{
    os_printf( "%s: %d\n", __FUNCTION__, evt->event );
    
    switch ( evt->event )
    {
        case EVENT_STAMODE_CONNECTED:
        {
            os_printf("connected to ssid %s, channel %d\n",
                        evt->event_info.connected.ssid,
                        evt->event_info.connected.channel);
            break;
        }

        case EVENT_STAMODE_DISCONNECTED:
        {
            os_printf("disconnected from ssid %s, reason %d\n",
                        evt->event_info.disconnected.ssid,
                        evt->event_info.disconnected.reason);
            
            os_printf("Gunna take a minute long nap now\n");
            deep_sleep_set_option( 0 );
            system_deep_sleep( 60 * 1000 * 1000 );  // 60 seconds
            break;
        }

        case EVENT_STAMODE_GOT_IP:
        {
            os_printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
                        IP2STR(&evt->event_info.got_ip.ip),
                        IP2STR(&evt->event_info.got_ip.mask),
                        IP2STR(&evt->event_info.got_ip.gw));
            os_printf("\n");

            /* Food for thought: Publish or broadcast ip address to server so data providers can 
            dynamically learn the ip address of this device. I guess i could use static ip #yuck */

            // We got an IP address, get that tcp server setup!
            initTcpServer(5011);

            break;
        }
        
        default:
        {
            break;
        }
    }
}


void user_init( void )
{
    static struct station_config config;
    
    uart_div_modify( 0, UART_CLK_FREQ / ( 115200 ) );
    os_printf( "%s\n", __FUNCTION__ );

    wifi_station_set_hostname( "lazerCat" );
    wifi_set_opmode_current( STATION_MODE );

    // Initialize the GPIO subsystem.
    // Apparently this just needs to be called. Odd
    gpio_init();

    // Initialize ze lazer!!!!
    // really not that cool, its just a gpio
    initLazer();
    
    config.bssid_set = 0;
    os_memcpy( &config.ssid, WIFI_SSID, 32 );
    os_memcpy( &config.password, WIFI_PASSWD, 64 );
    wifi_station_set_config( &config );
    
    // Generic callback which handles various wifi states
    wifi_set_event_handler_cb( wifi_callback );

    // Init pwm modules which control the servo
    initServo();
}