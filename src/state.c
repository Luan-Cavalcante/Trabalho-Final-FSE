#include "state.h"

struct State *getState()
{
    static struct State state = {
        .buzzerOn = false,
        .headlightOn = false,
        .lowPowerMode = false,
        .temperature = 0,
        .humidity = 0,
        .light = 0,
        .obstacle = 0
    };
    return &state;
}
