#include "config.h"
#include "Button.h"
#include "menu.h"
#include "game_loop.h"

/* ************************************************************************** */
/*                                GLOBAL & CONSTANTS                          */
/* ************************************************************************** */
// OBJ_NBR and pin/difficulty constants are defined in config.h


// Global variables
int   _difficulty = 2;
int   obj_list[OBJ_NBR];
byte  MENU_LED_REGISTER   = 0b00000000;  // Chip 1: menu/difficulty LEDs
byte  MAGNET_REGISTER     = 0b00000000;  // Chip 2: electromagnet outputs
byte  MAGNET_LED_REGISTER = 0b00000000;  // Chip 3: magnet indicator LEDs

/* ************************************************************************** */
/*                                     SETUP                                  */
/* ************************************************************************** */

Button upButton(UP_BUTTON_PIN);
Button downButton(DOWN_BUTTON_PIN);
Button startButton(START_BUTTON_PIN);

void setup() {
  // RANDOM SEED
  randomSeed(analogRead(0));

  // INPUTS
  upButton.begin();
  downButton.begin();
  startButton.begin();

  // OUTPUTS
  pinMode(DATA_LOCK_PIN, OUTPUT);
  pinMode(DATA_SHIFT_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(DATA_OUTPUT_PIN, OUTPUT);
}

/* ************************************************************************** */
/*                                 MAIN LOOP                                  */
/* ************************************************************************** */

void loop() {
  // 1. Setup initial state
  init_game();

  // 2. Show menu and allow difficulty selection
  menu_loop();

  // 3. Switch off menu LEDs
  MENU_LED_REGISTER = 0b00000000;
  sendRegisters();

  delay(1000); // Small delay to avoid bouncing issues when starting the game

  // 4. Game loop
  game_loop();
}

/* ************************************************************************** */
/*                              CUSTOM FUNCTIONS                              */
/* ************************************************************************** */


void  init_game() {
  // Initialize game state, reset variables, etc.
  for (int i = 0; i < OBJ_NBR; i++)
    obj_list[i] = i + 1; // Reset the object list to default values

  _difficulty = 2; // Reset difficulty to default

  // Activate magnets
  MAGNET_REGISTER = 0b11111111; // All magnets on
  MAGNET_LED_REGISTER = 0b11111111; // All LEDs on
  sendRegisters();
}
