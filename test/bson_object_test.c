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

START_TEST(bson_object_from_bytes_all_types)
{
  /*
     {
       "testint": 12345,
       "testobj": {
         "string_test": "ABCDEFGabcdefg",
         "arr": [100, 200, 300]
       },
       "float": -123.456,
       "boolValue": true
     }
   */
  uint8_t test_data[] = {
      0x86, 0x0, 0x0, 0x0, // overall length
      BSON_TAG_INT32, 't', 'e', 's', 't', 'i', 'n', 't', 0x0,
        0x39, 0x30, 0x0, 0x0,
      BSON_TAG_DOCUMENT, 't', 'e', 's', 't', 'o', 'b', 'j', 0x0,
        0x50, 0x0, 0x0, 0x0, // length of this embedded document
        BSON_TAG_STRING, 's', 't', 'r', 'i', 'n', 'g', '_', 't', 'e', 's', 't', 0x0,
          0x0F, 0x0, 0x0, 0x0, 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 0x0,
        BSON_TAG_ARRAY, 'a', 'r', 'r', 0x0,
          0x26, 0x0, 0x0, 0x0, // length of this embedded array
          BSON_TAG_INT64, '0', 0x0,
            0x64, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
          BSON_TAG_INT64, '1', 0x0,
            0xC8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
          BSON_TAG_INT64, '2', 0x0,
            0x2C, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
          0x0, // end of array
        0x0, // end of document
      BSON_TAG_DOUBLE, 'f', 'l', 'o', 'a', 't', 0x0,
      0x77, 0xBE, 0x9F, 0x1A, 0x2F, 0xDD, 0x5E, 0xC0,
      BSON_TAG_BOOLEAN, 'b', 'o', 'o', 'l', 'V', 'a', 'l', 'u', 'e', 0x0,
        0x01, // true
      0x0 // end of document
  };

  uint8_t *buf = malloc(sizeof(test_data));
  if (buf == NULL) {
    ck_abort_msg("malloc failed");
  }
  memcpy(buf, test_data, sizeof(test_data));

  BsonObject output;
  size_t ret = bson_object_from_bytes_len(&output, buf, sizeof(test_data));
  free(buf);

  ck_assert_uint_eq(ret, sizeof(test_data));

  // check elements
  BsonElement *e = bson_object_get(&output, "testint");
  ck_assert(e);
  ck_assert_int_eq(e->type, TYPE_INT32);

  e = bson_object_get(&output, "testobj");
  ck_assert(e);
  ck_assert_int_eq(e->type, TYPE_DOCUMENT);

  e = bson_object_get(&output, "float");
  ck_assert(e);
  ck_assert_int_eq(e->type, TYPE_DOUBLE);

  e = bson_object_get(&output, "boolValue");
  ck_assert(e);
  ck_assert_int_eq(e->type, TYPE_BOOLEAN);

  // check non-existing key
  e = bson_object_get(&output, "do-not-exist");
  ck_assert(e == NULL);

  // check values
  int32_t testint = bson_object_get_int32(&output, "testint");
  ck_assert_int_eq(testint, 12345);

  BsonObject *sub = bson_object_get_object(&output, "testobj");
  ck_assert(sub);

  double float_value = bson_object_get_double(&output, "float");
  ck_assert(float_value == -123.456);

  bson_boolean bool_value = bson_object_get_bool(&output, "boolValue");
  ck_assert_int_eq(bool_value, BOOLEAN_TRUE);

  // check inside sub object
  e = bson_object_get(sub, "string_test");
  ck_assert(e);
  ck_assert_int_eq(e->type, TYPE_STRING);

  e = bson_object_get(sub, "arr");
  ck_assert(e);
  ck_assert_int_eq(e->type, TYPE_ARRAY);

  e = bson_object_get(sub, "non-existent");
  ck_assert(e == NULL);

  char *string_test = bson_object_get_string(sub, "string_test");
  ck_assert(string_test);
  ck_assert_str_eq(string_test, "ABCDEFGabcdefg");

  BsonArray *arr = bson_object_get_array(sub, "arr");
  ck_assert(arr);
  ck_assert_uint_eq(arr->count, 3);

  int64_t arr_value = bson_array_get_int64(arr, 0);
  ck_assert_int_eq(arr_value, 100);

  arr_value = bson_array_get_int64(arr, 1);
  ck_assert_int_eq(arr_value, 200);

  arr_value = bson_array_get_int64(arr, 2);
  ck_assert_int_eq(arr_value, 300);

  bson_object_deinitialize(&output);
}
END_TEST

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

  // Test normal method
  BsonObject output;
  size_t ret = bson_object_from_bytes_len(&output, buf, sizeof(test_data_corrupted));

  // Test deprecated method
  BsonObject deprecated_output = bson_object_from_bytes(buf);
  free(buf);

  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(bson_object_size(&deprecated_output), 5);
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

  // Test normal method
  BsonObject output;
  size_t ret = bson_object_from_bytes_len(&output, buf, sizeof(test_data_corrupted));

  // Test deprecated method
  BsonObject deprecated_output = bson_object_from_bytes(buf);
  free(buf);

  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(bson_object_size(&deprecated_output), 5);
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

  // Test normal method
  BsonObject output;
  size_t ret = bson_object_from_bytes_len(&output, buf, sizeof(test_data_corrupted));

  // Test deprecated method
  BsonObject deprecated_output = bson_object_from_bytes(buf);
  free(buf);

  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(bson_object_size(&deprecated_output), 5);
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

  // Test normal method
  BsonObject output;
  size_t ret = bson_object_from_bytes_len(&output, buf, sizeof(test_data_corrupted));

  // Test deprecated method
  BsonObject deprecated_output = bson_object_from_bytes(buf);
  free(buf);

  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(bson_object_size(&deprecated_output), 5);
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

  // Test normal method
  BsonObject output;
  size_t ret = bson_object_from_bytes_len(&output, buf, sizeof(test_data_corrupted));

  // Test deprecated method
  BsonObject deprecated_output = bson_object_from_bytes(buf);
  free(buf);

  ck_assert_uint_eq(ret, 0);
  ck_assert_uint_eq(bson_object_size(&deprecated_output), 5);
}
END_TEST


Suite *suite(void) {
  Suite *s = suite_create("bson_object_test");

  TCase *tc = tcase_create("read_from_bytes");
  tcase_add_test(tc, bson_object_from_bytes_all_types);
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
