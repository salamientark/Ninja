#include "game_loop.h"

/* GLOBAL VARIABLES */
int DELAY_BETWEEN_DROPS;
int MAGNET_LED_ON_TIME;
int DROP_TIME;

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
  /* Initial setup | difficulty_1 */
  switch (_difficulty) {
    case 1:
      DELAY_BETWEEN_DROPS = 2000;
      MAGNET_LED_ON_TIME = 1000;
      DROP_TIME = 2000;
      break;
    case 2:
      DELAY_BETWEEN_DROPS = 1700;
      MAGNET_LED_ON_TIME = 700;
      DROP_TIME = 2000;
      break;
    case 3:
      DELAY_BETWEEN_DROPS = 1500;
      MAGNET_LED_ON_TIME = 400;
      DROP_TIME = 2000;
      break;
    case 4:
      DELAY_BETWEEN_DROPS = 1500;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1500;
      break;
    case 5:
      DELAY_BETWEEN_DROPS = 1400;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1500;
      // Init 2 drop possibility
      break;
    case 6:
      DELAY_BETWEEN_DROPS = 1000;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1000;
      // Init 2 drop possibility
      break;
    case 7:
      DELAY_BETWEEN_DROPS = 800;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 1000;
      // Random led indicator
      // Init 2 drop possibility
      break;
    case 8:
      DELAY_BETWEEN_DROPS = 400;
      MAGNET_LED_ON_TIME = 0;
      DROP_TIME = 600;
      // confusing led indicator
      // Init 2 drop possibility
      break;
    // Fallback to difficulty 1
    default:
      DELAY_BETWEEN_DROPS = 2000;
      MAGNET_LED_ON_TIME = 1000;
      DROP_TIME = 2000;
     break;
  }
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

  // 2. Display the objects in a random order (e.g., using LEDs or a screen)
  while (obj_index < OBJ_NBR) {
    // Show the droppng object
    magnetLedWrite(obj_list[obj_index] - 1, 1); // Turn on the corresponding LED
    delay(MAGNET_LED_ON_TIME); // Wait for a moment before showing the next object

    // Drop the object
    offMagnet(obj_list[obj_index] - 1); // Turn off the magnet corresponding to the displayed object (assuming obj_list values are 1-indexed)
    delay(DROP_TIME); // Simulate the drop time for the object

    // Switch off the LED after the drop
    offMagnetLED(obj_list[obj_index] - 1); // Ensure the LED is turned off after the drop
    delay(DELAY_BETWEEN_DROPS); // Wait before showing the next object
    obj_index++;
  }
}
