#pragma once

#include <Arduino.h>
#include "74hc595.h"
#include "config.h"

extern byte obj_list[OBJ_NBR];
extern int _difficulty;

void  shuffleList(byte arrayToShuffle[], int size);
void  game_loop();
