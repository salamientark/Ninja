#include "game_loop.h"
#include "standby.h"
#include <avr/pgmspace.h>

/* ── Difficulty table (PROGMEM) ── */
/* PR 1: current curve preserved exactly; new-axis fields sentinel-disabled. */

static const DifficultyConfig DIFFICULTY_TABLE[8] PROGMEM = {
  // lvl 1
  { 1000, 1000, 2000, 2000, 0, 0, 2000, 0, 0, 0, 0,   0, 0 },
  // lvl 2
  {  700,  700, 1700, 1700, 0, 0, 2000, 0, 0, 0, 0,   0, 0 },
  // lvl 3
  {  400,  400, 1500, 1500, 0, 0, 2000, 0, 0, 0, 0,   0, 0 },
  // lvl 4
  {    0,    0, 1500, 1500, 0, 0, 1500, 0, 0, 0, 0,   0, 0 },
  // lvl 5
  {    0,    0, 1000, 1800, 0, 0, 1500, 1, 1, 0, 0,   0, 0 },
  // lvl 6
  {    0,    0,  700, 1300, 0, 0, 1000, 1, 2, 0, 0,   0, 0 },
  // lvl 7
  {    0,    0,  500, 1100, 0, 0, 1000, 2, 3, 0, 0, 300, 0 },
  // lvl 8
  {    0,    0,  200,  600, 0, 0,  600, 3, 4, 0, 0,   0, 0 },
};

/* RAM-resident copy of the active level config. */
static DifficultyConfig _cfg;

/* Per-round mutable state (was TWO_DROP_CURRENT static). */
static int _two_drop_current;

void shuffleList(byte arrayToShuffle[], int size) {
  for (int i = size - 1; i > 0; i--) {
    int j = random(0, i + 1);
    byte temp = arrayToShuffle[i];
    arrayToShuffle[i] = arrayToShuffle[j];
    arrayToShuffle[j] = temp;
  }
}

void  setup_game_loop() {
  uint8_t lvl_idx = constrain(_difficulty - 1, 0, 7);
  memcpy_P(&_cfg, &DIFFICULTY_TABLE[lvl_idx], sizeof(DifficultyConfig));
  _two_drop_current = random(_cfg.two_drop_min, _cfg.two_drop_max + 1);
}

/* ── Drop state machine (non-blocking, replaces delay()-based drops) ── */

enum DropState {
  DROP_FAKE_LEDS,   // show random distraction LEDs (difficulty 7)
  DROP_LED_ON,      // light indicator LED(s) before release
  DROP_RELEASE,     // release magnet(s)
  DROP_CLEANUP,     // wait for physical drop, then turn off LED(s)
  DROP_WAIT         // pause between drops
};

static void game_loop_8() {
  int obj_index      = 0;
  int twoDrop_budget = _two_drop_current;

  confuse_anim_reset();

  unsigned long dropStart    = millis();
  unsigned long dropInterval = 1000UL;

  while (obj_index < OBJ_NBR) {
    unsigned long now = millis();

    confuse_anim_tick();

    if (now - dropStart >= dropInterval) {
      bool doTwoDrop = (twoDrop_budget > 0)
                    && (obj_index + 1 < OBJ_NBR)
                    && (random(0, 2) == 0);

      if (doTwoDrop) {
        offMagnet(obj_list[obj_index]);
        offMagnet(obj_list[obj_index + 1]);
        sendRegisters();
        obj_index     += 2;
        twoDrop_budget--;
      } else {
        offMagnet(obj_list[obj_index]);
        sendRegisters();
        obj_index++;
      }

      dropInterval = (unsigned long)random(_cfg.drop_time + _cfg.wait_min,
                                           _cfg.drop_time + _cfg.wait_max + 1);
      dropStart = now;
    }
  }

  // Wait for last stick to fall before returning (caller re-energizes magnets)
  while (millis() - dropStart < (unsigned long)_cfg.drop_time) {
    confuse_anim_tick();
  }

  MENU_LED_REGISTER   = 0x00;
  MAGNET_LED_REGISTER = 0x00;
  sendRegisters();
}

