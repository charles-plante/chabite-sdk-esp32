# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

RodAlert ("Chabite") is ESP32-C3 firmware (Arduino framework, PlatformIO) for a
fishing-rod alert device. It exposes a single BLE GATT service that phones
connect to; the device notifies connected clients of events. Multiple phones
can pair to one device at the same time — advertising is restarted on every
connect/disconnect so additional clients can keep discovering it.

## Commands

```bash
pio run                 # build firmware for esp32-c3-devkitm-1 (default env)
pio run -t upload       # flash to a connected board
pio device monitor      # serial monitor @ 115200 baud
pio test -e native      # run host unit tests (Unity)
scripts/merge_firmware.sh   # merge bootloader+partitions+firmware into chabite-firmware.bin (run `pio run` first)
```

Run a single native test by editing the `RUN_TEST(...)` list in the test's
`main()` — the Unity runner has no per-test CLI filter.

## Architecture

Two build environments in `platformio.ini`:
- `esp32-c3-devkitm-1` — the real firmware target (default for `pio run`). It
  ignores `test_native` so host-only tests never compile as firmware.
- `native` — host-PC build used **only** for unit tests (`test_filter = test_native`).

Pure logic lives in libraries under `lib/` so it can be unit-tested on the host
without the Arduino/BLE stack. `lib/RodId` holds the device-naming logic
(`formatRodId`, `buildDeviceName`) — keep these free of Arduino dependencies so
the `native` env keeps compiling. `src/main.cpp` is the firmware entry point and
is the only place that pulls in `<Arduino.h>` and the BLE headers.

Device identity: the rod ID is the last 3 bytes of the BT MAC as uppercase hex
(e.g. `DEADBE`), and the BLE device/advertised name is `Chabite-<rodId>`.

BLE contract (defined in `src/main.cpp`):
- Service UUID `1fa3fdf2-0a5c-40eb-b520-6d31560637ab`
- Characteristic UUID `44f675d0-c42f-4a7a-9e9d-6acb50c9d162` (READ | NOTIFY, with a BLE2902 CCCD)

`setup()`/`loop()` interaction happens through `volatile` flags set in BLE
callbacks and acted on in `loop()` — the callbacks run in the BLE task context,
so keep them minimal and defer real work to the loop.
