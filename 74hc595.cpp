#include "74hc595.h"

void menuLedWrite(byte pin, bool state) {
  bitWrite(MENU_LED_REGISTER, pin, state);
  sendRegisters();
}

void offMenuLed(byte pin) {
  bitClear(MENU_LED_REGISTER, pin);
  sendRegisters();
}

void magnetWrite(byte pin, bool state) {
  bitWrite(MAGNET_REGISTER, pin, state);
  sendRegisters();
}

void magnetLedWrite(byte pin, bool state) {
  bitWrite(MAGNET_LED_REGISTER, pin, state);
  sendRegisters();
}

void offMagnet(byte magnetPin) {
  bitClear(MAGNET_REGISTER, magnetPin);
  sendRegisters();
}

void offMagnetLED(byte magnetLEDPin) {
  bitClear(MAGNET_LED_REGISTER, magnetLEDPin);
  sendRegisters();
}

void outputEnable() {
  digitalWrite(DATA_OUTPUT_PIN, LOW);  // OE is active LOW
}

void outputDisable() {
  digitalWrite(DATA_OUTPUT_PIN, HIGH);
}

// ==================================
// Fonction : sendRegisters
// Sends both chip registers in one latch cycle.
// Chip 2 (LED) is shifted first so it cascades through chip 1's Q7'.
// Chip 1 (magnets) is shifted second and stays in chip 1.
// ==================================
void sendRegisters() {
  digitalWrite(DATA_LOCK_PIN, LOW);
  shiftOut(DATA_PIN, DATA_SHIFT_PIN, MSBFIRST, MAGNET_LED_REGISTER);  // Chip 3 (shifts deepest)
  shiftOut(DATA_PIN, DATA_SHIFT_PIN, MSBFIRST, MAGNET_REGISTER);      // Chip 2 (middle)
  shiftOut(DATA_PIN, DATA_SHIFT_PIN, MSBFIRST, MENU_LED_REGISTER);    // Chip 1 (closest to Arduino)
  digitalWrite(DATA_LOCK_PIN, HIGH);  // Single latch — all chips update simultaneously
}
