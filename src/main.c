#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

#define JOYSTICK_X ADC1_CHANNEL_3
#define JOYSTICK_Y ADC1_CHANNEL_6
#define LDR ADC1_CHANNEL_7

#define JOYSTICK_BOTAO 36

#define LED1 21
#define LED2 19
#define LED3 18
#define LED4 4
#define FAROL 2

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
       float temperatura = 20.0 + (float)rand()/(float)(RAND_MAX/10.0);
       sprintf(mensagem, "{\"temperatura\": %f}", temperatura);
       mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

       sprintf(jsonatributos, "{\"luz\":%d}",luz);

       mqtt_envia_mensagem("v1/devices/me/attributes", jsonatributos);
       vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }
}

void TrataGPIO(){
  esp_rom_gpio_pad_select_gpio(JOYSTICK_BOTAO);
  esp_rom_gpio_pad_select_gpio(LED1);
  esp_rom_gpio_pad_select_gpio(LED2);
  esp_rom_gpio_pad_select_gpio(LED3);
  esp_rom_gpio_pad_select_gpio(LED4);
  esp_rom_gpio_pad_select_gpio(FAROL);

  // seta para outputs
  gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED4, GPIO_MODE_OUTPUT);
  gpio_set_direction(FAROL, GPIO_MODE_OUTPUT);

  // input do botão e as analógicas
  gpio_set_direction(JOYSTICK_BOTAO, GPIO_MODE_INPUT);
  gpio_pulldown_en(JOYSTICK_BOTAO);
  gpio_pullup_dis(JOYSTICK_BOTAO);

  // Configura o conversor AD
  adc1_config_width(ADC_WIDTH_BIT_10);

  adc1_config_channel_atten(JOYSTICK_X, ADC_ATTEN_DB_6);
  adc1_config_channel_atten(JOYSTICK_Y, ADC_ATTEN_DB_6);
  adc1_config_channel_atten(LDR, ADC_ATTEN_DB_6);

  while (true)
  {
    int posicao_x = adc1_get_raw(JOYSTICK_X);
    int posicao_y = adc1_get_raw(JOYSTICK_Y);
    luz = adc1_get_raw(LDR);

    int botao = gpio_get_level(JOYSTICK_BOTAO);

    posicao_x = posicao_x - 512;
    posicao_y = posicao_y - 512;
    //printf("Posição X: %.3d \t Posição Y: %.3d \t Luz : %d  |  Botão: %d\n", posicao_x, posicao_y, luz,botao);
    vTaskDelay(300 / portTICK_PERIOD_MS);

    if(posicao_x == 511)
    {
      gpio_set_level(LED1,1);
    }
    else if(posicao_x == -512)
    {
      gpio_set_level(LED2,1);
    }
    else{
      gpio_set_level(LED1,0);
      gpio_set_level(LED2,0);

    }

    if(posicao_y == 511)
    {
      gpio_set_level(LED4,1);
    }
    else if(posicao_y == -512)
    {
      gpio_set_level(LED3,1);
    }
    else{
      gpio_set_level(LED3,0);
      gpio_set_level(LED4,0);

    }

    // luz 
    if (luz < 200)
    {
      gpio_set_level(FAROL,1);
    }
    else{
      gpio_set_level(FAROL,0);
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

    //xTaskCreate(&TrataGPIO, "Comunicação com as GPIO", 4096, NULL, 1, NULL);
    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);

}

