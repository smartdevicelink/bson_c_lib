#include <stdlib.h>
#include <string.h>
 
#include <lua5.2/lua.h>
#include <lua5.2/lauxlib.h>
 
#include <bson_object.h>

static int table_to_bson_object(lua_State *L, BsonObject *obj, char *errorMessage);
static int table_to_bson_array(lua_State *L, BsonArray *arr, char *errorMessage);
static int bson_put_map_value(lua_State *L, void *bson, const char *key, char *errorMessage);

static int bson_object_to_table(lua_State *L, BsonObject *obj, char *errorMessage);
static int bson_array_to_table(lua_State *L, BsonArray *arr, char *errorMessage);
static int bson_element_to_table(lua_State *L, const BsonElement *element, char *errorMessage);

/*
  @brief Convert a Lua table into a BSON-formatted byte string.
    Lua arguments:
      1 (table) - Explicitly typed Lua table to be converted to BSON format.
        Entries in this map should be in the following format:
          map[key] = {
            type = element_type,
            value = val
          }
    Lua return values:
      1 (string) - Byte string containing the BSON representation of the map

  @param L - Lua state, used to modify the Lua stack

  @return The number of return values pushed to the stack
*/
static int map_to_bytes(lua_State *L) {
  if (lua_istable(L, 1) == 0) {
    return luaL_argerror(L, 1, "Arg 1 must be a table");
  }

  BsonObject bson;
  char errorMessage[255];

  const int result = table_to_bson_object(L, &bson, errorMessage);  
  if (result != 0) {
    return luaL_argerror(L, 1, errorMessage);
  }

  const size_t obj_size = bson_object_size(&bson);
  uint8_t *bytes = bson_object_to_bytes(&bson);

  bson_object_deinitialize(&bson);
  if (bytes == NULL) {
    return luaL_argerror(L, 1, "Map could not be parsed into bson");
  }

  //Create string copy of the BSON bytes to pass to Lua
  char *byte_string = malloc(obj_size * sizeof(char));
  size_t index = 0;
  for (index = 0; index < obj_size; index++) {
    byte_string[index] = bytes[index] & 0xFF;
  }
  free(bytes);

  lua_pushlstring(L, (const char *)byte_string, obj_size);
  free(byte_string);

  return 1;
}

/*
  @brief Convert a Lua table into a BSON object.
    Lua arguments:
      1 (table) - Explicitly typed Lua table to be converted to BSON format.
        Entries in this map should be in the following format:
          map[key] = {
            type = element_type,
            value = val
          }

  @param L - Lua state, used to modify the Lua stack
  @param obj - The object where the converted table is stored
  @param errorMessage - To be populated with an error message if an error occurs

  @return 0 on success, non-zero on failure
*/
static int table_to_bson_object(lua_State *L, BsonObject *obj, char *errorMessage) {
  //Verifies that the object at the top of the stack is a table
  if (lua_istable(L, -1) == 0) {
    snprintf(errorMessage, 255, "Expected table, but value was of type: %s", 
             lua_typename(L, lua_type(L, -1)));
    return -1;
  }
  if (!bson_object_initialize_default(obj)) {
    snprintf(errorMessage, 255, "Failed to initialize BSON object");
  }

  lua_pushnil(L); //Stack: [bson_table, nil]
  while (lua_next(L, -2) != 0) { //Stack: [bson_table, key, {type, value}]
    //Verifies that the entry is in a explicitly typed format, since explicit typing is required in BSON
    if (lua_istable(L, -1) == 0) {
      snprintf(errorMessage, 255, "Table is not in BSON format (key: {type, value})");
      bson_object_deinitialize(obj);
      return -1;
    }

    //Retrieve the key from the stack
    const char *key = luaL_checkstring(L, -2);

    const int result = bson_put_map_value(L, obj, key, errorMessage); //Stack: [bson_table, key]
    if (result != 0) {
      bson_object_deinitialize(obj);
      return -1;
    }
    lua_pop(L, 1);
  }
  //Stack: [bson_table]
  return 0;
}

