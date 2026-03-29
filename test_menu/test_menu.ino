// test_menu.ino
// Quick hardware test for the new menu architecture:
//   - 3 buttons (Difficulty+, Difficulty-, Start)
//   - 1x 74HC595 driving 8 LEDs
//
// Reuses Button class pattern and menu_loop logic from main project.
// Adjust PIN_* defines below to match your wiring.

#include <Arduino.h>

// ---------------------------------------------------------------------------
// PIN CONFIG  (match your new schematic)
// ---------------------------------------------------------------------------
#define UP_PIN          7
#define DOWN_PIN        8
#define START_PIN       9

#define DATA_PIN        13   // SER / DS
#define SHIFT_PIN       10   // SRCLK / SH_CP
#define LATCH_PIN       11   // RCLK / ST_CP
#define OE_PIN          12   // OE (active LOW)

#define DIFFICULTY_MAX  8

// ---------------------------------------------------------------------------
// Button class (same implementation as Button.h / Button.cpp)
// ---------------------------------------------------------------------------
class Button {
  int           pin;
  unsigned long lastDebounceTime;
  unsigned long debounceDelay;
  int           buttonState;
  int           lastButtonState;

public:
  Button(int pinNumber, unsigned long delayTime = 10)
    : pin(pinNumber), debounceDelay(delayTime),
      lastDebounceTime(0), buttonState(HIGH), lastButtonState(HIGH) {}

  void begin() {
    pinMode(pin, INPUT_PULLUP);
    delay(50);
    buttonState = digitalRead(pin);
    lastButtonState = buttonState;
    lastDebounceTime = millis();
  }

  bool isPressed() {
    int  reading   = digitalRead(pin);
    bool triggered = false;

    if (reading != lastButtonState)
      lastDebounceTime = millis();

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;
        if (buttonState == LOW)
          triggered = true;
      }
    }
    lastButtonState = reading;
    return triggered;
  }
};

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
int  _difficulty      = 2;
byte LED_REGISTER     = 0b00000000;

Button upButton(UP_PIN);
Button downButton(DOWN_PIN);
Button startButton(START_PIN);

// ---------------------------------------------------------------------------
// 74HC595 helpers (single chip)
// ---------------------------------------------------------------------------
void sendRegister() {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, SHIFT_PIN, MSBFIRST, LED_REGISTER);
  digitalWrite(LATCH_PIN, HIGH);
}

void ledWrite(byte bit, bool state) {
  if (state)
    LED_REGISTER |=  (1 << bit);
  else
    LED_REGISTER &= ~(1 << bit);
  sendRegister();
}

// ---------------------------------------------------------------------------
// show_difficulty: light N LEDs for current difficulty (same as menu.cpp)
// ---------------------------------------------------------------------------
void show_difficulty() {
  for (int i = 0; i < DIFFICULTY_MAX; i++)
    ledWrite(i, i < _difficulty);
}

// ---------------------------------------------------------------------------
// menu_loop: same pattern as menu.cpp, no standby
// ---------------------------------------------------------------------------
void menu_loop() {
  while (true) {
    if (upButton.isPressed()) {
      if (_difficulty < DIFFICULTY_MAX) _difficulty++;
      show_difficulty();
    }
    if (downButton.isPressed()) {
      if (_difficulty > 1) _difficulty--;
      show_difficulty();
    }
    if (startButton.isPressed()) {
      return;  // exit to "game"
    }
  }
}

// ---------------------------------------------------------------------------
// Setup / Loop
// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);

  upButton.begin();
  downButton.begin();
  startButton.begin();

  pinMode(DATA_PIN,  OUTPUT);
  pinMode(SHIFT_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(OE_PIN,    OUTPUT);
  digitalWrite(OE_PIN, LOW);  // enable outputs

  show_difficulty();
  Serial.println("Menu ready. Up/Down = difficulty, Start = confirm.");
}

void loop() {
  show_difficulty();
  menu_loop();

  // Reached here = Start was pressed
  Serial.print("Started! Difficulty = ");
  Serial.println(_difficulty);

  // Flash all LEDs 3x as "game start" confirmation
  for (int i = 0; i < 3; i++) {
    LED_REGISTER = 0xFF; sendRegister(); delay(200);
    LED_REGISTER = 0x00; sendRegister(); delay(200);
  }

  // Reset difficulty and loop back to menu
  _difficulty = 2;
}
