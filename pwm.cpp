#include "pwm.h"
#include "config.h"

// Timer1 Fast PWM, mode 14 (TOP = ICR1), prescaler = 8.
// f_pwm = 16MHz / (8 * (1 + TOP)) = 16MHz / (8 * 501) ~= 3992 Hz.
// ~4kHz -> 250us period. Safe for 1N4007 flyback (trr ~2-5us, 50x margin).
// Raise toward prescaler=1 + smaller TOP later (needs UF4007) for inaudible ~20kHz.
#define MAGNET_PWM_TOP  500

void magnet_pwm_init() {
  // Write PORT bit HIGH first (enables pullup while still INPUT), then flip to OUTPUT.
  // Avoids a few-us LOW glitch between pinMode(OUTPUT) and digitalWrite(HIGH).
  digitalWrite(MAGNET_OE_PIN, HIGH);
  pinMode(MAGNET_OE_PIN, OUTPUT);

  TCCR1A = _BV(COM1A1) | _BV(WGM11);            // non-inverting, Fast PWM low bits
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11); // Fast PWM mode 14, prescaler = 8
  ICR1   = MAGNET_PWM_TOP;
  OCR1A  = MAGNET_PWM_TOP;                      // 0% duty after invert = OE HIGH = disabled
}

void magnet_pwm_set_duty(byte duty) {
  OCR1A = (uint16_t)MAGNET_PWM_TOP - ((uint32_t)MAGNET_PWM_TOP * duty / 255);
}

void magnet_pwm_disable() {
  OCR1A = MAGNET_PWM_TOP;
}
