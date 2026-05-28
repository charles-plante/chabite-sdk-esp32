#pragma once

#include <stddef.h>
#include <stdint.h>

// Buffer sizes (including null terminator).
#define ROD_ID_LEN 7        // 6 hex chars + '\0'
#define ROD_DEVICE_NAME_LEN 15  // "Chabite-XXXXXX" + '\0'

// Write the last 3 bytes of `mac` as uppercase hex into `out`.
// `out` must have room for ROD_ID_LEN bytes. Truncates safely if not.
void formatRodId(const uint8_t mac[6], char *out, size_t outSize);

// Write "Chabite-<rodId>" into `out`.
// `out` must have room for ROD_DEVICE_NAME_LEN bytes. Truncates safely if not.
void buildDeviceName(const char *rodId, char *out, size_t outSize);
