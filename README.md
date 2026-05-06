# Ninja Reflex Game

A reflex-training arcade game for Arduino Uno. Eight objects hang from electromagnets and drop one by one — catch them before they hit the floor. Difficulty scales from leisurely warm-ups to chaotic multi-drops with distraction LEDs.

## Features

- **8-channel electromagnet release** driven by a TIP120 array
- **8 difficulty levels** with tunable timing, multi-drops, and fake-LED distractions
- **Dual 74HC595 chains** — one for menu LEDs, one daisy-chained pair for magnets + indicator LEDs
- **Non-blocking state machine** — drops, animations, and standby all run cooperatively off `millis()`
- **Standby mode** with idle animation after 60s of inactivity
- **Single `.ino` sketch** — no external libraries required beyond the Arduino core

## Hardware

| Part                | Qty | Notes                                       |
|---------------------|-----|---------------------------------------------|
| Arduino Uno         | 1   | Any ATmega328P-compatible board             |
| 74HC595             | 3   | 1 for menu, 2 daisy-chained for magnets+LEDs|
| TIP120 NPN Darlington | 8 | Low-side switch per electromagnet           |
| Electromagnet       | 8   | Driven from external PSU                    |
| 1N4007 flyback diode| 8   | Across each coil                            |
| Push button         | 3   | Up / Down / Start                           |
| LEDs + resistors    | 16  | 8 difficulty + 8 magnet indicators          |

> [!IMPORTANT]
> Magnets need an **external PSU** sharing GND with the Arduino. Don't try to power coils from the 5V rail.

Full pinout, bit mapping, and wiring conventions: see [`UNO_74HC595_PINMAP.md`](./UNO_74HC595_PINMAP.md).

## Project Layout

```
Ninja.ino       — entry point: setup(), loop(), init_game()
config.h        — pin macros, OBJ_NBR, DIFFICULTY_MAX
74hc595.{h,cpp} — register helpers + dual-chain shiftOut
menu.{h,cpp}    — difficulty selection + standby trigger
standby.{h,cpp} — idle animation + difficulty-8 confuse animation
game_loop.{h,cpp} — drop state machine, per-difficulty timing
Button.{h,cpp}  — debounced edge-triggered button
amalgamate.py   — concat all sources into one file (LLM context helper)
```

## Build & Flash

Using `arduino-cli`:

```bash
arduino-cli compile --fqbn arduino:avr:uno .
arduino-cli upload  --fqbn arduino:avr:uno -p /dev/ttyACM0 .
```

Or open `Ninja.ino` in the Arduino IDE and hit **Upload**.

## How to Play

1. Power on. Difficulty LEDs show current level.
2. **Up / Down** to change difficulty (1–8).
3. **Start** to begin. All magnets engage; hang your objects.
4. Watch the indicator LEDs — magnets release one (or several) at a time.
5. Catch objects before they hit the floor.

### Difficulty Curve

| Lvl | Drop interval (ms) | LED warning | Multi-drop | Fake LEDs |
|-----|--------------------|-------------|------------|-----------|
| 1   | 2000               | 1000 ms     | —          | —         |
| 2   | 1700               | 700 ms      | —          | —         |
| 3   | 1500               | 400 ms      | —          | —         |
| 4   | 1500               | none        | —          | —         |
| 5   | 1000–1800          | none        | 1×         | —         |
| 6   | 700–1300           | none        | 1–2×       | —         |
| 7   | 500–1100           | none        | 2–3×       | yes (300ms)|
| 8   | 200–600            | none        | 3–4×       | confuse anim |

> [!NOTE]
> Difficulty 8 swaps to a tighter loop (`game_loop_8`) with the confuse animation overlay.

## Architecture Notes

- **Three byte registers** (`MENU_LED_REGISTER`, `MAGNET_REGISTER`, `MAGNET_LED_REGISTER`) hold desired hardware state. `sendRegisters()` flushes them to the chips.
- **Drop loop** is a 5-state machine (`DROP_FAKE_LEDS → DROP_LED_ON → DROP_RELEASE → DROP_CLEANUP → DROP_WAIT`) — no `delay()` on the hot path, so animations and inputs stay responsive.
- **Object indices** are 0–7; `obj_list[]` is shuffled per game so drop order is randomised.
- **Magnet bit = 1 → coil energised → object held.** Releasing means clearing the bit.

## License

See repository root for license terms.
