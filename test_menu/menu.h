#pragma once
#include <Arduino.h>
#include "Button.h"
#include "config.h"

// Globals defined in test_menu.ino, referenced here
extern int _difficulty;
extern Button upButton;
extern Button downButton;
extern Button startButton;

extern void menuLedWrite(byte pin, bool state);

void menu_loop();
void show_difficulty();
