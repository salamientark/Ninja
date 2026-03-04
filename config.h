#pragma once

// PIN CONSTANTS
#define DOWN_BUTTON_PIN       1
#define UP_BUTTON_PIN         2
#define START_BUTTON_PIN      3
#define DIFFICULTY_5_LED_PIN  13
#define DIFFICULTY_4_LED_PIN  12
#define DIFFICULTY_3_LED_PIN  11
#define DIFFICULTY_2_LED_PIN  10
#define DIFFICULTY_1_LED_PIN  9

// 74HC595
#define DATA_LOCK_PIN         7   // (ST_CP) -> latch pin
#define DATA_SHIFT_PIN        6   // (SH_CP) -> clock pin
#define DATA_PIN              5   // (DS) -> data pin
#define DATA_OUTPUT_PIN       4   // (OE) -> output enable pin (active LOW)

#define MAGNET_1_PIN          1
#define MAGNET_2_PIN          2
#define MAGNET_3_PIN          3
#define MAGNET_4_PIN          4
#define MAGNET_5_PIN          5
#define MAGNET_6_PIN          6
#define MAGNET_7_PIN          7

// PROGRAM CONSTANTS
#define DIFFICULTY_MAX        5
#define OBJ_NBR               7
