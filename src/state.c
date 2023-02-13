#include "nvs_flash.h"
#include "esp_log.h"

#include "state.h"

struct State *getState()
{
    static struct State state = {
        .buzzerOn = false,
        .headlightOn = false,
        .headlightManual = false,
        .lowPowerMode = false,
        .sleepMode = false,
        .musicOn = false,
        .temperature = 0,
        .humidity = 0,
        .light = 0,
        .obstacle = 0
    };
    return &state;
}

void saveState()
{
    ESP_ERROR_CHECK(nvs_flash_init());

    nvs_handle handle;
    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READWRITE, &handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace não encontrado");
        return;
    }

    struct State *state = getState();
#define SAVE_NVS_FIELD(__name) if ((res_nvs = nvs_set_i32(handle, #__name, (int32_t)state->__name)) != ESP_OK) ESP_LOGE("NVS", "Não foi possível escrever (%s)", esp_err_to_name(res_nvs));
    SAVE_NVS_FIELD(headlightManual);
    SAVE_NVS_FIELD(lowPowerMode);
    SAVE_NVS_FIELD(musicOn);
    SAVE_NVS_FIELD(headlightOn);
#undef SAVE_NVS_FIELD

    nvs_commit(handle);
    nvs_close(handle);
}

void loadState()
{
    ESP_ERROR_CHECK(nvs_flash_init());

    nvs_handle handle;
    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &handle);

    if (res_nvs == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGE("NVS", "Namespace não encontrado");
        return;
    }

    struct State *state = getState();
#define LOAD_NVS_FIELD(name) if ((res_nvs = nvs_get_i32(handle, #name, (int32_t*)&state->name)) != ESP_OK) ESP_LOGE("NVS", "Não foi possível ler (%s)", esp_err_to_name(res_nvs));
    LOAD_NVS_FIELD(headlightManual);
    LOAD_NVS_FIELD(lowPowerMode);
    LOAD_NVS_FIELD(musicOn);
    LOAD_NVS_FIELD(headlightOn);
#undef LOAD_NVS_FIELD

    nvs_close(handle);
}
