#include "config.h"
#include "Button.h"
#include "74hc595.h"
#include "menu.h"
#include "game_loop.h"
#include "pwm.h"

/* ************************************************************************** */
/*                                GLOBAL & CONSTANTS                          */
/* ************************************************************************** */
// OBJ_NBR and pin/difficulty constants are defined in config.h

// Global variables
int   _difficulty = 2;
byte  obj_list[OBJ_NBR];
byte  MENU_LED_REGISTER   = 0b00000000;  // Chain A — menu/difficulty LEDs
byte  MAGNET_REGISTER     = 0b00000000;  // Chain B — electromagnet outputs
byte  MAGNET_LED_REGISTER = 0b00000000;  // Chain B — magnet indicator LEDs

/* ************************************************************************** */
/*                                     SETUP                                  */
/* ************************************************************************** */

Button upButton(UP_BUTTON_PIN);
Button downButton(DOWN_BUTTON_PIN);
Button startButton(START_BUTTON_PIN);

void setup() {
  // RANDOM SEED
  randomSeed(analogRead(0));

  // ---- STAGE 1: disable all outputs BEFORE anything can glitch ----
  // Magnet chain: OE HIGH immediately (also sets pinMode).
  magnet_pwm_init();
  // Menu chain: keep OE HIGH until we have clean data latched.
  pinMode(MENU_OE_PIN, OUTPUT);
  digitalWrite(MENU_OE_PIN, HIGH);

  // ---- STAGE 2: configure shift register pins ----
  pinMode(MENU_DATA_PIN,    OUTPUT);
  pinMode(MENU_SHIFT_PIN,   OUTPUT);
  pinMode(MENU_LATCH_PIN,   OUTPUT);
  pinMode(MAGNET_DATA_PIN,  OUTPUT);
  pinMode(MAGNET_SHIFT_PIN, OUTPUT);
  pinMode(MAGNET_LATCH_PIN, OUTPUT);

  // ---- STAGE 3: latch all zeros into every chip so outputs start clean ----
  MENU_LED_REGISTER   = 0;
  MAGNET_REGISTER     = 0;
  MAGNET_LED_REGISTER = 0;
  sendRegisters();

  delay(20); // let rails settle before enabling outputs

  // ---- STAGE 4: enable outputs ----
  digitalWrite(MENU_OE_PIN, LOW);          // menu LEDs (all still 0)
  magnet_pwm_set_duty(MAGNET_HOLD_DUTY);   // magnets enabled at configured duty (all bits still 0)

  // ---- STAGE 5: inputs ----
  upButton.begin();
  downButton.begin();
  startButton.begin();
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
    obj_list[i] = i; // 0-based object indices

  _difficulty = 2; // Reset difficulty to default

  // Activate magnets
  MAGNET_REGISTER = 0b11111111; // All magnets on
  MAGNET_LED_REGISTER = 0b11111111; // All LEDs on
  sendRegisters();
}
