// Teste do Sensor detector de som KY-038

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void app_main(void)
{
    // Pino do KY-038 conectado ao pino 14 na ESP32
    const int sensorPin = 14;
    // Pino para o LED na ESP32
    const int ledPin = 2;

    // Configuração do pino do sensor
    gpio_config_t sensor_conf;
    sensor_conf.intr_type = GPIO_INTR_DISABLE;
    sensor_conf.mode = GPIO_MODE_INPUT;
    sensor_conf.pin_bit_mask = (1ULL << sensorPin);
    sensor_conf.pull_down_en = 0;
    sensor_conf.pull_up_en = 0;
    gpio_config(&sensor_conf);

    // Configuração do pino do LED
    gpio_config_t led_conf;
    led_conf.intr_type = GPIO_INTR_DISABLE;
    led_conf.mode = GPIO_MODE_OUTPUT;
    led_conf.pin_bit_mask = (1ULL << ledPin);
    led_conf.pull_down_en = 0;
    led_conf.pull_up_en = 0;
    gpio_config(&led_conf);

    while (1)
    {
        int sensorValue = gpio_get_level(sensorPin); // Lê o valor do KY-038

        // Se o sensor detectar som, acende o LED da ESP
        if (sensorValue == 1)
        {
            printf("Som detectado!\n");
            gpio_set_level(ledPin, 1);
        }
        else
        {
            printf("Sem som detectado.\n");
            gpio_set_level(ledPin, 0);
        }

        // Aguarda meio segundo antes de ler novamente o valor do sensor
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
