#include "menu.h"
#include <Arduino.h>

void menu_loop() {
  while (true) {
    if (upButton.isPressed()) {
      _difficulty++;
      if (_difficulty > DIFFICULTY_MAX)
        _difficulty = DIFFICULTY_MAX;
    }
    if (downButton.isPressed()) {
      _difficulty--;
      if (_difficulty < 1)
        _difficulty = 1;
    }
    if (startButton.isPressed()) {
      return;
    }
    show_difficulty();
  }
}

void show_difficulty() {
  int counter = 0;
  while (counter < DIFFICULTY_MAX) {
    menuLedWrite(MENU_LED_1_PIN + counter, counter < _difficulty);
    counter++;
  }
}
