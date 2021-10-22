#include "freertos/FreeRTOS.h"

#include "dht11.h"
#include "mqtt.h"
#include "wifi.h"
#include "global.h"
#include "my_nvs.h"
#include "button.h"
#include "led.h"

xSemaphoreHandle conexaoWifiSemaphore;
xQueueHandle button_queue;

void register_mac_addr() {
    unsigned char mac[6] = {0};

    esp_efuse_mac_get_default(mac);
    asprintf(&mac_str, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
}

void wifi_connection(void* params) {
    wifi_start();

    while(1) {
        if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY)) {
            mqtt_start();
        }
    }
}

void read_avarage_dht11() {
    int count = 0;
    int count_success = 0;
    int temperature = 0;
    int humidity = 0;

    while (1) {
        struct dht11_reading s = DHT11_read();

        if (s.temperature >= 0 || s.humidity >= 0) {
            temperature += s.temperature;
            humidity += s.humidity;
            count_success++;

            printf("\n\nStatus (%d) = Temp: %d | Um: %d\n\n", count, s.temperature, s.humidity);
        }

        count++;

        if (count == 5) {
            char* message_temperature;
            char* message_humidity;
            
            asprintf(&message_temperature, "%.2f", (float) temperature / count_success);
            asprintf(&message_humidity, "%.2f", (float) humidity / count_success);

            send_message_to_topic(message_temperature, "temperatura");
            send_message_to_topic(message_humidity, "umidade");

            printf("Status = Temp: %s | Um: %s\n", message_temperature, message_humidity);

            count = 0;
            count_success = 0;
            temperature = 0;
            humidity = 0;
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    DHT11_init(4);

    init_nvs();

    matricula = "0461";
    register_mac_addr();
    
    conexaoWifiSemaphore = xSemaphoreCreateBinary();

    xTaskCreate(&wifi_connection, "Conexão com mqtt", 4096, NULL, 1, NULL);
    
    init_button();

    xTaskCreate(&button_interruption, "Interrupções do botão", 4096, NULL, 1, NULL);

    init_led();

    read_avarage_dht11();
}