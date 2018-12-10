#include <check.h>
#include <stdint.h>
#include <stdlib.h>
#include "bson_object.h"

#define BSON_TAG_DOUBLE   (0x01)
#define BSON_TAG_STRING   (0x02)
#define BSON_TAG_DOCUMENT (0x03)
#define BSON_TAG_ARRAY    (0x04)
#define BSON_TAG_BOOLEAN  (0x08)
#define BSON_TAG_INT32    (0x10)
#define BSON_TAG_INT64    (0x12)

START_TEST(bson_object_from_bytes_corrupted_key)
{
  /*
     {
       "string_te (data corrupted during the key)
   */
  uint8_t test_data_corrupted[] = {
      0x0E, 0x0, 0x0, 0x0, // overall length
      BSON_TAG_STRING, 's', 't', 'r', 'i', 'n', 'g', '_', 't', 'e' /* buffer ends in the middle */
  };

  uint8_t *buf = malloc(sizeof(test_data_corrupted));
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_data_corrupted, sizeof(test_data_corrupted));

  BsonObject ret = bson_object_from_bytes(buf);
  free(buf);

  bson_object_deinitialize(&ret);
}
END_TEST

START_TEST(bson_object_from_bytes_corrupted_tag_length)
{
  /*
     {
       "string_test": (data corrupted before starting the string)
   */
  uint8_t test_data_corrupted[] = {
      0x13, 0x0, 0x0, 0x0, // overall length
      BSON_TAG_STRING, 's', 't', 'r', 'i', 'n', 'g', '_', 't', 'e', 's', 't', 0x0,
        0x0, 0x10, /* buffer ends during the 4-byte length field */
  };

  uint8_t *buf = malloc(sizeof(test_data_corrupted));
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_data_corrupted, sizeof(test_data_corrupted));

  BsonObject ret = bson_object_from_bytes(buf);
  free(buf);

  bson_object_deinitialize(&ret);
}
END_TEST

START_TEST(bson_object_from_bytes_corrupted_string)
{
  /*
     {
       "string_test": "ABCDEF (very long string, but corrupted in the middle)
   */
  uint8_t test_data_corrupted[] = {
      0x1C, 0x0, 0x0, 0x0, // overall length
      BSON_TAG_STRING, 's', 't', 'r', 'i', 'n', 'g', '_', 't', 'e', 's', 't', 0x0,
        0x0, 0x10, 0x0, 0x0, 'A', 'B', 'C', 'D', 'E', 'F', 'G' /* buffer ends in the middle */
  };

  uint8_t *buf = malloc(sizeof(test_data_corrupted));
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_data_corrupted, sizeof(test_data_corrupted));

  BsonObject ret = bson_object_from_bytes(buf);
  free(buf);

  bson_object_deinitialize(&ret);
}
END_TEST

START_TEST(bson_object_from_bytes_corrupted_integer)
{
  /*
     {
       "value": 123 (data size is not sufficient)
   */
  uint8_t test_data_corrupted[] = {
      0x0E, 0x0, 0x0, 0x0, // overall length
      BSON_TAG_INT32, 'v', 'a', 'l', 'u', 'e', 0x0,
        0x7B, 0x0, 0x0 /* missing the last byte */
  };

  uint8_t *buf = malloc(sizeof(test_data_corrupted));
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_data_corrupted, sizeof(test_data_corrupted));

  BsonObject ret = bson_object_from_bytes(buf);
  free(buf);

  bson_object_deinitialize(&ret);
}
END_TEST

START_TEST(bson_object_from_bytes_corrupted_tag)
{
  /*
     {
       "value": 123,
        (next tag is missing)
   */
  uint8_t test_data_corrupted[] = {
      0x0F, 0x0, 0x0, 0x0, // overall length
      BSON_TAG_INT32, 'v', 'a', 'l', 'u', 'e', 0x0,
        0x7B, 0x0, 0x0, 0x0
      /* missing tag */
  };

  uint8_t *buf = malloc(sizeof(test_data_corrupted));
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_data_corrupted, sizeof(test_data_corrupted));

  BsonObject ret = bson_object_from_bytes(buf);
  free(buf);

  bson_object_deinitialize(&ret);
}
END_TEST


Suite *suite(void) {
  Suite *s = suite_create("bson_object_test");

  TCase *tc = tcase_create("read_from_bytes");
  tcase_add_test(tc, bson_object_from_bytes_corrupted_key);
  tcase_add_test(tc, bson_object_from_bytes_corrupted_tag_length);
  tcase_add_test(tc, bson_object_from_bytes_corrupted_string);
  tcase_add_test(tc, bson_object_from_bytes_corrupted_integer);
  tcase_add_test(tc, bson_object_from_bytes_corrupted_tag);

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
