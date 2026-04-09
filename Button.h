#pragma once


class Button {
  private:
    int pin;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;
    int buttonState;
    int lastButtonState;

  public:
    // Constructor: sets up the button when you create it
    Button(int pinNumber, unsigned long delayTime = 10);

    // Member functions
    void  begin();
    bool  isPressed();

};
