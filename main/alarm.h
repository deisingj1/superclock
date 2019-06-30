#include "driver/ledc.h"

#define ALARM_ACT_PIN 21
#define ALARM_TONE_PIN 4

#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_LS_CH3_GPIO       ALARM_TONE_PIN
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

typedef uint32_t alarm_freq_t;

#define NOTE_A4 440

void init_alarm();
esp_err_t alarm_activate(alarm_freq_t freq);
esp_err_t alarm_deactivate();