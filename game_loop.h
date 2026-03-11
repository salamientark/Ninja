#pragma once

# include <Arduino.h>
# include "74hc595.h"
# include "config.h"

extern int obj_list[OBJ_NBR];


void  shuffleList(int arrayToShuffle[], int size);

void  game_loop();
