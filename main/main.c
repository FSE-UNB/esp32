/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/ledc.h"
#include "esp_system.h"

#include "dht11.h"
#include "mqtt.h"
#include "wifi.h"
#include "global.h"
#include "my_nvs.h"
#include "button.h"

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 2
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (2) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz

xSemaphoreHandle conexaoWifiSemaphore;
xQueueHandle button_queue;

static void example_ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_DUTY_RES,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel);
}

void update_led(uint32_t duty) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

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
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */

    // DHT11_init(4);

    init_nvs();

    matricula = "0461";
    register_mac_addr();
    
    conexaoWifiSemaphore = xSemaphoreCreateBinary();

    example_ledc_init();

    xTaskCreate(&wifi_connection, "Conexão com mqtt", 4096, NULL, 1, NULL);
    
    init_button();

    xTaskCreate(&button_interruption, "Interrupções do botão", 4096, NULL, 1, NULL);

    // subscribe em fse2021/<matricula>/dispositivos/<ID_do_dispositivo>
    // publish em fse2021/<matricula>/dispositivos


    while(1) {
        // Blink off (output low) 
        printf("Turning off the LED\n");
        update_led(LEDC_DUTY);
        // Blink on (output high) 
        printf("Turning on the LED\n");
        update_led(LEDC_DUTY*2);
        // Blink on (output high) 
        printf("Turning on the LED\n");
        update_led(0);

        //  Read DHT11 
        // printf("Reading DHT11\n");
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // struct dht11_reading s = DHT11_read();
        // printf("Status = %d | Temp: %d | Um: %d\n", s.status, s.temperature, s.humidity);
    }

}