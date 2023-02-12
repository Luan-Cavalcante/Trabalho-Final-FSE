#include "driver/gpio.h"

#include "car.h"

#define ENGINE1_EN 12
#define ENGINE1_FIRST 14
#define ENGINE1_SECOND 27

#define ENGINE2_EN 33
#define ENGINE2_FIRST 26
#define ENGINE2_SECOND 25

void engineInit(struct L298NEngine engine)
{
  esp_rom_gpio_pad_select_gpio(engine.gpio_en);
  esp_rom_gpio_pad_select_gpio(engine.gpio1);
  esp_rom_gpio_pad_select_gpio(engine.gpio2);

  gpio_set_direction(engine.gpio_en, GPIO_MODE_OUTPUT);
  gpio_set_direction(engine.gpio1, GPIO_MODE_OUTPUT);
  gpio_set_direction(engine.gpio2, GPIO_MODE_OUTPUT);

  gpio_set_level(engine.gpio_en, 0);
}

void engineSpin(struct L298NEngine engine, int direction)
{
  switch (direction)
  {
  case ENGINE_FORWARD:
    gpio_set_level(engine.gpio1,1);
    gpio_set_level(engine.gpio2,0);
    gpio_set_level(engine.gpio_en, 1);
    break;
  case ENGINE_BACKWARD:
    gpio_set_level(engine.gpio1,0);
    gpio_set_level(engine.gpio2,1);
    gpio_set_level(engine.gpio_en, 1);
    break;
  case ENGINE_BREAK:
    gpio_set_level(engine.gpio1,1);
    gpio_set_level(engine.gpio2,1);
    gpio_set_level(engine.gpio_en, 1);
    break;
  case ENGINE_DEAD:
    gpio_set_level(engine.gpio1,0);
    gpio_set_level(engine.gpio2,0);
    gpio_set_level(engine.gpio_en, 0);
    break;
  }
}

void carInit(struct Car car)
{
  engineInit(car.leftEngine);
  engineInit(car.rightEngine);
}

struct Car getCar()
{
  static struct Car car = {
    .leftEngine = {
      .gpio_en = ENGINE1_EN,
      .gpio1 = ENGINE1_FIRST,
      .gpio2 = ENGINE1_SECOND,
    },
    .rightEngine = {
      .gpio_en = ENGINE2_EN,
      .gpio1 = ENGINE2_FIRST,
      .gpio2 = ENGINE2_SECOND,
    }
  };
  return car;
}

void carMove(struct Car car, int direction)
{
  switch (direction)
  {
  case CAR_FORWARD:
    engineSpin(car.leftEngine, ENGINE_FORWARD);
    engineSpin(car.rightEngine, ENGINE_FORWARD);
    break;
  case CAR_BACKWARD:
    engineSpin(car.leftEngine, ENGINE_BACKWARD);
    engineSpin(car.rightEngine, ENGINE_BACKWARD);
    break;
  case CAR_BREAK:
    engineSpin(car.leftEngine, ENGINE_BREAK);
    engineSpin(car.rightEngine, ENGINE_BREAK);
    break;
  case CAR_TURN_LEFT:
    engineSpin(car.leftEngine, ENGINE_DEAD);
    engineSpin(car.rightEngine, ENGINE_FORWARD);
    break;
  case CAR_TURN_RIGHT:
    engineSpin(car.leftEngine, ENGINE_FORWARD);
    engineSpin(car.rightEngine, ENGINE_DEAD);
    break;
  case CAR_IDLE:
    engineSpin(car.leftEngine, ENGINE_DEAD);
    engineSpin(car.rightEngine, ENGINE_DEAD);
  }
}
