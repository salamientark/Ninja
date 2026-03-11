#pragma once

// PIN CONSTANTS
#define DOWN_BUTTON_PIN       4
#define UP_BUTTON_PIN         2
#define START_BUTTON_PIN      3
// Chip 1 — menu/difficulty LEDs (bit positions 0–7)
#define MENU_LED_1_PIN        0
#define MENU_LED_2_PIN        1
#define MENU_LED_3_PIN        2
#define MENU_LED_4_PIN        3
#define MENU_LED_5_PIN        4
#define MENU_LED_6_PIN        5
#define MENU_LED_7_PIN        6
#define MENU_LED_8_PIN        7

// 74HC595
#define DATA_LOCK_PIN         8  // (RCLK / ST_CP) -> latch pin
#define DATA_SHIFT_PIN        7  // (SRCLK / SH_CP) -> clock pin
#define DATA_PIN              6  // (SER / DS) -> data pin
#define DATA_OUTPUT_PIN       5   // (OE) -> output enable pin (active LOW)

// Chip 2 — electromagnets (Q0–Q7)
#define MAGNET_1_PIN          0
#define MAGNET_2_PIN          1
#define MAGNET_3_PIN          2
#define MAGNET_4_PIN          3
#define MAGNET_5_PIN          4
#define MAGNET_6_PIN          5
#define MAGNET_7_PIN          6
#define MAGNET_8_PIN          7

// Chip 3 — magnet indicator LEDs (Q0–Q7)
#define MAGNET_LED_1_PIN      0
#define MAGNET_LED_2_PIN      1
#define MAGNET_LED_3_PIN      2
#define MAGNET_LED_4_PIN      3
#define MAGNET_LED_5_PIN      4
#define MAGNET_LED_6_PIN      5
#define MAGNET_LED_7_PIN      6
#define MAGNET_LED_8_PIN      7

// PROGRAM CONSTANTS
#define DIFFICULTY_MAX        8
#define OBJ_NBR               8
