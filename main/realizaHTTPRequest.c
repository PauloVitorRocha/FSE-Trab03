#include "realizaHTTPRequest.h"

#define tamanhoMaxBUFFER 1024
#define IP_STACK_KEY CONFIG_IP_STACK_ACCESS_KEY
#define OPEN_WEATHER_MAP_KEY CONFIG_OPEN_WEATHER_MAP_ACCESS_KEY

extern xSemaphoreHandle conexaoWifiSemaphoreHTTP;

char *buffer;

int tamanhoBuffer = 0;

double latitude = -500.0, longitude = -500.0;
double temperaturaAtual = -500.0, temperaturaMinima = -500.0, temperaturaMaxima = -500.0;
int umidade = -500.0;

void alocaMemoriaBuffer(){

    if (tamanhoBuffer == 0)
    {
        buffer = (char *)malloc(tamanhoMaxBUFFER);
        if (buffer == NULL)
        {
            ESP_LOGE("malloc", "Error ao alocar memoria");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void parserJsonLatitudeLongitude()
{
    if (tamanhoBuffer > 0)
    {
        buffer[tamanhoBuffer] = '\0';
        cJSON *jsonLocalizacao = cJSON_Parse(buffer);
        latitude = cJSON_GetObjectItemCaseSensitive(jsonLocalizacao, "latitude")->valuedouble;
        longitude = cJSON_GetObjectItemCaseSensitive(jsonLocalizacao, "longitude")->valuedouble;
        cJSON_Delete(jsonLocalizacao);
        printf("%lf %lf\n\n", latitude, longitude);
        free(buffer);
        buffer = NULL;
        tamanhoBuffer = 0;
    }
}

void imprimeVariaveisTempo()
{
    if (temperaturaMinima > -500 && temperaturaAtual > -500 && temperaturaMaxima > -500 && umidade > -500)
    {
        printf("Temperatura Atual = %lf\n", temperaturaAtual);
        printf("Temperatura mínima que pode chegar= %lf\n", temperaturaMinima);
        printf("Temperatura máxima que pode chegar = %lf\n", temperaturaMaxima);
        printf("Umidade = %d%%\n", umidade);
    }
}

void parserJsonTempo()
{
       
    if (tamanhoBuffer > 0)
    {
        buffer[tamanhoBuffer] = '\0';
        cJSON *jsonTempo = cJSON_Parse(buffer);
        cJSON *campoMain = cJSON_GetObjectItemCaseSensitive(jsonTempo, "main");
        temperaturaAtual = cJSON_GetObjectItemCaseSensitive(campoMain, "temp")->valuedouble;
        temperaturaMinima = cJSON_GetObjectItemCaseSensitive(campoMain, "temp_min")->valuedouble;
        temperaturaMaxima = cJSON_GetObjectItemCaseSensitive(campoMain, "temp_max")->valuedouble;
        umidade = cJSON_GetObjectItemCaseSensitive(campoMain, "humidity")->valueint;
        cJSON_Delete(jsonTempo);
        free(buffer);
        buffer = NULL;
        tamanhoBuffer = 0;
    }
    
}

void RealizaHTTPRequest(void *params)
{
    
   
    while (true)
    {
        if (xSemaphoreTake(conexaoWifiSemaphoreHTTP, portMAX_DELAY))
        {
            alocaMemoriaBuffer();
            char url[300];
            sprintf(url, "http://api.ipstack.com/check?access_key=%s&fields=latitude,longitude",IP_STACK_KEY);
            http_request(url);
            parserJsonLatitudeLongitude();

            alocaMemoriaBuffer();
            if (latitude > -500 && longitude > -500)
            {
                sprintf(url, "http://api.openweathermap.org/data/2.5/weather?lat=%lf&lon=%lf&appid=%s&units=metric", latitude, longitude, OPEN_WEATHER_MAP_KEY);
                http_request(url);
                parserJsonTempo();
                imprimeVariaveisTempo();
            }
        }
    }
}