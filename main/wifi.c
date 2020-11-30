#include "wifi.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#define WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD
#define WIFI_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define TAG "Wifi"

static EventGroupHandle_t s_wifi_event_group;

static int s_retry_num = 0;
extern xSemaphoreHandle conexaoWifiSemaphoreHTTP, conexaoWifiSemaphoreLED, desconexaoWifi, desconexaoWifiSemaphoreHTTP;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "disconnect");
        xSemaphoreGive(desconexaoWifi);
        xSemaphoreGive(desconexaoWifiSemaphoreHTTP);
        vTaskDelay(5 * 1000 / portTICK_PERIOD_MS);
        if (s_retry_num < WIFI_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",WIFI_SSID, WIFI_PASS);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Endereço IP recebido:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",WIFI_SSID, WIFI_PASS);
        xSemaphoreGive(conexaoWifiSemaphoreHTTP);
        xSemaphoreGive(conexaoWifiSemaphoreLED);
    
    }
}


void disconnectManual(void *params){

    while(true){
        vTaskDelay(20*1000/ portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "entrei no disconect2");
        esp_wifi_disconnect();
    }

}

void wifi_start(){

    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    //xTaskCreate(&disconnectManual, "Disconecta ", 4096, NULL, 2, NULL);
   
   
}

void wifi_stop();