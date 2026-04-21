#include "game_loop.h"
#include "standby.h"

/* FILE-LOCAL VARIABLES — only used within game_loop.cpp */
static int DELAY_BETWEEN_DROPS_MIN;
static int DELAY_BETWEEN_DROPS_MAX;
static int MAGNET_LED_ON_TIME;
static int DROP_TIME;
static int TWO_DROP_MIN;
static int TWO_DROP_MAX;
static int TWO_DROP_CURRENT;
static bool RANDOM_LEDS_ENABLED = false;
static int  FAKE_LED_ON_TIME     = 0;

void shuffleList(byte arrayToShuffle[], int size) {
  for (int i = size - 1; i > 0; i--) {
    int j = random(0, i + 1);
    byte temp = arrayToShuffle[i];
    arrayToShuffle[i] = arrayToShuffle[j];
    arrayToShuffle[j] = temp;
  }
}

void  setup_game_loop() {
  RANDOM_LEDS_ENABLED = false;
  /* Initial setup | difficulty_1 */
  switch (_difficulty) {
    case 1:
      DELAY_BETWEEN_DROPS_MIN = 2000; DELAY_BETWEEN_DROPS_MAX = 2000;
      MAGNET_LED_ON_TIME = 1000;
      DROP_TIME = 2000;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
    case 2:
      DELAY_BETWEEN_DROPS_MIN = 1700; DELAY_BETWEEN_DROPS_MAX = 1700;
      MAGNET_LED_ON_TIME = 700;
      DROP_TIME = 2000;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
    case 3:
      DELAY_BETWEEN_DROPS_MIN = 1500; DELAY_BETWEEN_DROPS_MAX = 1500;
      MAGNET_LED_ON_TIME = 400;
      DROP_TIME = 2000;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
    case 4:
      DELAY_BETWEEN_DROPS_MIN = 1500; DELAY_BETWEEN_DROPS_MAX = 1500;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1500;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
    case 5:
      DELAY_BETWEEN_DROPS_MIN = 1000; DELAY_BETWEEN_DROPS_MAX = 1800;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1500;
      TWO_DROP_MIN = 1;
      TWO_DROP_MAX = 1;
      break;
    case 6:
      DELAY_BETWEEN_DROPS_MIN = 700;  DELAY_BETWEEN_DROPS_MAX = 1300;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1000;
      TWO_DROP_MIN = 1;
      TWO_DROP_MAX = 2;
      break;
    case 7:
      DELAY_BETWEEN_DROPS_MIN = 500;  DELAY_BETWEEN_DROPS_MAX = 1100;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1000;
      TWO_DROP_MIN = 2;
      TWO_DROP_MAX = 3;
      RANDOM_LEDS_ENABLED = true;
      FAKE_LED_ON_TIME    = 300;
      break;
    case 8:
      DELAY_BETWEEN_DROPS_MIN = 200;  DELAY_BETWEEN_DROPS_MAX = 600;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 600;
      TWO_DROP_MIN = 3;
      TWO_DROP_MAX = 4;
      break;
    // Fallback to difficulty 1
    default:
      DELAY_BETWEEN_DROPS_MIN = 2000; DELAY_BETWEEN_DROPS_MAX = 2000;
      MAGNET_LED_ON_TIME = 1000;
      DROP_TIME = 2000;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
  }
  TWO_DROP_CURRENT = random(TWO_DROP_MIN, TWO_DROP_MAX + 1);
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
  int twoDrop_budget = TWO_DROP_CURRENT;

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

      dropInterval = (unsigned long)random(DROP_TIME + DELAY_BETWEEN_DROPS_MIN, DROP_TIME + DELAY_BETWEEN_DROPS_MAX + 1);
      dropStart = now;
    }
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
        if (!fakeLedMask || elapsed >= (unsigned long)FAKE_LED_ON_TIME) {
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
          isTwoDrop = (TWO_DROP_CURRENT > 0)
                   && (obj_index + 1 < OBJ_NBR)
                   && (random(0, 2) == 0);
          cur1 = obj_list[obj_index];
          cur2 = isTwoDrop ? obj_list[obj_index + 1] : -1;

          magnetLedWrite(cur1, 1);
          if (cur2 >= 0) magnetLedWrite(cur2, 1);
          sendRegisters();
          stateStart = millis();
        }
        if (elapsed >= (unsigned long)MAGNET_LED_ON_TIME) {
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
        if (elapsed >= (unsigned long)DROP_TIME) {
          offMagnetLED(cur1);
          if (cur2 >= 0) offMagnetLED(cur2);
          sendRegisters();
          // Compute random wait before next drop
          waitDuration = (unsigned long)random(DELAY_BETWEEN_DROPS_MIN,
                                               DELAY_BETWEEN_DROPS_MAX + 1);
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
          if (isTwoDrop) TWO_DROP_CURRENT--;
          cur1 = -1;
          cur2 = -1;
          state      = RANDOM_LEDS_ENABLED ? DROP_FAKE_LEDS : DROP_LED_ON;
          stateStart = millis();
          stateEntry = true;
        }
        break;
      }
    }
  }
}
