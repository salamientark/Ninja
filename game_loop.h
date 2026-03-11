#pragma once

# include <Arduino.h>
# include "74hc595.h"
# include "config.h"

extern int obj_list[OBJ_NBR];
extern int _difficulty;
extern int TWO_DROP_MIN;
extern int TWO_DROP_MAX;
extern int TWO_DROP_CURRENT;

void  shuffleList(int arrayToShuffle[], int size);
void  oneDrop(int obj);
void  twoDrop(int obj1, int obj2);
void  game_loop();