/*
  @brief Convert a Lua table into a BSON array.
    Lua arguments:
      1 (table) - Explicitly typed Lua table to be converted to BSON format.
        Entries in this map should be in the following format:
          map[index] = {
            type = element_type,
            value = val
          }

  @param L - Lua state, used to modify the Lua stack
  @param arr - The array where the converted table is stored
  @param errorMessage - To be populated with an error message if an error occurs

  @return 0 on success, non-zero on failure
*/
static int table_to_bson_array(lua_State *L, BsonArray *arr, char *errorMessage) {
  //Verifies that the object at the top of the stack is a table
  if (lua_istable(L, -1) == 0) {
    snprintf(errorMessage, 255, "Expected array, but value was of type: %s", 
         lua_typename(L, lua_type(L, -1)));
    return -1;
  }

  //Stack: [bson_array]
  const int len = luaL_len(L, -1);
  if (!bson_array_initialize(arr, len)) {
    snprintf(errorMessage, 255, "Failed to initialize BSON array");
  }

  int index;
  for (index = 1; index <= len; index++) {
    lua_rawgeti(L, -1, index); //Stack: [bson_array, {type, value}]
    //Verifies that the entry is in a explicitly typed format, since explicit typing is required in BSON
    if (lua_istable(L, -1) == 0) {
      snprintf(errorMessage, 255, "Table is not in BSON format (key: {type, value})");
      bson_array_deinitialize(arr);
      return -1;
    }

    const int result = bson_put_map_value(L, arr, NULL, errorMessage); //Stack: [bson_array]
    if (result != 0) {
      bson_array_deinitialize(arr);
      return -1;
    }
    lua_pop(L, 1);
  }
  return 0;
}

/*
  @brief Convert a Lua table into a BSON object.
    Lua arguments:
      1 (table) - Explicitly typed Lua table entry to be converted to BSON format.
        This table should be in the following format:
          {
            type = element_type,
            value = val
          }

  @param L - Lua state, used to modify the Lua stack
  @param bson - The BSON object/array where the converted table is stored
  @param errorMessage - To be populated with an error message if an error occurs

  @return 0 on success, non-zero on failure
*/
static int bson_put_map_value(lua_State *L, void *bson, const char *key, char *errorMessage) {
  //Retrieve the type of the entry and place it on the stack
  lua_getfield(L, -1, "type"); //Stack: [{type, value}, type]

  //Store the type of the entry and remove it from the stack
  const lua_Integer type = luaL_checkinteger(L, -1);
  lua_pop(L, 1); //Stack: [{type, value}]

  //Retrieve the value of the entry and place it on the stack
  lua_getfield(L, -1, "value"); //Stack: [{type, value}, value]
  switch ((const element_type)type) {
    case TYPE_DOCUMENT: {
      BsonObject value;
      const int result = table_to_bson_object(L, &value, errorMessage);
      if (result != 0) {
        return result;
      }

      //Assume array if no key is provided
      if (key == NULL) {
        bson_array_add_object((BsonArray *)bson, &value);
      }
      else {
        bson_object_put_object((BsonObject *)bson, key, &value);
      }
      break;
    }
    case TYPE_ARRAY: {
      BsonArray value;
      const int result = table_to_bson_array(L, &value, errorMessage);
      if (result != 0) {
        return result;
      }

      //Assume array if no key is provided
      if (key == NULL) {
        bson_array_add_array((BsonArray *)bson, &value);
      }
      else {
        bson_object_put_array((BsonObject *)bson, key, &value);
      }
      break;
    }
    case TYPE_INT32: {
      lua_Integer value = luaL_checkinteger(L, -1);

      //Assume array if no key is provided
      if (key == NULL) {
        bson_array_add_int32((BsonArray *)bson, (int32_t)value);
      }
      else {
        bson_object_put_int32((BsonObject *)bson, key, (int32_t)value);
      }
      break;
    }
    case TYPE_INT64: {
      lua_Integer value = luaL_checkinteger(L, -1);

      //Assume array if no key is provided
      if (key == NULL) {
        bson_array_add_int64((BsonArray *)bson, (int64_t)value);
      }
      else {
        bson_object_put_int64((BsonObject *)bson, key, (int64_t)value);
      }
      break;
    }
    case TYPE_STRING: {
      char *value = strdup(luaL_checkstring(L, -1));

      //Assume array if no key is provided
      if (key == NULL) {
        bson_array_add_string((BsonArray *)bson, value);
      }
      else {
        bson_object_put_string((BsonObject *)bson, key, value);
      }
      free(value);
      break;
    }
    case TYPE_DOUBLE: {
      lua_Number value = luaL_checknumber(L, -1);

      //Assume array if no key is provided
      if (key == NULL) {
        bson_array_add_double((BsonArray *)bson, (double)value);
      }
      else {
        bson_object_put_double((BsonObject *)bson, key, (double)value);
      }
      break;
    }
    case TYPE_BOOLEAN: {
      if (lua_isboolean(L, -1) == 0) {
        snprintf(errorMessage, 255, "Expected boolean, but value was of type: %s", 
             lua_typename(L, lua_type(L, -1)));
        return -1;
      }
      int value = lua_toboolean(L, -1);

      //Assume array if no key is provided
      if (key == NULL) {
        bson_array_add_bool((BsonArray *)bson, (bson_boolean)value);
      }
      else {
        bson_object_put_bool((BsonObject *)bson, key, (bson_boolean)value);
      }
      break;
    }
    default: {
      snprintf(errorMessage, 255, "Error parsing table, unrecognized type: %i", (int)type);
      return -1;
    }
  }
  lua_pop(L, 1); //Stack: [{type, value}]
  return 0;
}

