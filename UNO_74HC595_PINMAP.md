# Arduino Uno + 3x 74HC595 + TIP120 Pin and Bit Mapping

Source of truth: `config.h` and `74hc595.cpp`.

## 1) Topology Overview

Three 74HC595 chips split across **two independent chains**:

- **Chain A** — 1 chip (standalone): menu / difficulty LEDs.
- **Chain B** — 2 chips daisy-chained: magnet drivers + magnet indicator LEDs, split by side.

Each chain has its own `DATA`, `SHIFT`, `LATCH`, and `OE` pin.

---

## 2) Arduino Uno Pin Assignment

### Buttons (use `INPUT_PULLUP`)

| Pin  | Role              | Macro               |
|------|-------------------|---------------------|
| D10  | Start             | `START_BUTTON_PIN`  |
| D11  | Difficulty down   | `DOWN_BUTTON_PIN`   |
| D12  | Difficulty up     | `UP_BUTTON_PIN`     |

### Chain A — Menu LEDs (1x 74HC595)

| Pin | Role                 | Macro              |
|-----|----------------------|--------------------|
| D2  | `SRCLK` (shift clk)  | `MENU_SHIFT_PIN`   |
| D3  | `RCLK`  (latch)      | `MENU_LATCH_PIN`   |
| D4  | `OE`    (active LOW) | `MENU_OE_PIN`      |
| D5  | `SER`   (data)       | `MENU_DATA_PIN`    |

### Chain B — Magnets + Magnet LEDs (2x 74HC595 daisy-chained)

| Pin | Role                 | Macro              |
|-----|----------------------|--------------------|
| D6  | `SER`   (data)       | `MAGNET_DATA_PIN`  |
| D7  | `OE`    (active LOW) | `MAGNET_OE_PIN`    |
| D8  | `RCLK`  (latch)      | `MAGNET_LATCH_PIN` |
| D9  | `SRCLK` (shift clk)  | `MAGNET_SHIFT_PIN` |

### Free

- `D0`, `D1` reserved for Serial (USB debug).
- `D13` free (built-in LED, unused by firmware).
- All analog pins free (`A0` used only as noise source for `randomSeed`).

---

## 3) 74HC595 Wiring

### Chain A (menu chip)

- Uno `D5`  → chip 1 `SER`
- Uno `D2`  → chip 1 `SRCLK`
- Uno `D3`  → chip 1 `RCLK`
- Uno `D4`  → chip 1 `OE`
- Tie chip 1 `MR` to `+5V` (no async reset)
- `Q7'` unused (no downstream chip)

### Chain B (magnet chain, chip 2 → chip 3)

- Uno `D6`  → chip 2 `SER`
- Uno `D9`  → chip 2 & chip 3 `SRCLK`
- Uno `D8`  → chip 2 & chip 3 `RCLK`
- Uno `D7`  → chip 2 & chip 3 `OE`
- Chip 2 `Q7'` → chip 3 `SER`
- Tie `MR` of both chips to `+5V`

### Common

- Shared `5V` and `GND` across all chips and external PSU.
- 100 nF decoupling cap per chip, close to `VCC`/`GND`.

---

## 4) Bit Map

Three firmware registers (see `Ninja.ino`):

- `MENU_LED_REGISTER`   — 8 bits → Chain A, chip 1
- `MAGNET_REGISTER`     — 8 bits → Chain B, magnets (one bit per object 0–7)
- `MAGNET_LED_REGISTER` — 8 bits → Chain B, indicator LEDs (one bit per object 0–7)

### 4.1 Chain A — Chip 1 (menu / difficulty LEDs)

`MENU_LED_REGISTER` shifted `MSBFIRST` to Q7..Q0.

| Bit | Output | Macro            | Meaning       |
|-----|--------|------------------|---------------|
| 0   | Q0     | `MENU_LED_1_PIN` | Difficulty 1  |
| 1   | Q1     | `MENU_LED_2_PIN` | Difficulty 2  |
| 2   | Q2     | `MENU_LED_3_PIN` | Difficulty 3  |
| 3   | Q3     | `MENU_LED_4_PIN` | Difficulty 4  |
| 4   | Q4     | `MENU_LED_5_PIN` | Difficulty 5  |
| 5   | Q5     | `MENU_LED_6_PIN` | Difficulty 6  |
| 6   | Q6     | `MENU_LED_7_PIN` | Difficulty 7  |
| 7   | Q7     | `MENU_LED_8_PIN` | Difficulty 8  |

### 4.2 Chain B — Byte Packing

Per `sendMagnetRegisters()` in `74hc595.cpp`:

```
sideA = ((MAGNET_REGISTER     & 0x0F) << 4) | (MAGNET_LED_REGISTER & 0x0F);
sideB = ( MAGNET_REGISTER     & 0xF0)       | ((MAGNET_LED_REGISTER >> 4) & 0x0F);
```

Shift order: `sideB` first, `sideA` second. First byte shifted propagates to the deepest chip.

- Chip 2 (closest to Arduino) ← `sideA` → objects **0–3**
- Chip 3 (deepest)            ← `sideB` → objects **4–7**

Per chip, with `MSBFIRST`:

| Output | Chip 2 (sideA)      | Chip 3 (sideB)      |
|--------|---------------------|---------------------|
| Q0     | Magnet LED obj 0    | Magnet LED obj 4    |
| Q1     | Magnet LED obj 1    | Magnet LED obj 5    |
| Q2     | Magnet LED obj 2    | Magnet LED obj 6    |
| Q3     | Magnet LED obj 3    | Magnet LED obj 7    |
| Q4     | Magnet drv  obj 0   | Magnet drv  obj 4   |
| Q5     | Magnet drv  obj 1   | Magnet drv  obj 5   |
| Q6     | Magnet drv  obj 2   | Magnet drv  obj 6   |
| Q7     | Magnet drv  obj 3   | Magnet drv  obj 7   |

Firmware bit indices (`magnetWrite(pin, state)`, `magnetLedWrite(pin, state)`) run `0..7` for objects `0..7`. Byte packing in `sendMagnetRegisters()` routes them to the right chip and output.

---

## 5) Magnet Driver Wiring (Per Channel)

Low-side switching through TIP120, one per object (8 total):

- 74HC595 magnet output → 1k–2.2k resistor → TIP120 base
- TIP120 emitter → power GND
- TIP120 collector → electromagnet `-`
- Electromagnet `+` → external PSU `+V`
- Flyback diode across coil: cathode → `+V`, anode → TIP120 collector

Must-have: common GND between Arduino and external magnet PSU.

---

## 6) Logic Convention

- Magnet bit = `1` → TIP120 ON → magnet energized → stick **held**
- Magnet bit = `0` → TIP120 OFF → magnet off → stick **released**
- Menu/magnet LED bit = `1` → LED ON

OE is active LOW: `digitalWrite(*_OE_PIN, LOW)` enables outputs, `HIGH` blanks them.

---

## 7) Startup Safe State

`init_game()` in `Ninja.ino`:

- `MAGNET_REGISTER     = 0b11111111` — all magnets ON (no accidental drops)
- `MAGNET_LED_REGISTER = 0b11111111` — all indicator LEDs ON
- `MENU_LED_REGISTER`  set by `menu_loop()` based on selected difficulty
- Both `OE` pins driven LOW in `setup()` to enable outputs after `pinMode`