void  game_loop() {
  int obj_index = -1;

  setup_game_loop();

  // Restore LED
  MAGNET_LED_REGISTER = 0b00000000;
  sendRegisters();

  // Shuffle the list of objects
  shuffleList(obj_list, OBJ_NBR);

  if (_difficulty == 8) {
    game_loop_8();
    return;
  }

  // State machine variables
  DropState      state        = DROP_WAIT;  // start with initial pause
  unsigned long  stateStart   = millis();
  unsigned long  waitDuration = 1000UL;     // 1s delay before first drop
  int  cur1 = -1, cur2 = -1;
  bool isTwoDrop  = false;
  bool stateEntry = true;  // true on first tick of each state
  byte fakeLedMask = 0;    // tracks which bits were set for fake LEDs

  while (obj_index < OBJ_NBR) {
    unsigned long now     = millis();
    unsigned long elapsed = now - stateStart;

    switch (state) {

      case DROP_FAKE_LEDS: {
        if (stateEntry) {
          stateEntry = false;
          fakeLedMask = 0;
          int count = random(0, 3);
          for (int i = 0; i < count; i++)
            fakeLedMask |= (1 << random(0, OBJ_NBR));
          if (fakeLedMask) {
            MAGNET_LED_REGISTER |= fakeLedMask;
            sendRegisters();
          }
          stateStart = millis();
        }
        if (!fakeLedMask || elapsed >= (unsigned long)_cfg.confuse_flash_ms) {
          MAGNET_LED_REGISTER &= ~fakeLedMask;  // clear only the fake bits
          if (fakeLedMask) sendRegisters();
          state      = DROP_LED_ON;
          stateStart = millis();
          stateEntry = true;
        }
        break;
      }

      case DROP_LED_ON: {
        if (stateEntry) {
          stateEntry = false;
          // Pick objects for this drop
          isTwoDrop = (_two_drop_current > 0)
                   && (obj_index + 1 < OBJ_NBR)
                   && (random(0, 2) == 0);
          cur1 = obj_list[obj_index];
          cur2 = isTwoDrop ? obj_list[obj_index + 1] : -1;

          magnetLedWrite(cur1, 1);
          if (cur2 >= 0) magnetLedWrite(cur2, 1);
          sendRegisters();
          stateStart = millis();
        }
        if (elapsed >= (unsigned long)_cfg.warn_min) {
          state      = DROP_RELEASE;
          stateStart = millis();
          stateEntry = true;
        }
        break;
      }

      case DROP_RELEASE: {
        offMagnet(cur1);
        if (cur2 >= 0) offMagnet(cur2);
        sendRegisters();
        state      = DROP_CLEANUP;
        stateStart = millis();
        stateEntry = true;
        break;
      }

      case DROP_CLEANUP: {
        if (elapsed >= (unsigned long)_cfg.drop_time) {
          offMagnetLED(cur1);
          if (cur2 >= 0) offMagnetLED(cur2);
          sendRegisters();
          // Compute random wait before next drop
          waitDuration = (unsigned long)random(_cfg.wait_min,
                                               _cfg.wait_max + 1);
          state      = DROP_WAIT;
          stateStart = millis();
          stateEntry = true;
        }
        break;
      }

      case DROP_WAIT: {
        if (elapsed >= waitDuration) {
          // Advance to next object(s)
          obj_index += isTwoDrop ? 2 : 1;
          if (isTwoDrop) _two_drop_current--;
          cur1 = -1;
          cur2 = -1;
          state      = (_cfg.confuse_flash_ms != 0) ? DROP_FAKE_LEDS : DROP_LED_ON;
          stateStart = millis();
          stateEntry = true;
        }
        break;
      }
    }
  }
}
