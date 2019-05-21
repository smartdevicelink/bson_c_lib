#include "../../../../../src/bson_object.h"
#include <jni.h>
#include <syslog.h>

jobject bson_object_to_hashmap(JNIEnv *env, BsonObject *bsonRef);
jobject bson_array_to_list(JNIEnv *env, BsonArray *bsonRef);

JNIEXPORT jlong JNICALL
Java_com_livio_BSON_BsonEncoder_initializeBsonObject(JNIEnv *env, jclass type) {

  BsonObject *bsonObject = malloc(sizeof(BsonObject));
  bson_object_initialize_default(bsonObject);
  return (long)bsonObject;
}

JNIEXPORT void JNICALL Java_com_livio_BSON_BsonEncoder_deinitializeBsonObject(
    JNIEnv *env, jclass type, jlong bsonRef) {

  bson_object_deinitialize((BsonObject *)bsonRef);
  free((BsonObject *)bsonRef);
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1put_1int32(
    JNIEnv *env, jclass type, jlong bsonRef, jstring key_, jint value) {
  const char *key = (*env)->GetStringUTFChars(env, key_, 0);

  jboolean tf =
      (jboolean)bson_object_put_int32((BsonObject *)bsonRef, key, value);

  (*env)->ReleaseStringUTFChars(env, key_, key);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1put_1int64(
    JNIEnv *env, jclass type, jlong bsonRef, jstring key_, jlong value) {
  const char *key = (*env)->GetStringUTFChars(env, key_, 0);

  jboolean tf =
      (jboolean)bson_object_put_int64((BsonObject *)bsonRef, key, value);

  (*env)->ReleaseStringUTFChars(env, key_, key);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1put_1string(
    JNIEnv *env, jclass type, jlong bsonRef, jstring key_, jstring value_) {
  const char *key = (*env)->GetStringUTFChars(env, key_, 0);

  const char *value = (*env)->GetStringUTFChars(env, value_, 0);

  jboolean tf = (jboolean)bson_object_put_string((BsonObject *)bsonRef, key,
                                                 (char *)value);

  (*env)->ReleaseStringUTFChars(env, key_, key);

  (*env)->ReleaseStringUTFChars(env, value_, value);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1put_1bool(
    JNIEnv *env, jclass type, jlong bsonRef, jstring key_, jboolean value) {
  const char *key = (*env)->GetStringUTFChars(env, key_, 0);

  bson_boolean bsonBoolean = BOOLEAN_INVALID;

  if (value == false) {
    bsonBoolean = BOOLEAN_FALSE;
  } else if (value == true) {
    bsonBoolean = BOOLEAN_TRUE;
  }

  jboolean ret =
      (jboolean)bson_object_put_bool((BsonObject *)bsonRef, key, bsonBoolean);

  (*env)->ReleaseStringUTFChars(env, key_, key);

  return ret;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1put_1double(
    JNIEnv *env, jclass type, jlong bsonRef, jstring key_, jdouble value) {
  const char *key = (*env)->GetStringUTFChars(env, key_, 0);

  jboolean tf =
      (jboolean)bson_object_put_double((BsonObject *)bsonRef, key, value);

  (*env)->ReleaseStringUTFChars(env, key_, key);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1put_1object(
    JNIEnv *env, jclass type, jlong bsonRef, jstring key_, jlong value) {
  const char *key = (*env)->GetStringUTFChars(env, key_, 0);

  jboolean tf = (jboolean)bson_object_put_object((BsonObject *)bsonRef, key,
                                                 (BsonObject *)value);

  (*env)->ReleaseStringUTFChars(env, key_, key);
  // The BsonObject struct has been copied into the root object, no need to keep
  // the original version
  free((BsonObject *)value);

  return tf;
}
JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1put_1array(
    JNIEnv *env, jclass type, jlong bsonRef, jstring key_, jlong value) {
  const char *key = (*env)->GetStringUTFChars(env, key_, 0);

  jboolean tf = (jboolean)bson_object_put_array((BsonObject *)bsonRef, key,
                                                (BsonArray *)value);

  (*env)->ReleaseStringUTFChars(env, key_, key);
  // The BsonArray struct has been copied into the object, no need to keep the
  // original version
  free((BsonArray *)value);

  return tf;
}

JNIEXPORT jbyteArray JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1to_1bytes(JNIEnv *env,
                                                        jclass type,
                                                        jlong bsonRef) {

  jbyte *bytes = (jbyte *)bson_object_to_bytes((BsonObject *)bsonRef);

  jsize capacity = (jsize)bson_object_size((BsonObject *)bsonRef);

  jbyteArray array = (*env)->NewByteArray(env, capacity);

  (*env)->SetByteArrayRegion(env, array, 0, capacity, (jbyte *)bytes);

  return array;
}

JNIEXPORT jlong JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1from_1bytes(JNIEnv *env,
                                                          jclass type,
                                                          jbyteArray data_) {
  BsonObject *bsonObject = malloc(sizeof(BsonObject));
  jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);
  jsize len = (*env)->GetArrayLength(env, data_);

  size_t bytesRead =
      bson_object_from_bytes_len(bsonObject, (uint8_t *)data, (size_t)len);

  (*env)->ReleaseByteArrayElements(env, data_, data, 0);
  if (bytesRead != (size_t)len) {
    syslog(LOG_CRIT, "Unexpected length of object, returning -1");
    free(bsonObject);
    return -1;
  }

  return (jlong)bsonObject;
}

JNIEXPORT jlong JNICALL Java_com_livio_BSON_BsonEncoder_initializeBsonArray(
    JNIEnv *env, jclass type, jlong size) {

  BsonArray *bsonArray = malloc(sizeof(BsonArray));
  bson_array_initialize(bsonArray, (size_t)size);
  return (jlong)bsonArray;
}

JNIEXPORT void JNICALL Java_com_livio_BSON_BsonEncoder_deinitializeBsonArray(
    JNIEnv *env, jclass type, jlong bsonRef) {

  bson_array_deinitialize((BsonArray *)bsonRef);
  free((BsonArray *)bsonRef);
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1array_1add_1int32(JNIEnv *env,
                                                        jclass type,
                                                        jlong bsonRef,
                                                        jint value) {

  jboolean tf = (jboolean)bson_array_add_int32((BsonArray *)bsonRef, value);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1array_1add_1int64(JNIEnv *env,
                                                        jclass type,
                                                        jlong bsonRef,
                                                        jlong value) {

  jboolean tf = (jboolean)bson_array_add_int64((BsonArray *)bsonRef, value);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1array_1add_1string(JNIEnv *env,
                                                         jclass type,
                                                         jlong bsonRef,
                                                         jstring value_) {
  const char *value = (*env)->GetStringUTFChars(env, value_, 0);

  jboolean tf =
      (jboolean)bson_array_add_string((BsonArray *)bsonRef, (char *)value);

  (*env)->ReleaseStringUTFChars(env, value_, value);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1array_1add_1bool(JNIEnv *env, jclass type,
                                                       jlong bsonRef,
                                                       jboolean value) {

  bson_boolean bsonBoolean = BOOLEAN_INVALID;

  if (value == false) {
    bsonBoolean = BOOLEAN_FALSE;
  } else if (value == true) {
    bsonBoolean = BOOLEAN_TRUE;
  }

  jboolean ret =
      (jboolean)bson_array_add_bool((BsonArray *)bsonRef, bsonBoolean);

  return ret;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1array_1add_1double(JNIEnv *env,
                                                         jclass type,
                                                         jlong bsonRef,
                                                         jdouble value) {

  jboolean tf = (jboolean)bson_array_add_double((BsonArray *)bsonRef, value);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1array_1add_1object(JNIEnv *env,
                                                         jclass type,
                                                         jlong bsonRef,
                                                         jlong value) {

  jboolean tf = (jboolean)bson_array_add_object((BsonArray *)bsonRef,
                                                (BsonObject *)value);
  // The BsonObject struct has been copied into the array, no need to keep the
  // original version
  free((BsonObject *)value);

  return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1array_1add_1array(JNIEnv *env,
                                                        jclass type,
                                                        jlong bsonRef,
                                                        jlong value) {

  jboolean tf =
      (jboolean)bson_array_add_array((BsonArray *)bsonRef, (BsonArray *)value);
  // The BsonArray struct has been copied into the root array, no need to keep
  // the original version
  free((BsonArray *)value);

  return tf;
}

jobject bson_object_to_hashmap(JNIEnv *env, BsonObject *bsonRef) {
  // initialize the HashMap class
  jclass mapClass = (*env)->FindClass(env, "java/util/HashMap");

  jsize map_len = (jsize)bson_object_size((BsonObject *)bsonRef);

  jmethodID init = (*env)->GetMethodID(env, mapClass, "<init>", "(I)V");
  jobject hashMap = (*env)->NewObject(env, mapClass, init, map_len);

  // initialize the put method of the HashMap class
  jmethodID put = (*env)->GetMethodID(
      env, mapClass, "put",
      "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

  MapIterator it = bson_object_iterator(bsonRef);

  MapEntry *entry;

  while ((entry = emhashmap_iterator_next(&it)) != NULL) {
    jstring key = (*env)->NewStringUTF(env, entry->key);
    BsonElement *element = entry->value;

    if (element->type == TYPE_DOCUMENT) {
      BsonObject *ref = (BsonObject *)element->value;

      jobject obj = bson_object_to_hashmap(env, ref);

      (*env)->CallObjectMethod(env, hashMap, put, key, obj);
    } else if (element->type == TYPE_ARRAY) {
      BsonArray *array = (BsonArray *)element->value;

      jobject obj = bson_array_to_list(env, array);

      (*env)->CallObjectMethod(env, hashMap, put, key, obj);
    } else if (element->type == TYPE_INT32) {
      jint result = *(int32_t *)element->value;

      jclass intClass = (*env)->FindClass(env, "java/lang/Integer");
      jmethodID intInit = (*env)->GetMethodID(env, intClass, "<init>", "(I)V");
      jobject obj = (*env)->NewObject(env, intClass, intInit, result);

      (*env)->CallObjectMethod(env, hashMap, put, key, obj);
    } else if (element->type == TYPE_INT64) {
      jlong result = *(int64_t *)element->value;

      jclass longClass = (*env)->FindClass(env, "java/lang/Long");
      jmethodID longInit =
          (*env)->GetMethodID(env, longClass, "<init>", "(J)V");
      jobject obj = (*env)->NewObject(env, longClass, longInit, result);

      (*env)->CallObjectMethod(env, hashMap, put, key, obj);
    } else if (element->type == TYPE_STRING) {
      jstring obj = (*env)->NewStringUTF(env, (char *)element->value);
      (*env)->CallObjectMethod(env, hashMap, put, key, obj);
    } else if (element->type == TYPE_BOOLEAN) {
      bson_boolean bb = *(bson_boolean *)element->value;
      jboolean result = false;
      if (bb == BOOLEAN_FALSE || bb == BOOLEAN_INVALID) {
        result = false;
      } else if (bb == BOOLEAN_TRUE) {
        result = true;
      }

      jclass boolClass = (*env)->FindClass(env, "java/lang/Boolean");
      jmethodID boolInit =
          (*env)->GetMethodID(env, boolClass, "<init>", "(Z)V");
      jobject obj = (*env)->NewObject(env, boolClass, boolInit, result);

      (*env)->CallObjectMethod(env, hashMap, put, key, obj);
    } else if (element->type == TYPE_DOUBLE) {
      jdouble result = *(double *)element->value;

      jclass doubleClass = (*env)->FindClass(env, "java/lang/Double");
      jmethodID doubleInit =
          (*env)->GetMethodID(env, doubleClass, "<init>", "(D)V");
      jobject obj = (*env)->NewObject(env, doubleClass, doubleInit, result);

      (*env)->CallObjectMethod(env, hashMap, put, key, obj);
    }
  }

  return hashMap;
}

jobject bson_array_to_list(JNIEnv *env, BsonArray *bsonRef) {
  // initialize the ArrayList class
  jclass listClass = (*env)->FindClass(env, "java/util/ArrayList");

  jsize list_len = (jsize)bsonRef->count;

  jmethodID initList = (*env)->GetMethodID(env, listClass, "<init>", "(I)V");
  jobject list = (*env)->NewObject(env, listClass, initList, list_len);

  // initialize the Add method of the class
  jmethodID add =
      (*env)->GetMethodID(env, listClass, "add", "(Ljava/lang/Object;)Z");

  size_t i;
  for (i = 0; i < bsonRef->count; i++) {
    BsonElement *element = bson_array_get(bsonRef, (size_t)i);
    if (element->type == TYPE_DOCUMENT) {
      BsonObject *ref = (BsonObject *)element->value;

      jobject obj = bson_object_to_hashmap(env, ref);

      (*env)->CallBooleanMethod(env, list, add, obj);
    } else if (element->type == TYPE_ARRAY) {
      BsonArray *array = (BsonArray *)element->value;

      jobject obj = bson_array_to_list(env, array);

      (*env)->CallBooleanMethod(env, list, add, obj);
    } else if (element->type == TYPE_INT32) {
      jint result = *(int32_t *)element->value;

      jclass intClass = (*env)->FindClass(env, "java/lang/Integer");
      jmethodID intInit = (*env)->GetMethodID(env, intClass, "<init>", "(I)V");
      jobject obj = (*env)->NewObject(env, intClass, intInit, result);

      (*env)->CallBooleanMethod(env, list, add, obj);
    } else if (element->type == TYPE_INT64) {
      jlong result = *(int64_t *)element->value;

      jclass longClass = (*env)->FindClass(env, "java/lang/Long");
      jmethodID longInit =
          (*env)->GetMethodID(env, longClass, "<init>", "(J)V");
      jobject obj = (*env)->NewObject(env, longClass, longInit, result);

      (*env)->CallBooleanMethod(env, list, add, obj);
    } else if (element->type == TYPE_STRING) {
      jstring obj = (*env)->NewStringUTF(env, (char *)element->value);
      (*env)->CallBooleanMethod(env, list, add, obj);
    } else if (element->type == TYPE_BOOLEAN) {
      bson_boolean bb = *(bson_boolean *)element->value;
      jboolean result = false;
      if (bb == BOOLEAN_FALSE || bb == BOOLEAN_INVALID) {
        result = false;
      } else if (bb == BOOLEAN_TRUE) {
        result = true;
      }

      jclass boolClass = (*env)->FindClass(env, "java/lang/Boolean");
      jmethodID boolInit =
          (*env)->GetMethodID(env, boolClass, "<init>", "(Z)V");
      jobject obj = (*env)->NewObject(env, boolClass, boolInit, result);

      (*env)->CallBooleanMethod(env, list, add, obj);
    } else if (element->type == TYPE_DOUBLE) {
      jdouble result = *(double *)element->value;

      jclass doubleClass = (*env)->FindClass(env, "java/lang/Double");
      jmethodID doubleInit =
          (*env)->GetMethodID(env, doubleClass, "<init>", "(D)V");
      jobject obj = (*env)->NewObject(env, doubleClass, doubleInit, result);

      (*env)->CallBooleanMethod(env, list, add, obj);
    }
  }
  return list;
}

JNIEXPORT jobject JNICALL
Java_com_livio_BSON_BsonEncoder_bson_1object_1get_1hashmap(JNIEnv *env,
                                                           jclass type,
                                                           jlong bsonRef) {
  if (bsonRef == -1) {
    syslog(LOG_CRIT, "Invalid BSON Object");
    return (*env)->NewGlobalRef(env, NULL);
  }

  return bson_object_to_hashmap(env, (BsonObject *)bsonRef);
}
