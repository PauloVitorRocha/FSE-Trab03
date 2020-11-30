#ifndef LED_H
#define LED_H

#include <stdio.h>
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include <string.h>

void inicializaLed();
void Led(void *params);
void piscaLed();

#endif