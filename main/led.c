#include "led.h"

#define LED 2

extern xSemaphoreHandle conexaoWifiSemaphoreLED, desconexaoWifi;

void inicializaLed(){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    gpio_set_level(LED, 0);
}


void piscaLed(void *params)
{
    int estado = 0;
    int ledContinuaPiscando = 1;
    while (true)
    {

        if (xSemaphoreTake(conexaoWifiSemaphoreLED, 500 / portTICK_PERIOD_MS))
        {
            gpio_set_level(LED, 1);
            ESP_LOGI("Task 2", "Escreve no display:");
            ledContinuaPiscando = 0;
        }
        else if (xSemaphoreTake(desconexaoWifi, 500 / portTICK_PERIOD_MS))
        {
            ESP_LOGI("Task 2.1", "desconectado o wifi");
            ledContinuaPiscando = 1;
        }
        else if (ledContinuaPiscando)
        {
            ESP_LOGI("Task 2.2", "led piscando");
            gpio_set_level(LED, estado);
            estado = !estado;
        }
    }
}
