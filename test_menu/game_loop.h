#pragma once

#include <Arduino.h>
#include "74hc595.h"
#include "config.h"

extern int obj_list[OBJ_NBR];
extern int _difficulty;
extern int TWO_DROP_MIN;
extern int TWO_DROP_MAX;
extern int TWO_DROP_CURRENT;

extern bool RANDOM_LEDS_ENABLED;
extern int  FAKE_LED_ON_TIME;

void  shuffleList(int arrayToShuffle[], int size);
void  showRandomFakeLeds();
void  oneDrop(int obj);
void  twoDrop(int obj1, int obj2);
void  game_loop();
