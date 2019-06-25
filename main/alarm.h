#include "driver/ledc.h"

#define ALARM_ACT_PIN 21
#define ALARM_TONE_PIN 4
#define TONE_A4_FREQ 440

#define LEDC_LS_MODE           LEDC_LOW_SPEED_MODE
#define LEDC_LS_TIMER          LEDC_TIMER_1
#define LEDC_LS_CH3_GPIO       ALARM_TONE_PIN
#define LEDC_LS_CH3_CHANNEL    LEDC_CHANNEL_3

void init_alarm();

