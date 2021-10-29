/* MQTT (over TCP) Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt.h"
#include "my_nvs.h"

static const char *TAG = "MQTT";

char* device_topic;
char* temp_topic;
char* hum_topic;
char* state_topic;
char* place;

esp_mqtt_client_handle_t client;

static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

void send_message_to_topic(char *value, char *topic_type) {
    if (strlen(place) == 0) return;

    char* topic;
    char* message;

    asprintf(&topic, "fse2021/%s/%s/%s", matricula, place, topic_type);
    asprintf(&message, "{ \"esp_id\": \"%s\", \"value\": %s }", mac_str, value);

    printf("topic: %s\n", topic);
    printf("message: %s\n", message);

    esp_mqtt_client_publish(client, topic, message, 0, 1, 0);
}

void send_ready_config_message() {
    bzero(place, strlen(place));
    esp_mqtt_client_publish(client, device_topic, mac_str, 0, 1, 0);
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    client = event->client;
    int msg_id;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            asprintf(&device_topic, "fse2021/%s/dispositivos/%s", matricula, mac_str);

            place = get_nvs_value();
            printf("place %s\n", place);

            if (strlen(place) == 0) {
                msg_id = esp_mqtt_client_publish(client, device_topic, mac_str, 0, 1, 0);
                ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            }

            msg_id = esp_mqtt_client_subscribe(client, device_topic, 0);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            char* event_topic;
            asprintf(&event_topic, "%.*s", event->topic_len, event->topic);
            char* event_data;
            asprintf(&event_data, "%.*s", event->data_len, event->data);

            JSONBody json = parser_json(event_data);

            printf("\n\n%s\n", event_data);
            printf("%s\n", json.type);
            printf("%s\n", json.place);
            printf("%.1f\n\n", json.value);

            if (strcmp(json.type, "config") == 0) {
                set_nvs_value(json.place);
                place = json.place;
            } else if (strcmp(json.type, "output") == 0) {
                if (strlen(place) > 0){
                    set_led_brightness(json.value);                
                }
            } else if (strcmp(json.type, "unconfig") == 0) {
                bzero(place, strlen(place));
                esp_mqtt_client_publish(client, device_topic, mac_str, 0, 1, 0);
            }

            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtt://broker.hivemq.com",
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}
