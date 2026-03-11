#include "standby.h"
#include "74hc595.h"
#include <Arduino.h>

#define STANDBY_FRAME_MS  80    // ms between animation frames
#define ANIM_COUNT        4     // number of animations
#define ANIM_DURATION_MS  8000  // ms before switching animation

static unsigned long lastFrameTime = 0;
static unsigned long animStartTime = 0;
static int           currentAnim   = 0;
static int           frameIndex    = 0;

static void anim_chase(void);
static void anim_knight_rider(void);
static void anim_cascade(void);
static void anim_alternate(void);

void standby_reset() {
    MAGNET_LED_REGISTER = 0x00;
    sendRegisters();
    lastFrameTime = millis();
    animStartTime = millis();
    currentAnim   = 0;
    frameIndex    = 0;
}

void standby_tick() {
    unsigned long now = millis();

    if (now - animStartTime >= ANIM_DURATION_MS) {
        currentAnim = (currentAnim + 1) % ANIM_COUNT;
        animStartTime = now;
        frameIndex = 0;
        MAGNET_LED_REGISTER = 0x00;
        sendRegisters();
    }

    if (now - lastFrameTime >= STANDBY_FRAME_MS) {
        lastFrameTime = now;
        switch (currentAnim) {
            case 0: anim_chase();        break;
            case 1: anim_knight_rider(); break;
            case 2: anim_cascade();      break;
            case 3: anim_alternate();    break;
        }
        frameIndex++;
    }
}

// Single lit LED scrolls across 8 positions (magnet LEDs only), cycling
static void anim_chase(void) {
    MAGNET_LED_REGISTER = (1 << (frameIndex % 8));
    sendRegisters();
}

// Single LED bounces back and forth across 8 positions
static void anim_knight_rider(void) {
    int period = 14;  // 8 positions * 2 - 2 = 14 steps for full bounce
    int step   = frameIndex % period;
    int pos    = (step < 8) ? step : (period - step);
    MAGNET_LED_REGISTER = (1 << pos);
    sendRegisters();
}

// LEDs fill up one by one then clear, across 8 positions
static void anim_cascade(void) {
    int cycle = frameIndex % 10;  // 8 fill + 2 blank frames
    MAGNET_LED_REGISTER = (cycle < 8) ? (byte)((1 << (cycle + 1)) - 1) : 0x00;
    sendRegisters();
}

// Odd/even bits alternate on/off on magnet LEDs
static void anim_alternate(void) {
    MAGNET_LED_REGISTER = (frameIndex % 2 == 0) ? 0xAA : 0x55;
    sendRegisters();
}
