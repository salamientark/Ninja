#pragma once

// PIN CONSTANTS — Buttons
#define UP_BUTTON_PIN         11
#define DOWN_BUTTON_PIN       10
#define START_BUTTON_PIN      9

// Chain A — Menu LEDs (74HC595 #1, standalone)
#define MENU_DATA_PIN         4  // SER / DS
#define MENU_OE_PIN           3  // OE (active LOW)
#define MENU_LATCH_PIN        2  // RCLK / ST_CP
#define MENU_SHIFT_PIN        1  // SRCLK / SH_CP

// Chip 1 — menu/difficulty LEDs (bit positions 0–7)
#define MENU_LED_1_PIN        0
#define MENU_LED_2_PIN        1
#define MENU_LED_3_PIN        2
#define MENU_LED_4_PIN        3
#define MENU_LED_5_PIN        4
#define MENU_LED_6_PIN        5
#define MENU_LED_7_PIN        6
#define MENU_LED_8_PIN        7

// Chain B — Magnets + Magnet LEDs (74HC595 #2 → #3 chained)
#define MAGNET_DATA_PIN       5   // SER / DS
#define MAGNET_OE_PIN         6   // OE (active LOW)
#define MAGNET_LATCH_PIN      7   // RCLK / ST_CP
#define MAGNET_SHIFT_PIN      8   // SRCLK / SH_CP

// PROGRAM CONSTANTS
#define DIFFICULTY_MAX        8
#define OBJ_NBR               8