/*
  @brief Convert a BSON-formatted byte string into a Lua table.
    Lua arguments:
      1 (string) - BSON-formatted byte string to be converted
    Lua return values:
      1 (table) - Lua table parsed from the given BSON string.
        Entries in this map are provided in the following format:
          map[key] = {
            type = element_type,
            value = val
          }

  @param L - Lua state, used to modify the Lua stack

  @return The number of return values pushed to the stack
*/
static int map_from_bytes(lua_State *L) {
  if (lua_isstring(L, 1) == 0) {
    return luaL_argerror(L, 1, "Arg 1 must be a Lua String");
  }
  size_t len;
  const char *byte_string = lua_tolstring(L, 1, &len);

  uint8_t *bytes = malloc(len);
  
  //Convert lua string to byte array to pass to the BSON library
  size_t index = 0;
  for (index = 0; index < len; index++) {
    bytes[index] = byte_string[index] & 0xFF;
  }

  BsonObject obj;
  size_t bytes_read = bson_object_from_bytes_len(&obj, bytes, len);
  free(bytes);

  char errorMessage[255];
  if (bytes_read != len) {
    snprintf(errorMessage, 255, "Provided string was not a valid BSON object, expected length: %zi, got: %zi", len, bytes_read);
    return luaL_argerror(L, 1, errorMessage);
  }

  const int result = bson_object_to_table(L, &obj, errorMessage);
  bson_object_deinitialize(&obj);

  if (result != 0) {
    return luaL_argerror(L, 1, errorMessage);
  }
  return 1;
}

/*
  @brief Convert a BSON object into a Lua table.
    Lua return values:
      1 (table) - Lua table generated from the given BSON object.
        Entries in this map are provided in the following format:
          key = {
            type = element_type,
            value = val
          }

  @param L - Lua state, used to modify the Lua stack
  @param obj - The BSON object to be converted
  @param errorMessage - Populated in the case of an error

  @return 0 on success, non-zero on failure
*/
static int bson_object_to_table(lua_State *L, BsonObject *obj, char *errorMessage) {
  MapIterator iter = bson_object_iterator(obj);
  BsonObjectEntry entry = bson_object_iterator_next(&iter);
  lua_newtable(L); //Stack: [table]
  while (entry.element != NULL) {
    const char *key = entry.key;
    const BsonElement *element = entry.element;

    const int result = bson_element_to_table(L, element, errorMessage); //Stack: [table, {type, value}]
    if (result != 0) {
      return result;
    }

    lua_setfield(L, -2, key); //Stack: [table]
    entry = bson_object_iterator_next(&iter);
  }
  return 0;
}

