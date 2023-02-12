// Teste do Sensor Infravermelho Reflexivo de Obstáculo KY-032

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define KY_032_PIN 32

void setup()
{
    gpio_pad_select_gpio(KY_032_PIN);
    gpio_set_direction(KY_032_PIN, GPIO_MODE_INPUT);
    Serial.begin(115200);
}

void loop()
{
    int sensorValue = gpio_get_level(KY_032_PIN);

    if (sensorValue == 1)
    {
        Serial.println("Obstáculo detectado");
    }
    else
    {
        Serial.println("Não há obstáculo");
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
}

void app_main(void)
{
    setup();

    while (true)
    {
        loop();
    }
}
