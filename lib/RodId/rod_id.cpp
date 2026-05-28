#include "rod_id.h"

#include <stdio.h>

void formatRodId(const uint8_t mac[6], char *out, size_t outSize) {
  if (out == nullptr || outSize == 0) {
    return;
  }
  snprintf(out, outSize, "%02X%02X%02X", mac[3], mac[4], mac[5]);
}

void buildDeviceName(const char *rodId, char *out, size_t outSize) {
  if (out == nullptr || outSize == 0) {
    return;
  }
  snprintf(out, outSize, "Chabite-%s", rodId == nullptr ? "" : rodId);
}
