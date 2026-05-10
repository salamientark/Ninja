#include "game_loop.h"
#include "standby.h"
#include <avr/pgmspace.h>

/* ── Difficulty table (PROGMEM) ── */
/* PR 1: current curve preserved exactly; new-axis fields sentinel-disabled. */

static const DifficultyConfig DIFFICULTY_TABLE[8] PROGMEM = {
  // fields: warn_min, warn_max, wait_min, wait_max, wait_end_min, wait_end_max,
  //         drop_time, two_drop_min, two_drop_max, two_drop_prob,
  //         three_drop_count, confuse_flash_ms, fakeout_prob
  // lvl 1 — fixed 800 warn, fixed 1700 wait
  {  800,  800, 1700, 1700,    0,    0, 1800, 0, 0,   0, 0,   0,  0 },
  // lvl 2 — fixed 500 warn, 1400 ±150 wait
  {  500,  500, 1250, 1550,    0,    0, 1700, 0, 0,   0, 0,   0,  0 },
  // lvl 3 — variable 150–350 warn, 1200 ±150 wait
  {  150,  350, 1050, 1350,    0,    0, 1500, 0, 0,   0, 0,   0,  0 },
  // lvl 4 — variable 0–200 warn, 1000 ±200 wait, fake-out 20%
  {    0,  200,  800, 1200,    0,    0, 1300, 0, 0,   0, 0,   0, 20 },
  // lvl 5 — no warn, accel 1100→700, two-drop 0–1 @30%
  {    0,    0, 1100, 1100,  700,  700, 1100, 0, 1,  30, 0,   0,  0 },
  // lvl 6 — accel 900→500, two-drop 1–2 @50%
  {    0,    0,  900,  900,  500,  500,  900, 1, 2,  50, 0,   0,  0 },
  // lvl 7 — accel 700→400, two-drop 2–3 @50%, confuse 300ms
  {    0,    0,  700,  700,  400,  400,  750, 2, 3,  50, 0, 300,  0 },
  // lvl 8 — accel 400→200 (PR3 will route through state machine), confuse 150ms
  {    0,    0,  400,  400,  200,  200,  600, 3, 4,  50, 1, 150,  0 },
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
  DROP_FAKE_LEDS,   // show random distraction LEDs (confuse_flash_ms > 0)
  DROP_FAKEOUT,     // light a magnet LED but no drop (fakeout_prob > 0)
  DROP_LED_ON,      // light indicator LED(s) before release
  DROP_RELEASE,     // release magnet(s)
  DROP_CLEANUP,     // wait for physical drop, then turn off LED(s)
  DROP_WAIT         // pause between drops
};

/* Linear interp uint16 over num/den with signed delta (handles b<a). */
static uint16_t lerp_u16(uint16_t a, uint16_t b, uint8_t num, uint8_t den) {
  if (den == 0) return a;
  return (uint16_t)((int32_t)a + ((int32_t)b - (int32_t)a) * num / den);
}

/* Compute wait range for the upcoming drop, applying tempo accel if enabled. */
static void compute_wait_range(int next_idx, uint16_t &lo, uint16_t &hi) {
  if (_cfg.wait_end_min == 0 && _cfg.wait_end_max == 0) {
    lo = _cfg.wait_min;
    hi = _cfg.wait_max;
    return;
  }
  if (next_idx < 0) next_idx = 0;
  if (next_idx > OBJ_NBR - 1) next_idx = OBJ_NBR - 1;
  lo = lerp_u16(_cfg.wait_min, _cfg.wait_end_min, (uint8_t)next_idx, OBJ_NBR - 1);
  hi = lerp_u16(_cfg.wait_max, _cfg.wait_end_max, (uint8_t)next_idx, OBJ_NBR - 1);
}

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
  bool isTwoDrop   = false;
  bool stateEntry  = true;          // true on first tick of each state
  byte fakeLedMask = 0;             // bits set for confuse / fakeout LEDs
  uint16_t cur_warn = 0;            // per-drop warn duration (ms)

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
          MAGNET_LED_REGISTER &= ~fakeLedMask;
          if (fakeLedMask) sendRegisters();
          fakeLedMask = 0;
          state      = DROP_LED_ON;
          stateStart = millis();
          stateEntry = true;
        }
        break;
      }

      case DROP_FAKEOUT: {
        // Light a random magnet LED for cur_warn ms, no stick drops.
        if (stateEntry) {
          stateEntry = false;
          fakeLedMask = (byte)(1 << random(0, OBJ_NBR));
          MAGNET_LED_REGISTER |= fakeLedMask;
          sendRegisters();
          stateStart = millis();
        }
        if (elapsed >= (unsigned long)cur_warn) {
          MAGNET_LED_REGISTER &= ~fakeLedMask;
          sendRegisters();
          fakeLedMask = 0;
          // Re-arm wait before retrying a real drop at same obj_index.
          uint16_t lo, hi;
          int next_idx = obj_index + (isTwoDrop ? 2 : 1);
          compute_wait_range(next_idx, lo, hi);
          waitDuration = (unsigned long)random(lo, hi + 1);
          state      = DROP_WAIT;
          stateStart = millis();
          stateEntry = true;
        }
        break;
      }

      case DROP_LED_ON: {
        if (stateEntry) {
          stateEntry = false;
          isTwoDrop = (_two_drop_current > 0)
                   && (obj_index + 1 < OBJ_NBR)
                   && (random(0, 100) < _cfg.two_drop_prob);
          cur1 = obj_list[obj_index];
          cur2 = isTwoDrop ? obj_list[obj_index + 1] : -1;

          // Per-drop variable warn (sentinel: warn_max==0 → no warn).
          cur_warn = (_cfg.warn_max == 0)
                       ? 0
                       : (uint16_t)random(_cfg.warn_min, _cfg.warn_max + 1);

          if (cur_warn == 0) {
            // Skip warn entirely — no LED flash, go straight to release.
            state      = DROP_RELEASE;
            stateStart = millis();
            stateEntry = true;
            break;
          }

          magnetLedWrite(cur1, 1);
          if (cur2 >= 0) magnetLedWrite(cur2, 1);
          sendRegisters();
          stateStart = millis();
        }
        if (elapsed >= (unsigned long)cur_warn) {
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
          // Lerp wait range using the upcoming drop's index.
          uint16_t lo, hi;
          int next_idx = obj_index + (isTwoDrop ? 2 : 1);
          compute_wait_range(next_idx, lo, hi);
          waitDuration = (unsigned long)random(lo, hi + 1);
          state      = DROP_WAIT;
          stateStart = millis();
          stateEntry = true;
        }
        break;
      }

      case DROP_WAIT: {
        if (elapsed >= waitDuration) {
          // Roll fakeout (skip on initial wait — no prior drop frame).
          bool fakeout = (_cfg.fakeout_prob > 0)
                      && (obj_index >= 0)
                      && (random(0, 100) < _cfg.fakeout_prob);
          if (fakeout) {
            // Force cur_warn > 0; LED needs a visible duration.
            uint16_t lo = _cfg.warn_min ? _cfg.warn_min : 1;
            uint16_t hi = _cfg.warn_max ? _cfg.warn_max : lo;
            if (hi < lo) hi = lo;
            cur_warn   = (uint16_t)random(lo, hi + 1);
            state      = DROP_FAKEOUT;
            stateStart = millis();
            stateEntry = true;
            break;
          }
          // Commit previous drop, advance for next.
          obj_index += isTwoDrop ? 2 : 1;
          if (isTwoDrop) _two_drop_current--;
          isTwoDrop = false;
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
