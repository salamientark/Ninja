#pragma once
#include <Arduino.h>

// Magnet PWM on MAGNET_OE_PIN (D9 / Timer1 OC1A).
// Active-LOW: duty 0..255 -> 0% = OE high (off), 255 = OE low (fully on).

void magnet_pwm_init();             // configure Timer1, leave OE disabled
void magnet_pwm_set_duty(byte duty);// 0..255
void magnet_pwm_disable();          // force OE HIGH (0% duty)
