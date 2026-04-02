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
  digitalWrite(MENU_OE_PIN,   LOW);  // OE is active LOW
  digitalWrite(MAGNET_OE_PIN, LOW);
}

void outputDisable() {
  digitalWrite(MENU_OE_PIN,   HIGH);
  digitalWrite(MAGNET_OE_PIN, HIGH);
}

// Chain A: 1 chip — menu LEDs only
static void sendMenuRegister() {
  digitalWrite(MENU_LATCH_PIN, LOW);
  shiftOut(MENU_DATA_PIN, MENU_SHIFT_PIN, MSBFIRST, MENU_LED_REGISTER);
  digitalWrite(MENU_LATCH_PIN, HIGH);
}

// Chain B: 2 chips — magnets (closest) + magnet LEDs (deepest)
static void sendMagnetRegisters() {
  digitalWrite(MAGNET_LATCH_PIN, LOW);
  shiftOut(MAGNET_DATA_PIN, MAGNET_SHIFT_PIN, MSBFIRST, MAGNET_LED_REGISTER); // Chip 3 — shifts deepest
  shiftOut(MAGNET_DATA_PIN, MAGNET_SHIFT_PIN, MSBFIRST, MAGNET_REGISTER);     // Chip 2 — closest to Arduino
  digitalWrite(MAGNET_LATCH_PIN, HIGH);
}

// Public: update all hardware simultaneously
void sendRegisters() {
  sendMenuRegister();
  sendMagnetRegisters();
}
