#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "wifi.h"
#include "mqtt.h"
#include "dht11.h"
#include "car.h"
#include "state.h"
#include "buzzer.h"

#include "esp32/rom/uart.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/rtc_io.h"

#define LDR ADC1_CHANNEL_7

#define LED1 21
#define LED2 19
#define LED3 18
#define FAROL1 4
#define FAROL 2
#define SENSOR_P 36
#define BUZZER 23

#define BOTAO_BOOT 0

#define DHT11_PIN 5

SemaphoreHandle_t conexaoWifiSemaphore;
SemaphoreHandle_t conexaoMQTTSemaphore;

void conectadoWifi(void *params)
{
  while (true)
  {
    if (xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))
    {
      // Processamento Internet
      mqtt_start();
    }
  }
}

void sendStateToDashboard()
{
  char mensagem[256];
  struct State *state = getState();

  sprintf(mensagem, "{\"temperatura\": %d, \"umidade\": %d}", state->temperature, state->humidity);
  mqtt_envia_mensagem("v1/devices/me/telemetry", mensagem);

  sprintf(mensagem, "{\"luz\":%d,\"ledFarol\":%d,\"farolManual\":%d,\"lowpower\":%d,\"music\":%d,\"sleep\":%d}", state->light, state->headlightOn, state->headlightManual, state->lowPowerMode, state->musicOn, state->sleepMode);
  mqtt_envia_mensagem("v1/devices/me/attributes", mensagem);

  ESP_LOGI("State", "(buzzerOn=%d,headlightOn=%d,headlightManual=%d,lowPowerMode=%d)", state->buzzerOn, state->headlightOn, state->headlightManual, state->lowPowerMode);
  ESP_LOGI("State", "(temperature=%d,humidity=%d,light=%d,obstacle=%d,musicOn=%d,lowPowerMode=%d)", state->temperature, state->humidity, state->light, state->obstacle, state->musicOn, state->lowPowerMode);
}

void trataComunicacaoComServidor(void *params)
{
  if (xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))
  {
    while (true)
    {
      vTaskDelay(1000 / portTICK_PERIOD_MS);

      sendStateToDashboard();
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

  // seta para outputs
  gpio_set_direction(LED1, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED3, GPIO_MODE_OUTPUT);
  gpio_set_direction(FAROL1, GPIO_MODE_OUTPUT);
  gpio_set_direction(FAROL, GPIO_MODE_OUTPUT);

  gpio_set_direction(SENSOR_P, GPIO_MODE_INPUT);

  adc1_config_width(ADC_ATTEN_DB_6);
  adc1_config_channel_atten(LDR, ADC_ATTEN_DB_6);
}

void TrataGPIO()
{
  struct State *state = getState();

  while (true)
  {
    vTaskDelay(200 / portTICK_PERIOD_MS);

    if (state->lowPowerMode)
      continue;

    /* Atualizando Estado da aplicação */
    state->light = adc1_get_raw(LDR);
    state->obstacle = !gpio_get_level(SENSOR_P);

    struct dht11_reading dht11_value = DHT11_read();
    if (dht11_value.status == DHT11_OK)
    {
      state->temperature = dht11_value.temperature;
      state->humidity = dht11_value.humidity;
    }

    if (!state->headlightManual)
      state->headlightOn = (state->light < 400);

    /* Atualizando ESP32 */
    // luz
    if (state->headlightOn)
    {
      gpio_set_level(FAROL, 1);
      gpio_set_level(FAROL1, 1);
    }
    else
    {
      gpio_set_level(FAROL, 0);
      gpio_set_level(FAROL1, 0);
    }

    // buzzer
    if (state->buzzerOn)
      make_sound(850);
    else
      stop_sound();

    if (state->obstacle)
      timed_sound(657, 600);
  }
}

void light_sleep_mode()
{
  esp_rom_gpio_pad_select_gpio(BOTAO_BOOT);
  gpio_set_direction(BOTAO_BOOT, GPIO_MODE_INPUT);

  gpio_wakeup_enable(BOTAO_BOOT, GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();

  struct State *state = getState();
  while(true)
  {
    vTaskDelay(50);

    if (rtc_gpio_get_level(BOTAO_BOOT) == 0)
    {
        printf("Aguardando soltar o botão ... \n");
        do
        {
            vTaskDelay(pdMS_TO_TICKS(10));
        } while (rtc_gpio_get_level(BOTAO_BOOT) == 0);

        state->sleepMode = !state->sleepMode;
    }

    if (!state->sleepMode)
      continue;

    printf("Entrando em modo Light Sleep\n");

    sendStateToDashboard();

    // Configura o modo sleep somente após completar a escrita na UART para finalizar o printf
    uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    // Entra em modo Light Sleep
    esp_light_sleep_start();
  }
}

void playMusics()
{
  struct Music musics[] = {song_got(), song_star_wars(), song_mario()};
  int size = sizeof(musics)/sizeof(*musics);
	while (true)
	{
		ESP_LOGI("BUZZER", "Comecando a playlist");
		for (int i = 0; i < size; i++)
		{
			ESP_LOGI("BUZZER", "Tocando a musica %s", musics[i].name);
			play_music(musics[i]);
			ESP_LOGI("BUZZER", "Finalizou a musica");
			vTaskDelay(5000 / portTICK_PERIOD_MS);
		}
		ESP_LOGI("BUZZER", "Fim da playlist");
  }
}

void app_main(void)
{
  // Inicializa o NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
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
  // Inicializa buzzer
  timed_sound(456, 1);
  // Inicializa estado
  loadState();

  xTaskCreate(&TrataGPIO, "Comunicação com as GPIO", 4096, NULL, 1, NULL);
  xTaskCreate(&conectadoWifi, "Conexão ao MQTT", 4096, NULL, 1, NULL);
  xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
  xTaskCreate(&playMusics, "Tocar musica", 4096, NULL, 1, NULL);

  light_sleep_mode();
}
