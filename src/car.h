#ifndef CAR_H__
#define CAR_H__

struct L298NEngine
{
    int gpio_en, gpio1, gpio2;
};

enum EngineDirection
{
    ENGINE_FORWARD, ENGINE_BACKWARD, 
    ENGINE_BREAK, ENGINE_DEAD
};

void engineInit(struct L298NEngine);
void engineSpin(struct L298NEngine, int direction);

struct Car
{
    struct L298NEngine leftEngine, rightEngine;
};

enum CarDirection
{
    CAR_FORWARD, CAR_BACKWARD, CAR_BREAK, 
    CAR_TURN_LEFT, CAR_TURN_RIGHT, CAR_IDLE
};

void carInit(struct Car);
struct Car getCar();
void carMove(struct Car, int);

#endif
