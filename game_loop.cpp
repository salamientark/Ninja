#include "game_loop.h"


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

void  game_loop() {
  int obj_index = 0;

  // 1. Shuffle the list of objects
  shuffleList(obj_list, OBJ_NBR);

  // 2. Display the objects in a random order (e.g., using LEDs or a screen)
  while (obj_index < OBJ_NBR) {
    offMagnet(obj_list[obj_index] - 1); // Turn off the magnet corresponding to the displayed object (assuming obj_list values are 1-indexed)
    offMagnetLED(obj_list[obj_index] - 1); // Turn off the corresponding LED
    delay(1000); // Wait for a moment before showing the next object
    obj_index++;
  }
}
