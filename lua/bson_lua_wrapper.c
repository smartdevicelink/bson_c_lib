#include <stdlib.h>
#include <string.h>
 
#include <lua5.2/lua.h>
#include <lua5.2/lauxlib.h>
 
#include <bson_object.h>

static int table_to_bson_object(lua_State *L, BsonObject *obj, char *errorMessage);
static int table_to_bson_array(lua_State *L, BsonArray *arr, char *errorMessage);

static int bson_object_to_table(lua_State *L, BsonObject *obj, char *errorMessage);
static int bson_element_to_table(lua_State *L, BsonElement *element, char *errorMessage);
static int bson_array_to_table(lua_State *L, BsonArray *arr, char *errorMessage);

static int lbson_object_to_bytes(lua_State *L) {
    BsonObject bson;
    bson_object_initialize_default(&bson);
    if (!lua_istable(L, 1)) {
        return luaL_argerror(L, 1, "Arg 1 must be a table");
    }

    char errorMessage[255];
    int success = table_to_bson_object(L, &bson, errorMessage);
    if (success != 0) {
        bson_object_deinitialize(&bson);
        return luaL_argerror(L, 1, errorMessage);
    }

    uint8_t *bytes = bson_object_to_bytes(&bson);
    if (bytes == NULL) {
        bson_object_deinitialize(&bson);
        return luaL_argerror(L, 1, "Map could not be parsed into bson");
    }

    size_t obj_size = bson_object_size(&bson);
    bson_object_deinitialize(&bson);

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

static int table_to_bson_object(lua_State *L, BsonObject *obj, char *errorMessage) {
    lua_pushnil(L); // Stack: [bson_table, nil]
    while (lua_next(L, -2) != 0) { //Stack: [bson_table, key, {type, value}]
        if (!lua_istable(L, -1)) {
            snprintf(errorMessage, 255, "Table is not in BSON format (key: {type, value})");
            return -1;
        }
        const char *key = luaL_checkstring(L, -2);

        lua_getfield(L, -1, "type"); //Stack: [bson_table, key, {type, value}, type]

        lua_Integer type = luaL_checkinteger(L, -1);
        lua_pop(L, 1); //Stack: [bson_table, key, {type, value}]

        lua_getfield(L, -1, "value"); //Stack: [bson_table, key, {type, value}, value]
        switch ((element_type)type) {
            case TYPE_DOCUMENT: {
                if (!lua_istable(L, -1)) {
                    snprintf(errorMessage, 255, "Expected table, but value was of type: %s", 
                             lua_typename(L, lua_type(L, -1)));
                    return -1;
                }
                BsonObject value;
                bson_object_initialize_default(&value);

                int success = table_to_bson_object(L, &value, errorMessage);
                if (success != 0) {
                    return success;
                }

                bson_object_put_object(obj, key, &value);
                break;
            }
            case TYPE_ARRAY: {
                if (!lua_istable(L, -1)) {
                    snprintf(errorMessage, 255, "Expected array, but value was of type: %s", 
                             lua_typename(L, lua_type(L, -1)));
                    return -1;
                }
                BsonArray value;
                bson_array_initialize(&value, 4);

                int success = table_to_bson_array(L, &value, errorMessage);
                if (success != 0) {
                    return success;
                }

                bson_object_put_array(obj, key, &value);
                break;
            }
            case TYPE_INT32: {
                lua_Integer value = luaL_checkinteger(L, -1);
                bson_object_put_int32(obj, key, (int32_t)value);
                break;
            }
            case TYPE_INT64: {
                lua_Integer value = luaL_checkinteger(L, -1);
                bson_object_put_int64(obj, key, (int64_t)value);
                break;
            }
            case TYPE_STRING: {
                char *value = strdup(luaL_checkstring(L, -1));
                bson_object_put_string(obj, key, value);
                free(value);
                break;
            }
            case TYPE_DOUBLE: {
                lua_Number value = luaL_checknumber(L, -1);
                bson_object_put_double(obj, key, (double)value);
                break;
            }
            case TYPE_BOOLEAN: {
                if (!lua_isboolean(L, -1)) {
                    snprintf(errorMessage, 255, "Expected boolean, but value was of type: %s", 
                             lua_typename(L, lua_type(L, -1)));
                    return -1;
                }
                int value = lua_toboolean(L, -1);
                bson_object_put_bool(obj, key, (bson_boolean)value);
                break;
            }
            default: {
                snprintf(errorMessage, 255, "Error parsing table, unrecognized type: %i", (int)type);
                return -1;
            }
        }
        lua_pop(L, 2); //Stack: [bson_table, key]
    }
    lua_pop(L, 2); //Stack: []
    return 0;
}

static int table_to_bson_array(lua_State *L, BsonArray *arr, char *errorMessage) {
    //Stack: [bson_array]
    int len = luaL_len(L, -1);
    int index;
    for (index = 1; index <= len; index++) {
        lua_rawgeti(L, -1, index); //Stack: [bson_array, {type, value}]
        if (!lua_istable(L, -1)) {
            snprintf(errorMessage, 255, "Table is not in BSON format (key: {type, value})");
            return -1;
        }

        lua_getfield(L, -1, "type"); //Stack: [bson_array, {type, value}, type]

        lua_Integer type = luaL_checkinteger(L, -1);
        lua_pop(L, 1); //Stack: [bson_table, {type, value}]

        lua_getfield(L, -1, "value"); //Stack: [bson_array, {type, value}, value]
        switch ((element_type)type) {
            case TYPE_DOCUMENT: {
                if (!lua_istable(L, -1)) {
                    snprintf(errorMessage, 255, "Expected table, but value was of type: %s", 
                             lua_typename(L, lua_type(L, -1)));
                    return -1;
                }
                BsonObject value;
                bson_object_initialize_default(&value);

                int success = table_to_bson_object(L, &value, errorMessage);
                if (success != 0) {
                    return success;
                }

                bson_array_add_object(arr, &value);
                break;
            }
            case TYPE_ARRAY: {
                if (!lua_istable(L, -1)) {
                    snprintf(errorMessage, 255, "Expected array, but value was of type: %s", 
                             lua_typename(L, lua_type(L, -1)));
                    return -1;
                }
                BsonArray value;
                bson_array_initialize(&value, 4);

                int success = table_to_bson_array(L, &value, errorMessage);
                if (success != 0) {
                    return success;
                }

                bson_array_add_array(arr, &value);
                break;
            }
            case TYPE_INT32: {
                lua_Integer value = luaL_checkinteger(L, -1);
                bson_array_add_int32(arr, (int32_t)value);
                break;
            }
            case TYPE_INT64: {
                lua_Integer value = luaL_checkinteger(L, -1);
                bson_array_add_int64(arr, (int64_t)value);
                break;
            }
            case TYPE_STRING: {
                char *value = strdup(luaL_checkstring(L, -1));
                bson_array_add_string(arr, value);
                free(value);
                break;
            }
            case TYPE_DOUBLE: {
                lua_Number value = luaL_checknumber(L, -1);
                bson_array_add_double(arr, (double)value);
                break;
            }
            case TYPE_BOOLEAN: {
                if (!lua_isboolean(L, -1)) {
                    snprintf(errorMessage, 255, "Expected boolean, but value was of type: %s", 
                             lua_typename(L, lua_type(L, -1)));
                    return -1;
                }
                int value = lua_toboolean(L, -1);

                bson_array_add_bool(arr, (bson_boolean)value);
                break;
            }
            default: {
                snprintf(errorMessage, 255, "Error parsing table, unrecognized type: %i", (int)type);
                return -1;
            }
        }
        lua_pop(L, 2); //Stack: [bson_array]
    }
    lua_pop(L, 1); //Stack: []
    return 0;
}

static int lbson_object_from_bytes(lua_State *L) {
    if (!lua_isstring(L, 1)) {
        return luaL_argerror(L, 1, "Arg 1 must be a Lua String");
    }
    size_t len;
    const char *byte_string = lua_tolstring(L, 1, &len);

    uint8_t *bytes = malloc(len);
    size_t index = 0;
    for (index = 0; index < len; index++) {
        bytes[index] = byte_string[index] & 0xFF;
    }

    BsonObject obj = bson_object_from_bytes(bytes);
    free(bytes);
    
    char errorMessage[255];
    int success = bson_object_to_table(L, &obj, errorMessage);

    bson_object_deinitialize(&obj);
    if (success != 0) {
        return luaL_argerror(L, 1, errorMessage);
    }
    return 1;
}

static int bson_object_to_table(lua_State *L, BsonObject *obj, char *errorMessage) {
    MapIterator iter = bson_object_iterator(obj);
    BsonObjectEntry entry = bson_object_iterator_next(&iter);
    lua_newtable(L); //Stack: [table]
    while (entry.element != NULL) {
        const char *key = entry.key;
        BsonElement *element = entry.element;

        int success = bson_element_to_table(L, element, errorMessage); //Stack [table, {type, value}]
        if (success != 0) {
            return success;
        }

        lua_setfield(L, -2, key); //Stack [table]
        entry = bson_object_iterator_next(&iter);
    }
    return 0;
}

static int bson_array_to_table(lua_State *L, BsonArray *arr, char *errorMessage) {
    lua_newtable(L); //Stack: [table]
    int index;
    for (index = 0; index <= arr->count; index++) {
        BsonElement *element = bson_array_get(arr, index);
        
        int success = bson_element_to_table(L, element, errorMessage); //Stack [table, {type, value}]
        if (success != 0) {
            return success;
        }

        lua_rawseti(L, -2, index + 1 /*Lua indexing*/); //Stack [table]
    }
    return 0;
}

static int bson_element_to_table(lua_State *L, BsonElement *element, char *errorMessage) {
    lua_newtable(L); //Stack: [{}];

    lua_pushinteger(L, ((lua_Integer)element->type) & 0xFF); //Stack: [{}, type]
    lua_setfield(L, -2, "type"); //Stack: [{type}]

    switch (element->type) {
        case TYPE_DOCUMENT: {
            BsonObject *value = (BsonObject *)element->value;
            int success = bson_object_to_table(L, value, errorMessage); //Stack [{type}, subTable]
            if (success != 0) {
                return success;
            }
            break;
        }
        case TYPE_ARRAY: {
            BsonArray *value = (BsonArray *)element->value;
            int success = bson_array_to_table(L, value, errorMessage); //Stack [{type}, subTable]
            if (success != 0) {
                return success;
            }
            break;
        }
        case TYPE_INT32: {
            int32_t value = *(int32_t *)element->value;
            lua_pushinteger(L, (lua_Integer)value); //Stack [{type}, value]
            break;
        }
        case TYPE_INT64: {
            int64_t value = *(int64_t *)element->value;
            lua_pushinteger(L, (lua_Integer)value); //Stack [{type}, value]
            break;
        }
        case TYPE_STRING: {
            char *value = (char *)element->value;
            lua_pushstring(L, value); //Stack [{type}, value]
            break;
        }
        case TYPE_DOUBLE: {
            double value = *(double *)element->value;
            lua_pushnumber(L, (lua_Number)value); //Stack [{type}, value]
            break;
        }
        case TYPE_BOOLEAN: {
            bson_boolean value = *(bson_boolean *)element->value; //Stack [{type}, value]
            lua_pushboolean(L, (int)value);
            break;
        }
        default: {
            snprintf(errorMessage, 255, "Unsupported  type in BSON object: %i", (int)element->type);
            return -1;
        }
    }
    lua_setfield(L, -2, "value"); //Stack [{type, value}]
    return 0;
}

static const struct luaL_Reg bson_object_methods[] = {
    { "to_bytes", lbson_object_to_bytes },
    { "to_table", lbson_object_from_bytes },
    { NULL, NULL }
};

int luaopen_bson4lua(lua_State *L) {
    // Create metatable
    luaL_newmetatable(L, "BSONObject"); //Stack: [{}]

    lua_pushvalue(L, -1); //Stack: [{}, {}]
    lua_setfield(L, -2, "__index"); //Stack: [{"__index":{}}]
    
    luaL_newlib(L, bson_object_methods);
    return 1;
}
