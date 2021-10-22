#ifndef BUTTON_H
#define BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "mqtt.h"

#define BUTTON_GPIO 0

void init_button();
void button_interruption();

#endif
