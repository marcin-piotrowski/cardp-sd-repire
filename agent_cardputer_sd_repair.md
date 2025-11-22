# Cardputer SD Repair Specification

This repository provides:

- `cardfix-core/`: A portable C++17 library that performs raw SD sector I/O via an abstract `RawCard` interface, writes an MBR + FAT32 boot sector, and runs destructive diagnostics.
- `tests/`: PlatformIO native-unit tests exercising the library without hardware by using the in-memory `MemoryCard` backend.
- `firmware/`: PlatformIO project for the M5Stack Cardputer (StampS3, Arduino framework) that mounts the SD slot, executes diagnostics, and reformats the card using `cardfix-core`. Output binary: `firmware/.pio/build/m5stack-stamps3/firmware.bin`.
- `.github/workflows/ci.yml`: CI that runs native tests and builds the firmware.
- `README.md`: Usage instructions and **strong data loss warnings**.

Responsibilities are separated: low-level SD I/O (`sector_io`), filesystem layout and formatting (`format`), diagnostics (`diagnostics`), and firmware UI/hardware glue (`sd_interface`, `ui`, and `main.cpp`).
