#ifndef LED_H
#define LED_H

#include <stdio.h>
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include <string.h>

void inicializaLed();
void piscaLed(void *params);

#endif