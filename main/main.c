#include <stdio.h>
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include <string.h>

#include "realizaHTTPRequest.h"
#include "led.h"

xSemaphoreHandle conexaoWifiSemaphoreHTTP, conexaoWifiSemaphoreLED,desconexaoWifi;


void app_main(void)
{
  inicializaLed();
  // Inicializa o NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
    ESP_ERROR_CHECK(ret);
    
    conexaoWifiSemaphoreHTTP = xSemaphoreCreateBinary();
    conexaoWifiSemaphoreLED = xSemaphoreCreateBinary();
    desconexaoWifi = xSemaphoreCreateBinary();
    xTaskCreate(&RealizaHTTPRequest,  "Processa HTTP", 4096*2, NULL, 2, NULL);
    xTaskCreate(&piscaLed, "PiscaLed ", 4096, NULL, 2, NULL);
    wifi_start();

}
