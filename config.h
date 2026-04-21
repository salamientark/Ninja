#pragma once

// PIN CONSTANTS — Buttons
#define UP_BUTTON_PIN         12
#define DOWN_BUTTON_PIN       11
#define START_BUTTON_PIN      10

// Chain A — Menu LEDs (74HC595 #1, standalone)
#define MENU_DATA_PIN         5  // SER / DS
#define MENU_OE_PIN           4  // OE (active LOW)
#define MENU_LATCH_PIN        3  // RCLK / ST_CP
#define MENU_SHIFT_PIN        2  // SRCLK / SH_CP

// Chain B — Magnets + Magnet LEDs (74HC595 #2 → #3 chained)
#define MAGNET_DATA_PIN       6   // SER / DS
#define MAGNET_OE_PIN         7   // OE (active LOW)
#define MAGNET_LATCH_PIN      8   // RCLK / ST_CP
#define MAGNET_SHIFT_PIN      9   // SRCLK / SH_CP

// PROGRAM CONSTANTS
#define DIFFICULTY_MAX        8
#define OBJ_NBR               8
