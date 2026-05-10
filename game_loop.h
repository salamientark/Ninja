#pragma once

#include <Arduino.h>
#include "74hc595.h"
#include "config.h"

struct DifficultyConfig {
  uint16_t warn_min;          // ms; min == max → fixed warn
  uint16_t warn_max;
  uint16_t wait_min;          // ms; start of round (or constant if no accel)
  uint16_t wait_max;
  uint16_t wait_end_min;      // ms; end of round; 0/0 → no accel
  uint16_t wait_end_max;
  uint16_t drop_time;         // ms; physical fall window
  uint8_t  two_drop_min;      // count per round; 0 → none
  uint8_t  two_drop_max;
  uint8_t  two_drop_prob;     // 0–100; chance per drop when budget > 0 (unused in PR1)
  uint8_t  three_drop_count;  // count per round; 0 → none (unused in PR1)
  uint16_t confuse_flash_ms;  // 0 → no confuse LEDs
  uint8_t  fakeout_prob;      // 0–100; 0 → no fake-out (unused in PR1)
};

extern byte obj_list[OBJ_NBR];
extern int _difficulty;

void  shuffleList(byte arrayToShuffle[], int size);
void  game_loop();
