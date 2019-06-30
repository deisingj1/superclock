#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "esp_log.h"
#include <string.h>

#include "nvs_flash.h"
#include "driver/gpio.h"
#include "tm1637.h"

#include "pin_names.h"

#include "alarm.h"
#include "sntp_connect.h"
#include "webui.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static const char *TAG_MAIN = "main";

int time_set = 0;

//GLOBAL STATES FOR INDICATORS
volatile int led_br_on = 0;
volatile int led_bl_on = 0;
volatile int led_tl_on = 0;

//this is the led driver
tm1637_led_t *lcd;
int disp_brightness = 4;

void lcd_tm1637_task(void *arg)
{

    int toggle = 0;

    while (true)
    {
        tm1637_set_brightness(lcd, disp_brightness);
        time_t now = 0;
        struct tm timeinfo = {0};
        time(&now);
        localtime_r(&now, &timeinfo);
        int time_number = 100 * timeinfo.tm_hour + timeinfo.tm_min;

        // Display time with blinking dots
        tm1637_set_number_lead_dot(lcd, time_number, true, toggle++ % 2 ? 0xFF : 0x00);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

void app_main(void)
{
    // Set up our hardware, get everything booted
    lcd = tm1637_init(LCD_CLK, LCD_DTA);
    init_alarm();

    //TODO there needs to be PWM config done for the front LEDs

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    alarm_activate(NOTE_A4);
    logout_time();
    alarm_deactivate();
    start_webserver();
    xTaskCreate(&lcd_tm1637_task, "lcd_tm1637_task", 4096, NULL, 5, NULL);
}
