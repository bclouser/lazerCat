#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "ip_addr.h"
#include "espconn.h"
#include "user_interface.h"
#include "user_config.h"
#include "pwm.h"

#include "secrets.h"

/*Definition of GPIO PIN params, for GPIO initialization*/
#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_MTDI_U
#define PWM_0_OUT_IO_NUM 12
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO12

#define PWM_1_OUT_IO_MUX PERIPHS_IO_MUX_MTDO_U
#define PWM_1_OUT_IO_NUM 15
#define PWM_1_OUT_IO_FUNC  FUNC_GPIO15

#define PWM_2_OUT_IO_MUX PERIPHS_IO_MUX_MTCK_U
#define PWM_2_OUT_IO_NUM 13
#define PWM_2_OUT_IO_FUNC  FUNC_GPIO13

#define PWM_3_OUT_IO_MUX PERIPHS_IO_MUX_MTMS_U
#define PWM_3_OUT_IO_NUM 14
#define PWM_3_OUT_IO_FUNC  FUNC_GPIO14

#define PWM_4_OUT_IO_MUX PERIPHS_IO_MUX_GPIO5_U
#define PWM_4_OUT_IO_NUM 5
#define PWM_4_OUT_IO_FUNC  FUNC_GPIO5



#define user_procTaskPrio        0
#define user_procTaskQueueLen    2
os_event_t user_procTaskQueue[user_procTaskQueueLen];


static void loop(os_event_t *events);

struct espconn our_conn;
ip_addr_t our_ip;
esp_tcp our_tcp;

char json_data[ 256 ];
char buffer[ 2048 ];

bool wifiConnected = false;
bool tcpServerUp = false;

static volatile os_timer_t some_timer;


int period = 1;
bool ascend = true;

void some_timerfunc(void *arg)
{
    pwm_set_period(period);
    pwm_set_duty(500, 5);
    pwm_start(); 

    if( (period >= 1000) || (period <= 0) ){
        ascend = (!ascend);
    }

    if(ascend){
        os_printf("period = %d\n", period);
        ++period;
    }
    else{
        os_printf("period = %d\n", period);
        --period;
    }
}


void user_rf_pre_init( void )
{
}

void server_received( void *arg, char *pdata, unsigned short len )
{
    struct espconn *conn = arg;
    
    os_printf( "%s: %s\n", __FUNCTION__, pdata );
    
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

    os_sprintf( buffer, "Ben Says hello");
    
    os_printf( "Sending: %s\n", buffer );
    espconn_sent( conn, buffer, os_strlen( buffer ) );
}

void tcp_disconnected( void *arg )
{
    struct espconn *conn = arg;
    os_printf( "%s\n", __FUNCTION__ );
    //wifi_station_disconnect();
}

initTcpServer(uint32 port)
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
    tcpServerUp = true;
}

// We aren't requesting a dns lookup so this will not be called
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
            os_printf("connect to ssid %s, channel %d\n",
                        evt->event_info.connected.ssid,
                        evt->event_info.connected.channel);
            break;
        }

        case EVENT_STAMODE_DISCONNECTED:
        {
            os_printf("disconnect from ssid %s, reason %d\n",
                        evt->event_info.disconnected.ssid,
                        evt->event_info.disconnected.reason);
            
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
            wifiConnected = true;

            //Start os task
            //system_os_task(loop, user_procTaskPrio,user_procTaskQueue, user_procTaskQueueLen);
            //system_os_post(user_procTaskPrio, 0, 0 );

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
    gpio_init();
    //Set GPIO2 to output mode
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
    //Set GPIO2 low
    gpio_output_set(0, BIT2, BIT2, 0);
    
    config.bssid_set = 0;
    os_memcpy( &config.ssid, WIFI_SSID, 32 );
    os_memcpy( &config.password, WIFI_PASSWD, 64 );
    wifi_station_set_config( &config );
    
    wifi_set_event_handler_cb( wifi_callback );

    uint32 io_info[][3] = {   {PWM_0_OUT_IO_MUX,PWM_0_OUT_IO_FUNC,PWM_0_OUT_IO_NUM},
                              {PWM_1_OUT_IO_MUX,PWM_1_OUT_IO_FUNC,PWM_1_OUT_IO_NUM},
                              {PWM_2_OUT_IO_MUX,PWM_2_OUT_IO_FUNC,PWM_2_OUT_IO_NUM},
                              {PWM_3_OUT_IO_MUX,PWM_3_OUT_IO_FUNC,PWM_3_OUT_IO_NUM},
                              {PWM_4_OUT_IO_MUX,PWM_4_OUT_IO_FUNC,PWM_4_OUT_IO_NUM},
                              };

    uint32 pwm_duty_init[] = {1};
    pwm_init(1, pwm_duty_init, 0, io_info );
    //pwm_init(uint32 period, uint32 *duty,uint32 pwm_channel_num,uint32 (*pin_info_list)[3]);
    //pwm_set_period(100);
    //pwm_set_duty(2000, 1);
    pwm_start(); 

    //Disarm timer
    os_timer_disarm(&some_timer);

    //Setup timer
    //os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);

    //Arm the timer
    //&some_timer is the pointer
    //1000 is the fire time in ms
    //0 for once and 1 for repeating
    //os_timer_arm(&some_timer, 10, 0);
}


//Main code function
static void ICACHE_FLASH_ATTR loop(os_event_t *events)
{
    if(wifiConnected){
        // if we haven't yet created our tcp server. Bring it up!
        if(!tcpServerUp){
            initTcpServer(5011);
        }
        else
        {
            //os_printf("wifi connected and tcp server up\n\r");

            //Do blinky stuff
            if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2)
            {
                //Set GPIO2 to LOW
                //gpio_output_set(0, BIT2, BIT2, 0);
                gpio_output_set(0, BIT2, BIT2, 0);
            }
            else
            {
                //Set GPIO2 to HIGH
                gpio_output_set(BIT2, 0, BIT2, 0);
            }
            os_delay_us(1000000);
        }
    }
    else{
        os_printf("wifi not connected yet\n\r");
        os_delay_us(5000000);
    }

    system_os_post(user_procTaskPrio, 0, 0 );
}