#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_attr.h"

#include "nvs_flash.h"
#include "driver/gpio.h"
#include "tm1637.h"
#include "driver/ledc.h"
#include "pwm.h"
#include "esp_sntp.h"
#include "protocol_examples_common.h"

#include "pin_names.h"
#include "alarm.h"
#include "webui.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

static const char *TAG = "example";

int time_set = 0;

//GLOBAL STATES FOR INDICATORS
volatile int led_br_on = 0;
volatile int led_bl_on = 0;
volatile int led_tl_on = 0;

//this is the led driver
tm1637_led_t * lcd;

httpd_req_t



static void obtain_time(void) __attribute__((unused));
static void initialize_sntp(void);

void time_sync_notification_cb(struct timeval *tv)
{
   ESP_LOGI(TAG, "Notification of a time synchronization event");
}
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


static void obtain_time(void)
{
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 9999;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
	led_br_on = !led_br_on;
	led_tl_on = !led_tl_on;
	led_bl_on = !led_bl_on;
        //TODO figure out how to make this in a method
	//gpio_set_level(LED_BR, led_br_on);
        //gpio_set_level(LED_TL, led_tl_on);
        //gpio_set_level(LED_BL, led_bl_on);
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    led_br_on = 0;
    led_tl_on = 0;
    led_bl_on = 0;
    //gpio_set_level(LED_BR, led_br_on);
    //gpio_set_level(LED_TL, led_tl_on);
    //gpio_set_level(LED_BL, led_bl_on);
    time(&now);
    localtime_r(&now, &timeinfo);
    time_set = 1;
    //ESP_ERROR_CHECK( example_disconnect() );
}

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}

void lcd_tm1637_task(void * arg) {

	setenv("TZ", "EST+5EDT,M3.2.0/03:00:00,M11.1.0/02:00:00", 1);
	tzset();
	int toggle = 0;

	while (true)
	{
        tm1637_set_brightness(lcd, disp_brightness);
		if(time_set) {
			time_t now = 0;
			struct tm timeinfo = { 0 };
			time(&now);
			localtime_r(&now, &timeinfo);
			int time_number = 100 * timeinfo.tm_hour + timeinfo.tm_min;

			// Display time with blinking dots
			tm1637_set_number_lead_dot(lcd, time_number, true, toggle++%2 ? 0xFF : 0x00);
		    vTaskDelay(500 / portTICK_RATE_MS);
		} else {
			tm1637_set_number_lead_dot(lcd, 8888, true, toggle++%2 ? 0xFF : 0x00);
		    vTaskDelay(500 / portTICK_RATE_MS);
		}
	}  
}

void app_main(void)
{
    // Set up our hardware, get everything booted
    lcd = tm1637_init(LCD_CLK, LCD_DTA);   
    init_alarm();

    //TODO there needs to be PWM config done for the front LEDs
   
   ESP_ERROR_CHECK( nvs_flash_init() );
   tcpip_adapter_init();
   ESP_ERROR_CHECK( esp_event_loop_create_default() );

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    
    obtain_time();    
    xTaskCreate(&lcd_tm1637_task, "lcd_tm1637_task", 4096, NULL, 5, NULL);
    static httpd_handle_t server = NULL;

    server = start_webserver();
}

