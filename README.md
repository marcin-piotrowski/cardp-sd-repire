# Cardputer SD Repair Toolkit

PlatformIO-based firmware and diagnostics to destructively test and reformat SD cards for the M5Stack Cardputer (StampS3).

> **Danger:** This tool wipes SD cards. Back up any data first. Always point it at the correct card.

## Repository layout

- `cardfix-core/`: Platform-neutral library for raw SD I/O, MBR/FAT32 layout, and diagnostics helpers.
- `tests/`: PlatformIO unit tests that exercise `cardfix-core` using the native runtime.
- `firmware/`: PlatformIO project targeting `m5stack-stamps3` that mounts the SD slot, runs diagnostics, and performs a destructive FAT32 format with a fresh MBR.

## Building and testing

### Unit tests (native)

```bash
pio test -e native
```

### Firmware build (Cardputer)

```bash
cd firmware
pio run -e m5stack-stamps3
# Firmware binary: firmware/.pio/build/m5stack-stamps3/firmware.bin
```

## Usage on device

1. Flash the firmware binary to the Cardputer using PlatformIO or `esptool.py`.
2. Insert the SD card **you intend to erase**.
3. Power on; the firmware runs diagnostics, writes a new MBR, and formats a FAT32 volume automatically.
4. Wait for the "Complete" message before removing the card.

## Safety notes

- The formatter overwrites sector 0 and early sectors; **data loss is guaranteed**.
- Use only on cards you can afford to wipe.
- If diagnostics fail, inspect wiring, reseat the card, and try another SD.
