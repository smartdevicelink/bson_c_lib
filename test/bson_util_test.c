#include <check.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "bson_util.h"

START_TEST(read_string)
{
  const char *test_string = "ABCDEFGHIJK4567890qwerty";

  size_t buf_size = strlen(test_string) + 1;
  uint8_t *buf = malloc(buf_size);
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_string, strlen(test_string));
  buf[buf_size - 1] = '\0';

  const uint8_t *p = buf;
  char *output = NULL;
  size_t size = buf_size;

  size_t ret = read_string_len(&output, &p, &size);
  ck_assert_int_eq(ret, buf_size);
  ck_assert_str_eq(output, test_string);
  ck_assert_ptr_eq(p, buf + buf_size);
  ck_assert_uint_eq(size, 0);

  free(output);
  free(buf);
}
END_TEST

START_TEST(read_string_twice)
{
  const char *test_string1 = "123456";
  const char *test_string2 = "abcdefghijklmno";

  size_t buf_size = strlen(test_string1) + 1 + strlen(test_string2) + 1;
  uint8_t *buf = malloc(buf_size);
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_string1, strlen(test_string1));
  buf[strlen(test_string1)] = '\0';

  memcpy(buf + strlen(test_string1) + 1, test_string2, strlen(test_string2));
  buf[strlen(test_string1) + 1 + strlen(test_string2)] = '\0';

  const uint8_t *p = buf;
  char *output = NULL;
  size_t size = buf_size;

  size_t ret = read_string_len(&output, &p, &size);
  ck_assert_int_eq(ret, (strlen(test_string1) + 1));
  ck_assert_str_eq(output, test_string1);
  ck_assert_ptr_eq(p, buf + (strlen(test_string1) + 1));
  ck_assert_uint_eq(size, buf_size - (strlen(test_string1) + 1));
  free(output);

  ret = read_string_len(&output, &p, &size);
  ck_assert_int_eq(ret, (strlen(test_string2) + 1));
  ck_assert_str_eq(output, test_string2);
  ck_assert_ptr_eq(p, buf + buf_size);
  ck_assert_uint_eq(size, 0);
  free(output);

  free(buf);
}
END_TEST

START_TEST(read_string_overrun)
{
  const char *test_string = "ABCDE";

  size_t buf_size = strlen(test_string); // no '\0' at the end
  uint8_t *buf = malloc(buf_size);
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_string, buf_size);

  const uint8_t *p = buf;
  char *output = NULL;
  size_t size = buf_size;

  size_t ret = read_string_len(&output, &p, &size);
  ck_assert_int_eq(ret, 0);
  ck_assert_ptr_eq(p, buf);
  ck_assert_uint_eq(size, buf_size);

  free(buf);
}
END_TEST


Suite *suite(void) {
  Suite *s = suite_create("bson_util_test");

  TCase *tc = tcase_create("read_bytes");
  tcase_add_test(tc, read_string);
  tcase_add_test(tc, read_string_twice);
  tcase_add_test(tc, read_string_overrun);

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
