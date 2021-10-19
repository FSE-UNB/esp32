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
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/ledc.h"

#include "dht11.h"

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
#define BOTAO 0

void trataInterrupcaoBotao();

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

xQueueHandle filaDeInterrupcao;
int clickBotao=0;

void update_led(uint32_t duty) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pino = (int)args;
    xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void initializaBotao()
{
    filaDeInterrupcao = xQueueCreate(10, sizeof(int));
    // Configuração do pino do Botão
    gpio_pad_select_gpio(BOTAO);
    // Configura o pino do Botão como Entrada
    gpio_set_direction(BOTAO, GPIO_MODE_INPUT);
    // Configura o resistor de Pulldown para o botão (por padrão a entrada estará em Zero)
    gpio_pulldown_en(BOTAO);
    // Desabilita o resistor de Pull-up por segurança.
    gpio_pullup_dis(BOTAO);
    gpio_set_intr_type(BOTAO, GPIO_INTR_POSEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOTAO, gpio_isr_handler, (void *)BOTAO);
   
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

    // example_ledc_init();
    
    // initializaBotao();

    while(1) {
        // trataInterrupcaoBotao();

        /* Blink off (output low) */
        // printf("Turning off the LED\n");
        // update_led(LEDC_DUTY);
        /* Blink on (output high) */
        // printf("Turning on the LED\n");
        // update_led(LEDC_DUTY*2);
        /* Blink on (output high) */
        // printf("Turning on the LED\n");
        // update_led(0);

        // /* Read DHT11 */
        // printf("Reading DHT11\n");
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        // struct dht11_reading s = DHT11_read();
        // printf("Status = %d | Temp: %d | Um: %d\n", s.status, s.temperature, s.humidity);
    }
}

void trataInterrupcaoBotao()
{
    int pino;

    while (true)
    {
        if (xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY))
        {
            // De-bouncing
            int estado = gpio_get_level(pino);
            if (estado == 1)
            {
                gpio_isr_handler_remove(pino);
                while (gpio_get_level(pino) == estado)
                {
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }

                clickBotao=!clickBotao;
                
                printf("BUTTON\n");
                // Habilitar novamente a interrupção
                vTaskDelay(50 / portTICK_PERIOD_MS);
                gpio_isr_handler_add(pino, gpio_isr_handler, (void *)pino);
            }
        }
    }
}