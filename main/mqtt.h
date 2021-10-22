#ifndef MQTT_H_
#define MQTT_H_

#include "global.h"
#include "parser.h"
#include "led.h"

void mqtt_start(void);
void send_message_to_topic(char *message, char *topic);

#endif