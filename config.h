#pragma once

// PIN CONSTANTS
#define DOWN_BUTTON_PIN       4
#define UP_BUTTON_PIN         2
#define START_BUTTON_PIN      3
#define DIFFICULTY_5_LED_PIN  13
#define DIFFICULTY_4_LED_PIN  12
#define DIFFICULTY_3_LED_PIN  11
#define DIFFICULTY_2_LED_PIN  10
#define DIFFICULTY_1_LED_PIN  9

// 74HC595
#define DATA_LOCK_PIN         8  // (RCLK / ST_CP) -> latch pin
#define DATA_SHIFT_PIN        7  // (SRCLK / SH_CP) -> clock pin
#define DATA_PIN              6  // (SER / DS) -> data pin
#define DATA_OUTPUT_PIN       5   // (OE) -> output enable pin (active LOW)

// Extended output constants (Q0–Q2 of the 74HC595)
#define EXT_PIN_1             0   // Q0
#define EXT_PIN_2             1   // Q1
#define EXT_PIN_3             2   // Q2
#define EXT_PIN_4             3   // Q3
#define EXT_PIN_5             4   // Q4
#define EXT_PIN_6             5   // Q5
#define EXT_PIN_7             6   // Q6
#define EXT_PIN_8             7   // Q7

#define MAGNET_1_PIN          0
#define MAGNET_2_PIN          1
#define MAGNET_3_PIN          2
#define MAGNET_4_PIN          3
#define MAGNET_5_PIN          4
#define MAGNET_6_PIN          5
#define MAGNET_7_PIN          6
#define MAGNET_8_PIN          7

// PROGRAM CONSTANTS
#define DIFFICULTY_MAX        5
#define OBJ_NBR               7
