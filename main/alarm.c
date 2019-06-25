#include "alarm.h"

ledc_channel_config_t alarm_conf = {
    .channel = LEDC_LS_CH3_CHANNEL,
    .duty = 0,
    .gpio_num = LEDC_LS_CH3_GPIO,
    .speed_mode = LEDC_LS_MODE,
    .hpoint = 0,
    .timer_sel = LEDC_LS_TIMER};

void init_alarm()
{

    // Set the config for the timer.
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_13_BIT, // resolution of PWM duty
        .freq_hz = TONE_A4_FREQ,              // frequency of PWM signal
        .speed_mode = LEDC_LS_MODE,           // timer mode
        .timer_num = LEDC_LS_TIMER            // timer index
    };

    // Set up channel 3 to use pins and conf
    ledc_channel_config_t alarm_conf = {
        .channel = LEDC_LS_CH3_CHANNEL,
        .duty = 0,
        .gpio_num = LEDC_LS_CH3_GPIO,
        .speed_mode = LEDC_LS_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_LS_TIMER};

    ledc_channel_config(&alarm_conf);
}

/**
 * Activate the alarm with the configured frequency
 */
esp_err_t alarm_activate()
{
    esp_err_t set_status = ledc_set_duty(alarm_conf.speed_mode, alarm_conf.channel, 2000);
    if(set_status == ESP_OK) {
        return ledc_update_duty(alarm_conf.speed_mode, alarm_conf.channel);
    } else return ESP_ERR_INVALID_STATE;
}
