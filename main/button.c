#include "button.h"

extern xQueueHandle button_queue;

int button_state = 0;
clock_t before, difference;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
    int pin = (int)args;
    xQueueSendFromISR(button_queue, &pin, NULL);
}

void init_button()
{
    button_queue = xQueueCreate(10, sizeof(int));
    // Configuração do pino do Botão
    gpio_pad_select_gpio(BUTTON_GPIO);
    // Configura o pino do Botão como Entrada
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    // Configura o resistor de Pulldown para o botão (por padrão a entrada estará em Zero)
    gpio_pulldown_en(BUTTON_GPIO);
    // Desabilita o resistor de Pull-up por segurança.
    gpio_pullup_dis(BUTTON_GPIO);
    gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_ANYEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, gpio_isr_handler, (void *)BUTTON_GPIO);
   
}

void button_interruption()
{
    int pin;

    while (true)
    {
        if (xQueueReceive(button_queue, &pin, portMAX_DELAY))
        {
            // De-bouncing
            int state = gpio_get_level(pin);

            if (state == 0) {
                before = clock();
            }
            if (state == 1) {
                difference = clock() - before;
                int msec = difference / CLOCKS_PER_SEC;

                printf("\nBotão foi apertado por %d segundos\n", msec);

                if (msec < 3) {
                    button_state =! button_state;

                    char* value;
                    
                    asprintf(&value, "%d", button_state);
                    send_message_to_topic(value, "estado");
                } else {
                    printf("Reset\n");
                    send_ready_config_message();
                }
            }
        }
    }
}
