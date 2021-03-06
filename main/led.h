#ifndef LED_H
#define LED_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (2) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY_FORCE         (8191) // ((2 ** 13) - 1) = 8191
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

void init_led();
void set_led_brightness(float brightness);

#endif