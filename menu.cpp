#include "menu.h"
#include <Arduino.h>

void menu_loop() {
  Serial.println("Menu: select difficulty");
  while (true) {
    if (upButton.isPressed()) {
      _difficulty++;
      if (_difficulty > DIFFICULTY_MAX)
        _difficulty = DIFFICULTY_MAX;
      Serial.print("UP pressed -> difficulty: ");
      Serial.println(_difficulty);
    }
    if (downButton.isPressed()) {
      _difficulty--;
      if (_difficulty < 1)
        _difficulty = 1;
      Serial.print("DOWN pressed -> difficulty: ");
      Serial.println(_difficulty);
    }
    if (startButton.isPressed()) {
      Serial.print("START pressed -> launching game at difficulty: ");
      Serial.println(_difficulty);
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