/*
  @brief Convert a BSON array into a Lua table.
    Lua return values:
      1 (table) - Lua table generated from the given BSON array.
        Entries in this map are provided in the following format:
          map[index] = {
            type = element_type,
            value = val
          }

  @param L - Lua state, used to modify the Lua stack
  @param arr - The BSON array to be converted
  @param errorMessage - Populated in the case of an error

  @return 0 on success, non-zero on failure
*/
static int bson_array_to_table(lua_State *L, BsonArray *arr, char *errorMessage) {
  lua_newtable(L); //Stack: [table]
  int index;
  for (index = 0; index < arr->count; index++) {
    const BsonElement *element = bson_array_get(arr, index);
    
    const int result = bson_element_to_table(L, element, errorMessage); //Stack: [table, {type, value}]
    if (result != 0) {
      return result;
    }

    lua_rawseti(L, -2, index + 1 /*Lua indexing*/); //Stack: [table]
  }
  return 0;
}

/*
  @brief Convert a BSON element into a explicitly typed lua table entry.
    Lua return values:
      1 (table) - Lua table generated from the given BSON element.
        Provided in the following format:
          {
            type = element_type,
            value = val
          }

  @param L - Lua state, used to modify the Lua stack
  @param element - The BSON element to be converted
  @param errorMessage - Populated in the case of an error

  @return 0 on success, non-zero on failure
*/
static int bson_element_to_table(lua_State *L, const BsonElement *element, char *errorMessage) {
  lua_newtable(L); //Stack: [{}];

  lua_pushinteger(L, ((lua_Integer)element->type) & 0xFF); //Stack: [{}, type]
  lua_setfield(L, -2, "type"); //Stack: [{type}]

  switch (element->type) {
    case TYPE_DOCUMENT: {
      BsonObject *value = (BsonObject *)element->value;
      const int result = bson_object_to_table(L, value, errorMessage); //Stack: [{type}, subTable]
      if (result != 0) {
        return result;
      }
      break;
    }
    case TYPE_ARRAY: {
      BsonArray *value = (BsonArray *)element->value;
      const int result = bson_array_to_table(L, value, errorMessage); //Stack: [{type}, subTable]
      if (result != 0) {
        return result;
      }
      break;
    }
    case TYPE_INT32: {
      int32_t value = *(int32_t *)element->value;
      lua_pushinteger(L, (lua_Integer)value); //Stack: [{type}, value]
      break;
    }
    case TYPE_INT64: {
      int64_t value = *(int64_t *)element->value;
      lua_pushinteger(L, (lua_Integer)value); //Stack: [{type}, value]
      break;
    }
    case TYPE_STRING: {
      char *value = (char *)element->value;
      lua_pushstring(L, value); //Stack: [{type}, value]
      break;
    }
    case TYPE_DOUBLE: {
      double value = *(double *)element->value;
      lua_pushnumber(L, (lua_Number)value); //Stack: [{type}, value]
      break;
    }
    case TYPE_BOOLEAN: {
      bson_boolean value = *(bson_boolean *)element->value; //Stack: [{type}, value]
      lua_pushboolean(L, (int)value);
      break;
    }
    default: {
      snprintf(errorMessage, 255, "Unsupported  type in BSON object: %i", (int)element->type);
      return -1;
    }
  }
  lua_setfield(L, -2, "value"); //Stack: [{type, value}]
  return 0;
}

//Methods available to Lua
static const struct luaL_Reg bson_object_methods[] = {
  { "to_bytes", map_to_bytes },
  { "to_table", map_from_bytes },
  { NULL, NULL }
};

int luaopen_bson4lua(lua_State *L) {
  luaL_newlib(L, bson_object_methods);
  return 1;
}
