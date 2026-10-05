# embedded

Bare-metal C practice on microcontrollers.

## Layout

```
external/pico-sdk/   Raspberry Pi Pico SDK (git submodule, pinned to a release)
pico/                Raspberry Pi Pico (RP2040) exercises, one folder each
  01-blink/          Blink the onboard LED with direct register writes
```

## One-time setup (macOS)

```bash
brew install --cask gcc-arm-embedded   # ARM cross-compiler + gdb
git submodule update --init            # after a fresh clone
```

## Build

```bash
cmake -S pico -B pico/build
cmake --build pico/build -j
```

Output: `pico/build/01-blink/blink.uf2` (and `blink.elf` for the debugger).

## Flash (drag and drop)

1. Hold the **BOOTSEL** button on the Pico while plugging in USB.
2. A drive named `RPI-RP2` appears.
3. Copy the `.uf2` onto it; the Pico reboots and runs it.

```bash
cp pico/build/01-blink/blink.uf2 /Volumes/RPI-RP2/
```

## Exercise plan

1. `01-blink`: GPIO via registers, timer-based delay
2. Button input with debounce
3. UART output via the Debug Probe (and watch it on the logic analyzer)
4. Interrupts: timer and GPIO ISRs, sharing data with the main loop safely
5. I2C driver for the LCD1602, written from the datasheet
6. ADC: read the thermistor and potentiometer
7. PWM tones on the passive buzzer
8. Own startup code and linker script (drop the SDK runtime)
9. Stretch: I2S audio out (MAX98357A)
