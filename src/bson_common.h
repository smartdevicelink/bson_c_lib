#ifndef BSON_FWD_H
#define BSON_FWD_H

#include "emhashmap/emhashmap.h"

//Byte which defines the type of a value as defined in the BSON spec
enum element_type {
  TYPE_DOUBLE = 0x01,
  TYPE_STRING = 0x02,
  TYPE_DOCUMENT = 0x03,
  TYPE_ARRAY = 0x04,
  TYPE_BINARY = 0x05, //unused
  TYPE_UNDEFINED = 0x06, //deprecated
  TYPE_OBJECT_ID = 0x07, //unused
  TYPE_BOOLEAN = 0x08,
  TYPE_DATE_TIME = 0x09, //unused
  TYPE_NULL = 0x0A, //unused
  TYPE_REGEX = 0x0B, //unused
  TYPE_DB_POINTER = 0x0C, //deprecated
  TYPE_JS_CODE = 0x0D, //unused
  TYPE_SYMBOL = 0x0E, //deprecated
  TYPE_JS_CODE_WITH_SCOPE = 0x0F, //unused
  TYPE_INT32 = 0x10,
  TYPE_TIMESTAMP = 0x11, //unused
  TYPE_INT64 = 0x12,
  TYPE_DEC128 = 0x13, //unused
  TYPE_MIN_KEY = 0xFF, //unused
  TYPE_MAX_KEY = 0x7F //unused
};
typedef enum element_type element_type;

//Definition of each boolean value according to the BSON spec
enum bson_boolean {
  BOOLEAN_INVALID = -1,
  BOOLEAN_FALSE = 0x00,
  BOOLEAN_TRUE = 0x01
};
typedef enum bson_boolean bson_boolean;


struct BsonObject {
  //Internal map implementation
  HashMap data;
};
typedef struct BsonObject BsonObject;

struct BsonElement {
  //The value of this element
  void *value;
  //The data type of this element
  element_type type;
  //Size of the element in bytes when converted to BSON 
  //Unused for TYPE_DOCUMENT and TYPE_ARRAY
  size_t size;
};
typedef struct BsonElement BsonElement;

struct BsonObjectEntry {
  char key[255];
  BsonElement *element;
};
typedef struct BsonObjectEntry BsonObjectEntry;

//Object representing a BSON array
struct BsonArray {
  //Array of BSON elements
  BsonElement **elements;
  //Number of elements currently in the array
  size_t count;
  //The current maximum number of elements in the array
  size_t maxCount;
};
typedef struct BsonArray BsonArray;

#endif  // BSON_FWD_H