#include "Button.h"
#include <Arduino.h>


Button::Button(int pinNumber, unsigned long delayTime) {
  pin = pinNumber;
  debounceDelay = delayTime;
  lastDebounceTime = 0;
  buttonState = HIGH;      // Assuming internal pull-up resistor
  lastButtonState = HIGH;
}


void  Button::begin() {
  pinMode(pin, INPUT_PULLUP); // Uses Arduino's built-in resistor
  delay(50);                  // Let pullup charge any debouncing capacitors
  buttonState = digitalRead(pin);
  lastButtonState = buttonState;
  lastDebounceTime = millis();
}


    // Call this continuously in loop() to check for a clean press
bool  Button::isPressed() {
  int reading = digitalRead(pin);
  bool wasPressed = false;

  // If the switch changed (due to noise or pressing)
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Reset the timer
  }

  // If enough time has passed to ignore the bouncing
  if ((millis() - lastDebounceTime) > debounceDelay) {

    // If the state has actually changed to a new state
    if (reading != buttonState) {
      buttonState = reading;

      // Only trigger if the new state is LOW (button pressed down)
      if (buttonState == LOW) {
        wasPressed = true;
      }
    }
  }

  lastButtonState = reading; // Save the reading for next time
  return wasPressed;
}
