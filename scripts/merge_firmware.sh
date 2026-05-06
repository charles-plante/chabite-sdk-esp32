#!/usr/bin/env bash
set -euo pipefail

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_DIR/.pio/build/esp32-c3-devkitm-1"
OUTPUT="$PROJECT_DIR/chabite-firmware.bin"
ESPTOOL="$HOME/.platformio/packages/tool-esptoolpy/esptool.py"
PYTHON="$HOME/.platformio/penv/bin/python"

if [[ ! -f "$BUILD_DIR/firmware.bin" ]]; then
  echo "firmware.bin not found — run 'pio run' first" >&2
  exit 1
fi

"$PYTHON" "$ESPTOOL" \
  --chip esp32c3 merge_bin -o "$OUTPUT" \
  --flash_mode dio --flash_freq 80m --flash_size 4MB \
  0x0000  "$BUILD_DIR/bootloader.bin" \
  0x8000  "$BUILD_DIR/partitions.bin" \
  0x10000 "$BUILD_DIR/firmware.bin"

echo "Merged binary: $OUTPUT (flash @ 0x0)"
