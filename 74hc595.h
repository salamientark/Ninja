#pragma once

# include <Arduino.h>
# include "config.h"

extern byte MENU_LED_REGISTER;
extern byte MAGNET_REGISTER;
extern byte MAGNET_LED_REGISTER;

/* CHIP CONTROL */
void outputEnable();
void outputDisable();

/* MENU LED CONTROL */
void menuLedWrite(byte pin, bool state);
void offMenuLed(byte pin);

/* MAGNET CONTROL */
void magnetWrite(byte pin, bool state);
void offMagnet(byte magnetPin);

/* MAGNET LED CONTROL */
void magnetLedWrite(byte pin, bool state);
void offMagnetLED(byte magnetLEDPin);

/* SHIFT REGISTER CONTROL */
void sendRegisters();
