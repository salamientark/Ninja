#include "game_loop.h"

/* GLOBAL VARIABLES */
int DELAY_BETWEEN_DROPS;
int MAGNET_LED_ON_TIME;
int DROP_TIME;
int TWO_DROP_MIN;
int TWO_DROP_MAX;
int TWO_DROP_CURRENT;
bool RANDOM_LEDS_ENABLED = false;
int  FAKE_LED_ON_TIME     = 0;

void shuffleList(int arrayToShuffle[], int size) {
  // Loop backward through the array
  for (int i = size - 1; i > 0; i--) {
    // Pick a random index from 0 to i
    int j = random(0, i + 1); 
    
    // Swap the elements
    int temp = arrayToShuffle[i];
    arrayToShuffle[i] = arrayToShuffle[j];
    arrayToShuffle[j] = temp;
  }
}

void  setup_game_loop() {
  RANDOM_LEDS_ENABLED = false;
  /* Initial setup | difficulty_1 */
  switch (_difficulty) {
    case 1:
      DELAY_BETWEEN_DROPS = 2000;
      MAGNET_LED_ON_TIME = 1000;
      DROP_TIME = 2000;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
    case 2:
      DELAY_BETWEEN_DROPS = 1700;
      MAGNET_LED_ON_TIME = 700;
      DROP_TIME = 2000;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
    case 3:
      DELAY_BETWEEN_DROPS = 1500;
      MAGNET_LED_ON_TIME = 400;
      DROP_TIME = 2000;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
    case 4:
      DELAY_BETWEEN_DROPS = 1500;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1500;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
    case 5:
      DELAY_BETWEEN_DROPS = 1400;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1500;
      TWO_DROP_MIN = 1;
      TWO_DROP_MAX = 1;
      break;
    case 6:
      DELAY_BETWEEN_DROPS = 1000;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1000;
      TWO_DROP_MIN = 1;
      TWO_DROP_MAX = 2;
      break;
    case 7:
      DELAY_BETWEEN_DROPS = 800;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1000;
      TWO_DROP_MIN = 2;
      TWO_DROP_MAX = 3;
      RANDOM_LEDS_ENABLED = true;
      FAKE_LED_ON_TIME    = 300;
      break;
    case 8:
      DELAY_BETWEEN_DROPS = 400;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 600;
      TWO_DROP_MIN = 3;
      TWO_DROP_MAX = 4;
      break;
    // Fallback to difficulty 1
    default:
      DELAY_BETWEEN_DROPS = 2000;
      MAGNET_LED_ON_TIME = 1000;
      DROP_TIME = 2000;
      TWO_DROP_MIN = 0;
      TWO_DROP_MAX = 0;
      break;
  }
  TWO_DROP_CURRENT = random(TWO_DROP_MIN, TWO_DROP_MAX + 1);
}

void showRandomFakeLeds() {
  int count = random(0, 3);  // 0, 1, or 2
  for (int i = 0; i < count; i++) {
    magnetLedWrite(random(0, OBJ_NBR), 1);
  }
  sendRegisters();
  if (count > 0) {
    delay(FAKE_LED_ON_TIME);
    MAGNET_LED_REGISTER = 0b00000000;
    sendRegisters();
  }
}

void oneDrop(int obj) {
  if (MAGNET_LED_ON_TIME > 0) {
    magnetLedWrite(obj, 1);
    delay(MAGNET_LED_ON_TIME);
  }
  offMagnet(obj);
  delay(DROP_TIME);
  offMagnetLED(obj);
  delay(DELAY_BETWEEN_DROPS);
}

void twoDrop(int obj1, int obj2) {
  if (MAGNET_LED_ON_TIME > 0) {
    magnetLedWrite(obj1, 1);
    magnetLedWrite(obj2, 1);
    delay(MAGNET_LED_ON_TIME);
  }
  offMagnet(obj1);
  offMagnet(obj2);
  delay(DROP_TIME);
  offMagnetLED(obj1);
  offMagnetLED(obj2);
  delay(DELAY_BETWEEN_DROPS);
}


void  game_loop() {
  int obj_index = 0;

  setup_game_loop();

  // Restore LED
  MAGNET_LED_REGISTER = 0b00000000;
  sendRegisters();

  // Shuffle the list of objects
  shuffleList(obj_list, OBJ_NBR);

  delay(1000); // Wait a moment before starting the game loop

  while (obj_index < OBJ_NBR) {
    bool doTwoDrop = (TWO_DROP_CURRENT > 0) && (obj_index + 1 < OBJ_NBR) && (random(0, 2) == 0);

    // Difficulty 7 and above: Random fake LEDs before the actual drop
    if (RANDOM_LEDS_ENABLED)
      showRandomFakeLeds();

    if (doTwoDrop) {
      twoDrop(obj_list[obj_index] - 1, obj_list[obj_index + 1] - 1);
      obj_index += 2;
      TWO_DROP_CURRENT--;
    }
    else {
      oneDrop(obj_list[obj_index] - 1);
      obj_index++;
    }
  }
}
