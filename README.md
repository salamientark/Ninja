# Ninja Reflex Game

A reflex-training arcade game for the Arduino Uno. Eight objects hang from electromagnets and drop one by one — catch them before they hit the floor. Difficulty scales from leisurely warm-ups to chaotic multi-drops with distraction LEDs and a screen-of-noise "confuse" overlay.

## Features

- **8-channel electromagnet release** — low-side switched through a TIP120 array
- **8 difficulty levels** in a single PROGMEM table: variable warning windows, tempo acceleration, multi-drops, fake-outs, and confuse animations
- **Dual 74HC595 chains** — one standalone chip for menu LEDs, a daisy-chained pair for magnets + indicator LEDs
- **Non-blocking state machine** — drops, animations, and standby all run cooperatively off `millis()`; no `delay()` on the hot path
- **Standby mode** — idle animation after 60 s of menu inactivity, with a separate confuse animation for high difficulty
- **No external libraries** — Arduino core only

## Hardware

| Part                  | Qty | Notes                                          |
|-----------------------|-----|------------------------------------------------|
| Arduino Uno           | 1   | Any ATmega328P-compatible board                |
| 74HC595               | 3   | 1 for menu, 2 daisy-chained for magnets + LEDs |
| TIP120 NPN Darlington | 8   | One low-side switch per electromagnet          |
| Electromagnet         | 8   | Driven from an external PSU                    |
| 1N4007 flyback diode  | 8   | One across each coil                           |
| Push button           | 3   | Up / Down / Start                              |
| LEDs + resistors      | 16  | 8 difficulty + 8 magnet indicators             |

> [!IMPORTANT]
> Magnets need an **external PSU** sharing GND with the Arduino. Do not power coils from the 5 V rail.

Full pinout, byte packing, and wiring conventions: see [`UNO_74HC595_PINMAP.md`](./UNO_74HC595_PINMAP.md).

## Project Layout

```
Ninja.ino         entry point: setup(), loop(), init_game()
config.h          pin macros, OBJ_NBR, DIFFICULTY_MAX
74hc595.{h,cpp}   register helpers + dual-chain shiftOut
menu.{h,cpp}      difficulty selection + standby trigger
standby.{h,cpp}   idle animation + confuse animation
game_loop.{h,cpp} PROGMEM difficulty table + drop state machine
Button.{h,cpp}    debounced, edge-triggered button
amalgamate.py     concat all sources into one file (LLM context helper)
```

## Build & Flash

Using `arduino-cli`:

```bash
arduino-cli compile --fqbn arduino:avr:uno .
arduino-cli upload  --fqbn arduino:avr:uno -p /dev/ttyACM0 .
```

Or open `Ninja.ino` in the Arduino IDE and hit **Upload**.

## How to Play

1. Power on. Difficulty LEDs show the current level (default 2).
2. **Up / Down** to change difficulty (1–8).
3. **Start** to begin. All magnets engage; hang your objects.
4. Watch the indicator LEDs — magnets release one or several at a time.
5. Catch objects before they hit the floor.

After 60 s idle in the menu, the board enters standby animation. The first **Start** press wakes it without starting a game.

### Difficulty Curve

Derived from `DIFFICULTY_TABLE` in `game_loop.cpp`. *Wait* is the gap between drops; `a→b` means the gap accelerates across the round.

| Lvl | Warn (ms) | Wait (ms)     | Multi-drop          | Fake-out | Confuse  |
|-----|-----------|---------------|---------------------|----------|----------|
| 1   | 800       | 1700          | —                   | —        | —        |
| 2   | 500       | 1250–1550     | —                   | —        | —        |
| 3   | 150–350   | 1050–1350     | —                   | —        | —        |
| 4   | 0–200     | 800–1200      | —                   | 20%      | —        |
| 5   | none      | 1100 → 700    | 0–1 drops @ 30%     | —        | —        |
| 6   | none      | 900 → 500     | 1–2 drops @ 50%     | —        | —        |
| 7   | none      | 700 → 400     | 2–3 drops @ 50%     | —        | 300 ms   |
| 8   | none      | 400 → 200     | 3–4 drops @ 50% + a 3-drop | — | 150 ms   |

- **Warn** — indicator LED lights this long before the magnet releases. `0` means no LED tell.
- **Fake-out** — a magnet LED flashes but nothing drops (floored at 150 ms so it is perceptible).
- **Confuse** — random distraction LEDs flash on every drop; a full-screen confuse animation also overlays standby at high difficulty.

> [!NOTE]
> All eight levels run through the same state machine. The difficulty-8 path adds the 3-drop event and the tightest tempo, but no longer uses a separate loop.

## Architecture Notes

- **Three byte registers** hold desired hardware state: `MENU_LED_REGISTER` (Chain A), `MAGNET_REGISTER` and `MAGNET_LED_REGISTER` (Chain B). `sendRegisters()` flushes all of them to the chips atomically.
- **PROGMEM config table** — each level is a `DifficultyConfig` struct copied into a RAM-resident `_cfg` at game start. Adding or tuning a level is a one-row edit.
- **Drop state machine** — `DROP_FAKE_LEDS → DROP_FAKEOUT → DROP_LED_ON → DROP_RELEASE → DROP_CLEANUP → DROP_WAIT`, all driven off `millis()` so inputs and animations stay responsive.
- **Object order** is randomized: `obj_list[0..7]` is shuffled per game.
- **Magnet bit = 1 → coil energized → object held.** Releasing clears the bit.
