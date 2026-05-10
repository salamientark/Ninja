#include "menu.h"
#include "74hc595.h"
#include "standby.h"
#include <Arduino.h>

#define STANDBY_TIMEOUT_MS 60000UL  // 1 minute

void menu_loop() {
  unsigned long lastActivity = millis();
  bool inStandby = false;

  while (true) {
    bool pressed = false;

    if (upButton.isPressed()) {
      _difficulty++;
      if (_difficulty > DIFFICULTY_MAX)
        _difficulty = DIFFICULTY_MAX;
      pressed = true;
    }
    if (downButton.isPressed()) {
      _difficulty--;
      if (_difficulty < 1)
        _difficulty = 1;
      pressed = true;
    }
    if (startButton.isPressed()) {
      if (inStandby) {
        // First press wakes from standby, does not start game
        standby_reset();
        inStandby = false;
        lastActivity = millis();
        continue;
      }
      return;  // Start game
    }

    if (pressed) {
      if (inStandby) {
        standby_reset();
        inStandby = false;
      }
      lastActivity = millis();
    }

    if (!inStandby && (millis() - lastActivity >= STANDBY_TIMEOUT_MS)) {
      inStandby = true;
      standby_reset();
    }

    if (inStandby) {
      standby_tick();
    } else {
      show_difficulty();
    }
  }
}

void show_difficulty() {
  int counter = 0;
  while (counter < DIFFICULTY_MAX) {
    bool on = counter < _difficulty;
    menuLedWrite(DIFFICULTY_MAX - 1 - counter, on);
    magnetLedWrite(counter, on);
    counter++;
  }
  sendRegisters();
}
