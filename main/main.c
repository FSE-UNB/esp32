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

void app_main(void)
{
    // DHT11_init(4);

    init_nvs();

    matricula = "0461";
    register_mac_addr();
    
    conexaoWifiSemaphore = xSemaphoreCreateBinary();

    xTaskCreate(&wifi_connection, "Conexão com mqtt", 4096, NULL, 1, NULL);
    
    init_button();

    xTaskCreate(&button_interruption, "Interrupções do botão", 4096, NULL, 1, NULL);

    init_led();

    while(1) {
        printf("LED 100%%\n");
        set_led_brightness(1.0);
        printf("LED 50%%\n");
        set_led_brightness(0.5);
        printf("LED 0%%\n");
        set_led_brightness(0);

        //  Read DHT11 
        // printf("Reading DHT11\n");
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // struct dht11_reading s = DHT11_read();
        // printf("Status = %d | Temp: %d | Um: %d\n", s.status, s.temperature, s.humidity);
    }

}