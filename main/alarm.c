/**
 * This sets up PWM on the ESP32, and uses the LEDC PWM as the input for an amplifier. This generates the alarm on the alarm clock.
 */
#include "alarm.h"

// Store config for alarm
ledc_channel_config_t alarm_conf = {
    .channel = LEDC_LS_CH3_CHANNEL,
    .duty = 0,
    .gpio_num = LEDC_LS_CH3_GPIO,
    .speed_mode = LEDC_HS_MODE,
    .hpoint = 0,
    .timer_sel = LEDC_HS_TIMER};

// Set the config for the timer.
ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_1_BIT, // resolution of PWM duty, fixed at 50%
    .freq_hz = NOTE_A4,                  // frequency of PWM signal
    .speed_mode = LEDC_HS_MODE,          // timer mode
    .timer_num = LEDC_HS_TIMER           // timer index
};

void init_alarm()
{

    // Set the config for the timer.
    // ledc_timer_config_t ledc_timer = {
    //     .duty_resolution = LEDC_TIMER_1_BIT, // resolution of PWM duty
    //     .freq_hz = NOTE_A4,              // frequency of PWM signal
    //     .speed_mode = LEDC_HS_MODE,           // timer mode
    //     .timer_num = LEDC_HS_TIMER            // timer index
    // };
    ledc_timer_config(&ledc_timer);

    // // Set up channel 3 to use pins and conf
    // ledc_channel_config_t alarm_conf = {
    //     .channel = LEDC_LS_CH3_CHANNEL,
    //     .duty = 0,
    //     .gpio_num = LEDC_LS_CH3_GPIO,
    //     .speed_mode = LEDC_HS_MODE,
    //     .hpoint = 0,
    //     .timer_sel = LEDC_HS_TIMER};

    ledc_channel_config(&alarm_conf);

    // This determines if the amp is turned on or not, turn off to start
    gpio_set_direction(ALARM_ACT_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(ALARM_ACT_PIN, 0);
}

/**
 * Activate the alarm with the configured frequency
 */
esp_err_t alarm_activate(alarm_freq_t freq)
{
    // Turn pin on that turns on amp
    gpio_set_level(ALARM_ACT_PIN, 1);

    // Update the timer frequency
    ledc_timer.freq_hz = freq;
    ledc_timer_config(&ledc_timer);

    // I don't think I actually need to do any of this
    esp_err_t set_status = ledc_set_duty(alarm_conf.speed_mode, alarm_conf.channel, 1);
    if (set_status == ESP_OK)
    {
        return ledc_update_duty(alarm_conf.speed_mode, alarm_conf.channel);
    }
    else
        return ESP_ERR_INVALID_STATE;
}

/**
 * Deactivate the alarm / turn off amp
 */
esp_err_t alarm_deactivate()
{
    gpio_set_level(ALARM_ACT_PIN, 0);
    return ESP_OK;
}
