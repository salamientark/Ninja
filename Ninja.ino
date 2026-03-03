#include "Button.h"

/* ************************************************************************** */
/*                                GLOBAL & CONSTANTS                          */
/* ************************************************************************** */
// PIN CONSTANTS
#define START_BUTTON_PIN		0
#define DOWN_BUTTON_PIN			1
#define UP_BUTTON_PIN			2
#define DIFFICULTY_5_LED_PIN	13
#define DIFFICULTY_4_LED_PIN	12
#define DIFFICULTY_3_LED_PIN	11
#define DIFFICULTY_2_LED_PIN	10
#define DIFFICULTY_1_LED_PIN	9

// PROGRAM CONSTANTS
#define DIFFICULTY_MAX			5


// Global variables
int _difficulty = 2;

unsigned long T1 = 0, T2 = 0;
uint8_t TimeInterval = 5; // 5ms

/* ************************************************************************** */
/*                                     SETUP                                  */
/* ************************************************************************** */

Button upButton(UP_BUTTON_PIN);
Button downButton(DOWN_BUTTON_PIN);
Button startButton(START_BUTTON_PIN);

void setup() {
  // INPUTS
  upButton.begin();
  downButton.begin();
  startButton.begin();

  // OUTPUTS
  pinMode(DIFFICULTY_5_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_4_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_3_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_2_LED_PIN, OUTPUT);
  pinMode(DIFFICULTY_1_LED_PIN, OUTPUT);
}

/* ************************************************************************** */
/*                                 MAIN LOOP                                  */
/* ************************************************************************** */

void loop() {
  // T2 = millis();
  // if( (T2-T1) >= TimeInterval) // Every 5ms
  // {
  //   // Read The Electromagnet Enable Button State
  //   //if (debounce()) {
  //   //  digitalWrite(EM_PIN, !digitalRead(EM_PIN)); // Toggle (Enable/Disable)
  //   //}
  //   
  //   // Difficulty increase
  //   if (debounce(UP_BUTTON_PIN)) {
  //     _difficulty++;
  //     if (_difficulty > 5) {
  //       _difficulty = 5;
  //     }
  //   }
  //   // Difficulty decrease
  //   //if (debounce(DOWN_BUTTON_PIN)) {
  //   //  _difficulty--;
  //   //  if (_difficulty < 1) {
  //   //    _difficulty = 1;
  //   //  }
  //   //}
  //   show_difficulty();
  //   T1 = millis();
  // }
  if (upButton.isPressed()) {
    _difficulty++;
    if (_difficulty > DIFFICULTY_MAX) {
      _difficulty = DIFFICULTY_MAX;
    }
  }
  if (downButton.isPressed()) {
    _difficulty--;
    if (_difficulty < 1) {
      _difficulty = 1;
    }
  }
  show_difficulty();
}

/* ************************************************************************** */
/*                                 MAIN LOOP                                  */
/* ************************************************************************** */

void	show_difficulty(void) {
  int counter = 0;
  while (counter < DIFFICULTY_MAX) {
    digitalWrite(DIFFICULTY_1_LED_PIN + counter, counter < _difficulty);
  	counter++;
  }
}

/* ************************************************************************** */
/*                              CUSTOM FUNCTIONS                              */
/* ************************************************************************** */

void  game_loop() {

}



bool debounce(int pin)
{
  static uint16_t btnState = 0;
  btnState = (btnState<<1) | (!digitalRead(pin));
  return (btnState == 0xFFF0);
}

