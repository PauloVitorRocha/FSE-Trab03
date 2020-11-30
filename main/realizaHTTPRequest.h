#ifndef REALIZA_HTTP_REQUEST_H
#define REALIZA_HTTP_REQUEST_H

#include <stdio.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "cJSON.h"
#include <string.h>

#include "wifi.h"
#include "http_client.h"

void RealizaHTTPRequest();

#endif