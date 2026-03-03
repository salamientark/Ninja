#pragma once
#include "Button.h"
#include "config.h"

// Globals defined in Ninja.ino, referenced here
extern int _difficulty;
extern Button upButton;
extern Button downButton;
extern Button startButton;

void menu_loop();
void show_difficulty();
