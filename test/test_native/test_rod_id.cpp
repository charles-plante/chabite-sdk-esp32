#include <unity.h>
#include <string.h>

#include "rod_id.h"

void setUp(void) {}
void tearDown(void) {}

// formatRodId -----------------------------------------------------------------

void test_formatRodId_uses_last_three_bytes(void) {
  uint8_t mac[6] = {0x11, 0x22, 0x33, 0xAB, 0xCD, 0xEF};
  char out[ROD_ID_LEN];
  formatRodId(mac, out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("ABCDEF", out);
}

void test_formatRodId_ignores_first_three_bytes(void) {
  uint8_t a[6] = {0x00, 0x00, 0x00, 0x12, 0x34, 0x56};
  uint8_t b[6] = {0xFF, 0xFF, 0xFF, 0x12, 0x34, 0x56};
  char outA[ROD_ID_LEN];
  char outB[ROD_ID_LEN];
  formatRodId(a, outA, sizeof(outA));
  formatRodId(b, outB, sizeof(outB));
  TEST_ASSERT_EQUAL_STRING(outA, outB);
  TEST_ASSERT_EQUAL_STRING("123456", outA);
}

void test_formatRodId_pads_with_zero(void) {
  uint8_t mac[6] = {0, 0, 0, 0x01, 0x02, 0x03};
  char out[ROD_ID_LEN];
  formatRodId(mac, out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("010203", out);
}

void test_formatRodId_all_zero(void) {
  uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
  char out[ROD_ID_LEN];
  formatRodId(mac, out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("000000", out);
}

void test_formatRodId_all_ff(void) {
  uint8_t mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  char out[ROD_ID_LEN];
  formatRodId(mac, out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("FFFFFF", out);
}

void test_formatRodId_output_length(void) {
  uint8_t mac[6] = {0, 0, 0, 0xA0, 0xB0, 0xC0};
  char out[ROD_ID_LEN];
  formatRodId(mac, out, sizeof(out));
  TEST_ASSERT_EQUAL_size_t(6, strlen(out));
}

void test_formatRodId_uses_uppercase(void) {
  uint8_t mac[6] = {0, 0, 0, 0xaa, 0xbb, 0xcc};
  char out[ROD_ID_LEN];
  formatRodId(mac, out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("AABBCC", out);
}

void test_formatRodId_truncates_when_buffer_too_small(void) {
  uint8_t mac[6] = {0, 0, 0, 0xAB, 0xCD, 0xEF};
  char out[4] = {'X', 'X', 'X', 'X'};
  formatRodId(mac, out, sizeof(out));
  // snprintf writes at most outSize-1 chars + '\0'
  TEST_ASSERT_EQUAL_STRING("ABC", out);
}

void test_formatRodId_null_buffer_is_safe(void) {
  uint8_t mac[6] = {0, 0, 0, 0x01, 0x02, 0x03};
  // Should not crash.
  formatRodId(mac, nullptr, 0);
  formatRodId(mac, nullptr, 10);
  TEST_PASS();
}

void test_formatRodId_zero_size_is_safe(void) {
  uint8_t mac[6] = {0, 0, 0, 0x01, 0x02, 0x03};
  char out[ROD_ID_LEN] = {'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z'};
  formatRodId(mac, out, 0);
  // Buffer untouched.
  TEST_ASSERT_EQUAL_CHAR('Z', out[0]);
}

// buildDeviceName -------------------------------------------------------------

void test_buildDeviceName_concatenates_prefix(void) {
  char out[ROD_DEVICE_NAME_LEN];
  buildDeviceName("ABCDEF", out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("Chabite-ABCDEF", out);
}

void test_buildDeviceName_empty_id(void) {
  char out[ROD_DEVICE_NAME_LEN];
  buildDeviceName("", out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("Chabite-", out);
}

void test_buildDeviceName_null_id_treated_as_empty(void) {
  char out[ROD_DEVICE_NAME_LEN];
  buildDeviceName(nullptr, out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("Chabite-", out);
}

void test_buildDeviceName_truncates_when_buffer_too_small(void) {
  char out[10] = {0};
  buildDeviceName("ABCDEF", out, sizeof(out));
  // "Chabite-ABCDEF" is 14 chars; only 9 chars fit + '\0'.
  TEST_ASSERT_EQUAL_STRING("Chabite-A", out);
}

void test_buildDeviceName_null_buffer_is_safe(void) {
  buildDeviceName("ABCDEF", nullptr, 0);
  buildDeviceName("ABCDEF", nullptr, 10);
  TEST_PASS();
}

void test_buildDeviceName_zero_size_is_safe(void) {
  char out[ROD_DEVICE_NAME_LEN] = {'Z', 'Z', 'Z'};
  buildDeviceName("ABCDEF", out, 0);
  TEST_ASSERT_EQUAL_CHAR('Z', out[0]);
}

// Integration: full pipeline (MAC -> id -> device name) -----------------------

void test_pipeline_mac_to_device_name(void) {
  uint8_t mac[6] = {0x24, 0x6F, 0x28, 0xDE, 0xAD, 0xBE};
  char id[ROD_ID_LEN];
  char name[ROD_DEVICE_NAME_LEN];
  formatRodId(mac, id, sizeof(id));
  buildDeviceName(id, name, sizeof(name));
  TEST_ASSERT_EQUAL_STRING("DEADBE", id);
  TEST_ASSERT_EQUAL_STRING("Chabite-DEADBE", name);
}

// Runner ----------------------------------------------------------------------

int main(int argc, char **argv) {
  UNITY_BEGIN();

  RUN_TEST(test_formatRodId_uses_last_three_bytes);
  RUN_TEST(test_formatRodId_ignores_first_three_bytes);
  RUN_TEST(test_formatRodId_pads_with_zero);
  RUN_TEST(test_formatRodId_all_zero);
  RUN_TEST(test_formatRodId_all_ff);
  RUN_TEST(test_formatRodId_output_length);
  RUN_TEST(test_formatRodId_uses_uppercase);
  RUN_TEST(test_formatRodId_truncates_when_buffer_too_small);
  RUN_TEST(test_formatRodId_null_buffer_is_safe);
  RUN_TEST(test_formatRodId_zero_size_is_safe);

  RUN_TEST(test_buildDeviceName_concatenates_prefix);
  RUN_TEST(test_buildDeviceName_empty_id);
  RUN_TEST(test_buildDeviceName_null_id_treated_as_empty);
  RUN_TEST(test_buildDeviceName_truncates_when_buffer_too_small);
  RUN_TEST(test_buildDeviceName_null_buffer_is_safe);
  RUN_TEST(test_buildDeviceName_zero_size_is_safe);

  RUN_TEST(test_pipeline_mac_to_device_name);

  return UNITY_END();
}
