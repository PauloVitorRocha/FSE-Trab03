#include "realizaHTTPRequest.h"

#define tamanhoMaxBUFFER 1024
#define IP_STACK_KEY CONFIG_IP_STACK_ACCESS_KEY
#define OPEN_WEATHER_MAP_KEY CONFIG_OPEN_WEATHER_MAP_ACCESS_KEY

extern xSemaphoreHandle conexaoWifiSemaphoreHTTP;

char *buffer;

int tamanhoBuffer = 0;

void RealizaHTTPRequest(void *params)
{
    while (true)
    {
        if (xSemaphoreTake(conexaoWifiSemaphoreHTTP, portMAX_DELAY))
        {
            if (tamanhoBuffer == 0)
            {
                buffer = (char *)malloc(tamanhoMaxBUFFER);
                if (buffer == NULL)
                {
                    ESP_LOGE("malloc", "Error ao alocar memoria");
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    continue;
                }
            }
            char url[300] = "http://api.ipstack.com/177.18.230.116?access_key=";
            strcat(url, IP_STACK_KEY);
            strcat(url, "&fields=latitude,longitude");

            http_request(url);

            if (tamanhoBuffer > 0)
            {
                buffer[tamanhoBuffer] = '\0';
                cJSON *json = cJSON_Parse(buffer);
                double latitude = cJSON_GetObjectItemCaseSensitive(json, "latitude")->valuedouble;
                double longitude = cJSON_GetObjectItemCaseSensitive(json, "longitude")->valuedouble;
                cJSON_Delete(json);
                printf("%lf %lf\n", latitude, longitude);
                free(buffer);
                buffer = NULL;
                tamanhoBuffer = 0;
            }
        }
    }
}