#include "74hc595.h"

void menuLedWrite(byte pin, bool state) {
  bitWrite(MENU_LED_REGISTER, pin, state);
}

void offMenuLed(byte pin) {
  bitClear(MENU_LED_REGISTER, pin);
}

void magnetWrite(byte pin, bool state) {
  bitWrite(MAGNET_REGISTER, pin, state);
}

void magnetLedWrite(byte pin, bool state) {
  bitWrite(MAGNET_LED_REGISTER, pin, state);
}

void offMagnet(byte magnetPin) {
  bitClear(MAGNET_REGISTER, magnetPin);
}

void offMagnetLED(byte magnetLEDPin) {
  bitClear(MAGNET_LED_REGISTER, magnetLEDPin);
}

void outputEnable() {
  digitalWrite(MENU_OE_PIN,   LOW);  // OE is active LOW
  analogWrite(MAGNET_OE_PIN, 255 - MAGNET_HOLD_DUTY); // PWM, active-LOW inverted
}

void outputDisable() {
  digitalWrite(MENU_OE_PIN,   HIGH);
  analogWrite(MAGNET_OE_PIN, 255); // 0% duty after invert = fully disabled
}

// Chain A: 1 chip — menu LEDs only
static void sendMenuRegister() {
  digitalWrite(MENU_LATCH_PIN, LOW);
  shiftOut(MENU_DATA_PIN, MENU_SHIFT_PIN, MSBFIRST, MENU_LED_REGISTER);
  digitalWrite(MENU_LATCH_PIN, HIGH);
}

// Chain B: 2 chips — each chip handles one side (4 LEDs + 4 magnets)
//   Per chip: Q0–Q3 = LEDs, Q4–Q7 = magnets
//   Side A (chip 2, closest):  objects 0–3
//   Side B (chip 3, deepest):  objects 4–7
static void sendMagnetRegisters() {
  byte sideA = ((MAGNET_REGISTER & 0x0F) << 4) | (MAGNET_LED_REGISTER & 0x0F);
  byte sideB = (MAGNET_REGISTER & 0xF0)        | ((MAGNET_LED_REGISTER >> 4) & 0x0F);

  digitalWrite(MAGNET_LATCH_PIN, LOW);
  shiftOut(MAGNET_DATA_PIN, MAGNET_SHIFT_PIN, MSBFIRST, sideB); // Chip 3 — shifts deepest
  shiftOut(MAGNET_DATA_PIN, MAGNET_SHIFT_PIN, MSBFIRST, sideA); // Chip 2 — closest to Arduino
  digitalWrite(MAGNET_LATCH_PIN, HIGH);
}

// Public: update all hardware simultaneously
void sendRegisters() {
  sendMenuRegister();
  sendMagnetRegisters();
}
