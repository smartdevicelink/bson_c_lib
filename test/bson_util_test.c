#include <check.h>
#include <stdint.h>
#include <stdlib.h>
#include "bson_util.h"

START_TEST(read_int64_le_zero)
{
  uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t *p = buf;
  int64_t ret = read_int64_le(&p);
  ck_assert_ptr_eq(p, buf + 8);
  ck_assert_int_eq(ret, 0);
}
END_TEST

START_TEST(read_int64_le_positive)
{
  uint8_t buf[8] = {0x12, 0xA2, 0xBF, 0x82, 0x34, 0xD0, 0x07, 0x73};
  uint8_t *p = buf;
  int64_t ret = read_int64_le(&p);
  ck_assert_ptr_eq(p, buf + 8);
  ck_assert_int_eq(ret, 0x7307D03482BFA212);
}
END_TEST

START_TEST(read_int64_le_negative)
{
  uint8_t buf[8] = {0x34, 0xC9, 0x30, 0x74, 0x22, 0x6F, 0xCA, 0xBD};
  int64_t expected = -(int64_t)((~((uint64_t)0xBDCA6F227430C934) + 1)); // = -4770878661476693708
  uint8_t *p = buf;
  int64_t ret = read_int64_le(&p);
  ck_assert_ptr_eq(p, buf + 8);
  ck_assert_int_eq(ret, expected);
}
END_TEST

START_TEST(read_int64_le_positive_max)
{
  uint8_t buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F};
  uint8_t *p = buf;
  int64_t ret = read_int64_le(&p);
  ck_assert_ptr_eq(p, buf + 8);
  ck_assert_int_eq(ret, INT64_MAX);
}
END_TEST

START_TEST(read_int64_le_negative_min)
{
  uint8_t buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  uint8_t *p = buf;
  int64_t ret = read_int64_le(&p);
  ck_assert_ptr_eq(p, buf + 8);
  ck_assert_int_eq(ret, -1);
}
END_TEST

START_TEST(read_int64_le_negative_max)
{
  uint8_t buf[8] = {0, 0, 0, 0, 0, 0, 0, 0x80};
  uint8_t *p = buf;
  int64_t ret = read_int64_le(&p);
  ck_assert_ptr_eq(p, buf + 8);
  ck_assert_int_eq(ret, INT64_MIN);
}
END_TEST


Suite *suite(void) {
  Suite *s = suite_create("bson_util_test");

  TCase *tc = tcase_create("read_bytes");
  tcase_add_test(tc, read_int64_le_zero);
  tcase_add_test(tc, read_int64_le_positive);
  tcase_add_test(tc, read_int64_le_negative);
  tcase_add_test(tc, read_int64_le_positive_max);
  tcase_add_test(tc, read_int64_le_negative_min);
  tcase_add_test(tc, read_int64_le_negative_max);

  suite_add_tcase(s, tc);
  return s;
}

int main(void) {
  int failed_num = 0;
  Suite *s = suite();
  SRunner *sr = srunner_create(s);
  srunner_set_fork_status(sr, CK_NOFORK);

  srunner_run_all(sr, CK_NORMAL);

  failed_num = srunner_ntests_failed(sr);
  srunner_free(sr);

  return failed_num == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
