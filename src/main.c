#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"
#include "dht11.h"
#include "car.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define LDR ADC1_CHANNEL_7

#define LED1 21
#define LED2 19
#define LED3 18
#define FAROL1 4
#define FAROL 2
#define SENSOR_P 36
#define BUZZER 23

#define DHT11_PIN 5

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

int luz = 0;

void conectadoWifi(void * params)
{
  while(true)
  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void trataComunicacaoComServidor(void * params)
{
  char mensagem[50];
  char jsonatributos[200];

  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while(true)
    {
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      struct dht11_reading value = DHT11_read();
      if (value.status != DHT11_OK)
      {
        ESP_LOGD("Comunicacao Servidor", "Falha ao ler o DHT11");
        continue;
      }

      sprintf(mensagem, "{\"temperatura\": %d, \"umidade\": %d}", value.temperature, value.humidity);
      mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

      sprintf(jsonatributos, "{\"luz\":%d}",luz);

      mqtt_envia_mensagem("v1/devices/me/attributes", jsonatributos);
      ESP_LOGI("Comunicacao Servidor", "%s", mensagem);
    }
  }
}

void initGPIO()
{
  esp_rom_gpio_pad_select_gpio(LED1);
  esp_rom_gpio_pad_select_gpio(LED2);
  esp_rom_gpio_pad_select_gpio(LED3);
  esp_rom_gpio_pad_select_gpio(FAROL1);
  esp_rom_gpio_pad_select_gpio(SENSOR_P);
  esp_rom_gpio_pad_select_gpio(FAROL);
  // esp_rom_gpio_pad_select_gpio(BUZZER);


  // seta para outputs
  gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
  gpio_set_direction(FAROL1, GPIO_MODE_OUTPUT);
  gpio_set_direction(FAROL, GPIO_MODE_OUTPUT);
  // gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);

  // gpio_set_level(BUZZER, 1);

  gpio_set_direction(SENSOR_P, GPIO_MODE_INPUT);

  adc1_config_width(ADC_ATTEN_DB_6);
  adc1_config_channel_atten(LDR, ADC_ATTEN_DB_6);
}

void TrataGPIO(){
  while (true)
  {
    luz = adc1_get_raw(LDR);
    int obstaculo = gpio_get_level(SENSOR_P);

    printf("Luz : %d presença : %d\n", luz,obstaculo);

    vTaskDelay(100 / portTICK_PERIOD_MS);
    // luz 
    if (luz < 200)
    {
      gpio_set_level(FAROL,1);
      gpio_set_level(FAROL1,1);
    }
    else{
      gpio_set_level(FAROL,0);
      gpio_set_level(FAROL1,0);
    }
  }
}

void app_main(void)
{
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();
    wifi_start();

    // Inicializa GPIO
    initGPIO();
    // Inicializa o carrinho
    carInit(getCar());
    // Inicializa DHT11
    DHT11_init(DHT11_PIN);

    xTaskCreate(&TrataGPIO, "Comunicação com as GPIO", 4096, NULL, 1, NULL);
    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    // xTaskCreate(&moveCar, "Movimentação do carrinho", 4096, NULL, 1, NULL);

}

