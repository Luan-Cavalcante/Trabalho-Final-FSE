#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "esp_system.h"
#include "esp_log.h"
#include "esp_heap_caps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "sdkconfig.h"
#include "buzzer.h"

#define BUZZER_GPIO 23
#define GPIO_OUTPUT_SPEED LEDC_HIGH_SPEED_MODE

inline bool isDelay(struct Note note)
{
	return note.freq == DELAY_NOTE;
}

void sound(int gpio_num, uint32_t freq, uint32_t duration)
{
	ledc_timer_config_t timer_conf = {
		.speed_mode = GPIO_OUTPUT_SPEED,
		.duty_resolution = LEDC_TIMER_10_BIT,
		.timer_num = LEDC_TIMER_0,
		.freq_hz = freq,
		.clk_cfg = LEDC_AUTO_CLK};
	ledc_timer_config(&timer_conf);

	ledc_channel_config_t ledc_conf = {
		.gpio_num = gpio_num,
		.speed_mode = GPIO_OUTPUT_SPEED,
		.channel = LEDC_CHANNEL_0,
		// .intr_type  = LEDC_INTR_DISABLE,
		.timer_sel = LEDC_TIMER_0,
		.hpoint = 0,
		.duty = 0x0 // 50%=0x3FFF, 100%=0x7FFF for 15 Bit
					// 50%=0x01FF, 100%=0x03FF for 10 Bit
	};
	ledc_channel_config(&ledc_conf);

	// start
	ledc_set_duty(GPIO_OUTPUT_SPEED, LEDC_CHANNEL_0, 0x7F); // 12% duty - play here for your speaker or buzzer
	ledc_update_duty(GPIO_OUTPUT_SPEED, LEDC_CHANNEL_0);
	vTaskDelay(duration / portTICK_PERIOD_MS);
	// stop
	ledc_set_duty(GPIO_OUTPUT_SPEED, LEDC_CHANNEL_0, 0);
	ledc_update_duty(GPIO_OUTPUT_SPEED, LEDC_CHANNEL_0);
}

void play_note(struct Note note)
{
	if (isDelay(note))
		vTaskDelay(note.duration / portTICK_PERIOD_MS);
	else
		sound(BUZZER_GPIO, note.freq, note.duration);
}

void play_music(struct Music music)
{
	for (int i = 0; i < music.size; i++)
		play_note(music.notes[i]);
}

// based on https://wiki.mikrotik.com/wiki/Super_Mario_Theme
struct Music song_mario()
{
	static struct Note notes[] = { newNote(660, 100),newNote(DELAY_NOTE, 150),newNote(660, 100),newDelayNote(300),newNote(660, 100),newDelayNote(300),newNote(510, 100),newDelayNote(100),newNote(660, 100),newDelayNote(300),newNote(770, 100),newDelayNote(550),newNote(380, 100),newDelayNote(575),newNote(510, 100),newDelayNote(450),newNote(380, 100),newDelayNote(400),newNote(320, 100),newDelayNote(500),newNote(440, 100),newDelayNote(300),newNote(480, 80),newDelayNote(330),newNote(450, 100),newDelayNote(150),newNote(430, 100),newDelayNote(300),newNote(380, 100),newDelayNote(200),newNote(660, 80),newDelayNote(200),newNote(760, 50),newDelayNote(150),newNote(860, 100),newDelayNote(300),newNote(700, 80),newDelayNote(150),newNote(760, 50),newDelayNote(350),newNote(660, 80),newDelayNote(300),newNote(520, 80),newDelayNote(150),newNote(580, 80),newDelayNote(150),newNote(480, 80),newDelayNote(500) };
	static struct Music music = {
		.name = "Mario", .size = sizeof(notes)/sizeof(*notes),
		.notes = notes
	};
	return music;
}

// based on http://processors.wiki.ti.com/index.php/Playing_The_Imperial_March#Code
// original composed by John Williams for the film Star Wars: The Empire Strikes Back
struct Music song_star_wars()
{
	static struct Note notes[] = { newNote(440, 500),newNote(440, 500),newNote(440, 500),newNote(349, 350),newNote(523, 150),newNote(440, 500),newNote(349, 350),newNote(523, 150),newNote(440, 650),newDelayNote(150),newNote(659, 500),newNote(659, 500),newNote(659, 500),newNote(698, 350),newNote(523, 150),newNote(415, 500),newNote(349, 350),newNote(523, 150),newNote(440, 650),newDelayNote(150),newNote(880, 500),newNote(440, 300),newNote(440, 150),newNote(880, 400),newNote(830, 200),newNote(784, 200),newNote(740, 125),newNote(698, 125),newNote(740, 250),newDelayNote(250),newNote(455, 250),newNote(622, 400),newNote(587, 200),newNote(554, 200),newNote(523, 125),newNote(466, 125),newNote(523, 250),newDelayNote(250),newNote(349, 125),newNote(415, 500),newNote(349, 375),newNote(440, 125),newNote(523, 500),newNote(440, 375),newNote(523, 125),newNote(659, 650) };
	static struct Music music = {
		.name = "Star Wars", .size = sizeof(notes)/sizeof(*notes),
		.notes = notes
	};
	return music;
}

struct Music song_got()
{
	static struct Note notes[] = { newNote(440, 700),newNote(294, 500),newNote(349, 300),newNote(392, 250),newNote(440, 250),newNote(294, 300),newNote(349, 200),newNote(392, 200),newNote(330, 700),newDelayNote(200),newNote(392, 500),newNote(262, 500),newNote(349, 200),newNote(330, 200),newNote(392, 200),newNote(262, 500),newNote(349, 200),newNote(330, 200),newNote(294, 500) };
	static struct Music music = {
		.name = "Game of Thrones", .size = sizeof(notes)/sizeof(*notes),
		.notes = notes
	};
	return music;
}

/* Exemplo de como usar o Buzzer */

// void gpio_task(void *pvParameters)
// {
// 	struct Music musics[] = {song_got(), song_star_wars(), song_mario()};
// 	int size = sizeof(musics)/sizeof(*musics);
// 	while (true)
// 	{
// 		ESP_LOGI("BUZZER", "Comecando a playlist");
// 		for (int i = 0; i < size; i++)
// 		{
// 			ESP_LOGI(musics[i].name, "Tocando a musica %s", musics[i].name);
// 			play_music(musics[i]);
// 			ESP_LOGI(musics[i].name, "Finalizou a musica");
// 			vTaskDelay(5000 / portTICK_PERIOD_MS);
// 		}
// 		ESP_LOGI("BUZZER", "Fim da playlist");
// 	}

// 	vTaskDelete(NULL);
// }

// void app_main()
// {
// 	ESP_LOGI("MAIN", "free DRAM %u IRAM %u", (unsigned)esp_get_free_heap_size(), (unsigned)heap_caps_get_free_size(MALLOC_CAP_32BIT));
// 	xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 5, NULL);
// }
