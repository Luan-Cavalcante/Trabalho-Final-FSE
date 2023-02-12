#ifndef STATE_H__
#define STATE_H__

#include "stdbool.h"

struct State
{
    bool buzzerOn, headlightOn, lowPowerMode;
    int temperature, humidity, light, obstacle;
};

struct State *getState();

#endif
