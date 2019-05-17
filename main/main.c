#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "tm1637.h"
#include "esp_sntp.h"
#include "protocol_examples_common.h"


static const char *TAG = "example";

//const gpio_num_t LCD_CLK = CONFIG_TM1637_CLK_PIN;
//const gpio_num_t LCD_DTA = CONFIG_TM1637_DIO_PIN;
const gpio_num_t LCD_CLK = 26;
const gpio_num_t LCD_DTA = 27;


static void obtain_time(void);
static void initialize_sntp(void);

void time_sync_notification_cb(struct timeval *tv)
{
   // ESP_LOGI(TAG, "Notification of a time synchronization event");
}
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}
void lcd_tm1637_task(void * arg) {
  	tm1637_led_t * lcd = tm1637_init(LCD_CLK, LCD_DTA);

	setenv("TZ", "EST+5EDT,M3.2.0/03:00:00,M11.1.0/02:00:00", 1);
	tzset();
	int toggle = 0;

        tm1637_set_brightness(lcd, 1);
	
	while (true)
	{
/*
		// Test segment control
		uint8_t seg_data[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
		for (uint8_t x=0; x<32; ++x)
		{
			uint8_t v_seg_data = seg_data[x%6];
			tm1637_set_segment_raw(lcd, 0, v_seg_data);
			tm1637_set_segment_raw(lcd, 1, v_seg_data);
			tm1637_set_segment_raw(lcd, 2, v_seg_data);
			tm1637_set_segment_raw(lcd, 3, v_seg_data);
			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		// Test brightness
		for (int x=0; x<7; x++) {
			tm1637_set_brightness(lcd, x);
			tm1637_set_number(lcd, 8888);
			vTaskDelay(300 / portTICK_RATE_MS);
		}
*/
	//	for (uint8_t x=0; x<3; ++x)
	//	{
			// Set random system time
			//struct timeval tm_test = {1517769863 + (x*3456), 0};
			//settimeofday(&tm_test, NULL);

			// Get current system time
			time_t now = 0;
			struct tm timeinfo = { 0 };
			time(&now);
			localtime_r(&now, &timeinfo);
			int time_number = 100 * timeinfo.tm_hour + timeinfo.tm_min;

			// Display time with blinking dots
			tm1637_set_number_lead_dot(lcd, time_number, true, toggle++%2 ? 0xFF : 0x00);
		        vTaskDelay(500 / portTICK_RATE_MS);
	//	}

/*		// Test display numbers
		for (int x=0; x<16; ++x) {
			bool show_dot = x%2; // Show dot every 2nd cycle
			tm1637_set_segment_number(lcd, 0, x, show_dot);
			tm1637_set_segment_number(lcd, 1, x, show_dot); // On my display "dot" (clock symbol ":") connected only here
			tm1637_set_segment_number(lcd, 2, x, show_dot);
			tm1637_set_segment_number(lcd, 3, x, show_dot);
			vTaskDelay(100 / portTICK_RATE_MS);
		}*/
	}  
}

static void obtain_time(void)
{
   // ESP_ERROR_CHECK( nvs_flash_init() );
   // tcpip_adapter_init();
   // ESP_ERROR_CHECK( esp_event_loop_create_default() );

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
  //  ESP_ERROR_CHECK(example_connect());

    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        //ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);

 //   ESP_ERROR_CHECK( example_disconnect() );
}

static void initialize_sntp(void)
{
    //ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}


void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
    obtain_time();    
    xTaskCreate(&lcd_tm1637_task, "lcd_tm1637_task", 4096, NULL, 5, NULL);
//    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
//    int level = 0;
//    while (true) {
//        gpio_set_level(GPIO_NUM_4, level);
//        level = !level;
//        vTaskDelay(300 / portTICK_PERIOD_MS);
//    }

}

