#include <jni.h>
#include "../../../../../src/bson_object.h"

BsonObject bsonObject;
BsonArray bsonArray;

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1put_1int32(JNIEnv *env, jclass type,
                                                                         jlong bsonRef,
                                                                         jstring key_, jint value) {
    const char* key = (*env)->GetStringUTFChars(env, key_, 0);

    jboolean tf = (jboolean) bson_object_put_int32((BsonObject*) bsonRef, (char *) key, value);

    (*env)->ReleaseStringUTFChars(env, key_, key);

    return tf;
}

JNIEXPORT jbyteArray JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1to_1bytes(JNIEnv *env,
                                                                        jobject instance,
                                                                        jlong bsonRef) {

    jbyte* bytes = (jbyte*) bson_object_to_bytes((BsonObject*) bsonRef);

    jsize capacity = (jsize) bson_object_size((BsonObject*) bsonRef);

    jbyteArray array = (*env)->NewByteArray(env, capacity);

    (*env)->SetByteArrayRegion(env, array, 0, capacity, (jbyte*) bytes);

    return array;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1put_1int64(JNIEnv *env,
                                                                         jobject instance,
                                                                         jlong bsonRef,
                                                                         jstring key_,
                                                                         jlong value) {
    const char* key = (*env)->GetStringUTFChars(env, key_, 0);

    jboolean tf = (jboolean) bson_object_put_int64((BsonObject*) bsonRef, (char *) key, value);

    (*env)->ReleaseStringUTFChars(env, key_, key);

    return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1put_1string(JNIEnv *env, jclass type,
                                                                          jlong bsonRef,
                                                                          jstring key_,
                                                                          jstring value_) {
    const char *key = (*env)->GetStringUTFChars(env, key_, 0);

    const char* value = (*env)->GetStringUTFChars(env, value_, 0);

    jboolean tf = (jboolean) bson_object_put_string((BsonObject*) bsonRef, (char *) key, (char*) value);

    (*env)->ReleaseStringUTFChars(env, key_, key);

    (*env)->ReleaseStringUTFChars(env, value_, value);

    return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1put_1bool(JNIEnv *env, jclass type,
                                                                        jlong bsonRef, jstring key_,
                                                                        jboolean value) {
    const char *key = (*env)->GetStringUTFChars(env, key_, 0);


    bson_boolean bsonBoolean = BOOLEAN_INVALID;

    if(value == false){
        bsonBoolean = BOOLEAN_FALSE;
    }else if(value == true){
        bsonBoolean = BOOLEAN_TRUE;
    }

    jboolean ret = (jboolean) bson_object_put_bool(&bsonObject, (char *) key, bsonBoolean);

    (*env)->ReleaseStringUTFChars(env, key_, key);

    return ret;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1put_1double(JNIEnv *env, jclass type,
                                                                          jlong bsonRef,
                                                                          jstring key_,
                                                                          jdouble value) {
    const char* key = (*env)->GetStringUTFChars(env, key_, 0);

    jboolean tf = (jboolean) bson_object_put_double((BsonObject*) bsonRef, (char *) key, value);

    (*env)->ReleaseStringUTFChars(env, key_, key);

    return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1put_1object(JNIEnv *env, jclass type,
                                                                          jlong bsonRef,
                                                                          jstring key_,
                                                                          jbyteArray value_) {
    const char *key = (*env)->GetStringUTFChars(env, key_, 0);
    jbyte *value = (*env)->GetByteArrayElements(env, value_, NULL);

    BsonObject bsonFromBytes = bson_object_from_bytes((uint8_t*) value);
    jboolean tf = bson_object_put_object((BsonObject*) bsonRef, key, &bsonFromBytes);

    (*env)->ReleaseStringUTFChars(env, key_, key);
    (*env)->ReleaseByteArrayElements(env, value_, value, 0);

    return tf;
}

JNIEXPORT jlong JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1from_1bytes(JNIEnv *env,
                                                                          jobject instance,
                                                                          jbyteArray data_) {
    jbyte *data = (*env)->GetByteArrayElements(env, data_, NULL);

    bsonObject = bson_object_from_bytes((uint8_t *) data);

    (*env)->ReleaseByteArrayElements(env, data_, data, 0);

    return (jlong) &bsonObject;
}

JNIEXPORT jlong JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_initializeBsonArray(JNIEnv *env, jclass type) {

    int default_size = 5;
    bson_array_initialize(&bsonArray, default_size);
    return (jlong) &bsonArray;

}

JNIEXPORT void JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_deinitializeBsonArray(JNIEnv *env, jclass type,
                                                                      jlong bsonRef) {

    bson_array_deinitialize((BsonArray*) bsonRef);

}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1array_1add_1int32(JNIEnv *env, jclass type,
                                                                        jlong bsonRef, jint value) {

    jboolean tf = (jboolean) bson_array_add_int32((BsonArray*) bsonRef, value);

    return tf;

}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1array_1add_1int64(JNIEnv *env, jclass type,
                                                                        jlong bsonRef,
                                                                        jlong value) {

    jboolean tf = (jboolean) bson_array_add_int64((BsonArray*) bsonRef, value);

    return tf;

}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1array_1add_1string(JNIEnv *env, jclass type,
                                                                         jlong bsonRef,
                                                                         jstring value_) {
    const char *value = (*env)->GetStringUTFChars(env, value_, 0);

    jboolean tf = (jboolean) bson_array_add_string((BsonArray*) bsonRef, (char*) value);

    (*env)->ReleaseStringUTFChars(env, value_, value);

    return tf;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1array_1add_1bool(JNIEnv *env, jclass type,
                                                                       jlong bsonRef,
                                                                       jboolean value) {

    bson_boolean bsonBoolean = BOOLEAN_INVALID;

    if(value == false){
        bsonBoolean = BOOLEAN_FALSE;
    }else if(value == true){
        bsonBoolean = BOOLEAN_TRUE;
    }

    jboolean ret = (jboolean) bson_array_add_bool((BsonArray*) bsonRef, bsonBoolean);

    return ret;
}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1array_1add_1double(JNIEnv *env, jclass type,
                                                                         jlong bsonRef,
                                                                         jdouble value) {

    jboolean tf = (jboolean) bson_array_add_double((BsonArray*) bsonRef, value);

    return tf;

}

JNIEXPORT jboolean JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1put_1array(JNIEnv *env,
                                                                         jobject instance,
                                                                         jlong bsonRef,
                                                                         jstring key_,
                                                                         jlong arrayRef) {
    const char *key = (*env)->GetStringUTFChars(env, key_, 0);

    jboolean tf = bson_object_put_array((BsonObject*) bsonRef, (char*) key, (BsonArray*) arrayRef);

    (*env)->ReleaseStringUTFChars(env, key_, key);

    return tf;
}

JNIEXPORT jlong JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_initializeBsonObject(JNIEnv *env, jclass type) {

    bson_object_initialize_default(&bsonObject);
    return (long) &bsonObject;

}

JNIEXPORT void JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_deinitializeBsonObject(JNIEnv *env, jclass type,
                                                                       jlong bsonRef) {

    bson_object_deinitialize((BsonObject*) bsonRef);

}

JNIEXPORT jobject JNICALL
Java_com_livio_bsonjavaport_BSON_BsonEncoder_bson_1object_1get_1hashmap(JNIEnv *env, jclass type,
                                                                        jlong bsonRef) {

    // initialize the HashMap class
    jclass mapClass = (*env)->FindClass(env, "java/util/HashMap");

    jsize map_len = (jsize) bson_object_size((BsonObject*) bsonRef);

    jmethodID init = (*env)->GetMethodID(env, mapClass, "<init>", "(I)V");
    jobject hashMap = (*env)->NewObject(env, mapClass, init, map_len);

    // initialize the put method of the HashMap class
    jmethodID put = (*env)->GetMethodID(env, mapClass, "put",
                                        "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    MapIterator it = bson_object_iterator((BsonObject*) bsonRef);

    MapEntry* entry;

    while((entry = emhashmap_iterator_next(&it)) != NULL){
        jstring key = (*env)->NewStringUTF(env, entry->key);
        BsonElement* element = entry->value;
        if(element->type == TYPE_ARRAY){
            BsonArray* array = (BsonArray *)element->value;

            // initialize the ArrayList class
            jclass listClass = (*env)->FindClass(env, "java/util/ArrayList");

            jsize list_len = (jsize) array->count;

            jmethodID initList = (*env)->GetMethodID(env, listClass, "<init>", "(I)V");
            jobject objList = (*env)->NewObject(env, listClass, initList, list_len);

            // initialize the Add method of the class
            jmethodID add = (*env)->GetMethodID(env, listClass, "add", "(Ljava/lang/Object;)Z");

            int i;
            for(i = 0; i < array->count; i++){
                BsonElement* a_elmnt = bson_array_get(array, i);
                if(a_elmnt->type == TYPE_INT32){
                    jint result = *(int32_t *) a_elmnt->value;

                    jclass intClass = (*env)->FindClass(env, "java/lang/Integer");
                    jmethodID intInit = (*env)->GetMethodID(env, intClass, "<init>", "(I)V");
                    jobject obj = (*env)->NewObject(env, intClass, intInit, result);

                    (*env)->CallBooleanMethod(env, objList, add, obj);
                }else if(a_elmnt->type  == TYPE_INT64){
                    jlong result = *(int64_t *) a_elmnt->value;

                    jclass longClass = (*env)->FindClass(env, "java/lang/Long");
                    jmethodID longInit = (*env)->GetMethodID(env, longClass, "<init>", "(J)V");
                    jobject obj = (*env)->NewObject(env, longClass, longInit, result);

                    (*env)->CallBooleanMethod(env, objList, add, obj);
                }else if(a_elmnt->type == TYPE_STRING){
                    jstring obj = (*env)->NewStringUTF(env, (char *) a_elmnt->value);
                    (*env)->CallBooleanMethod(env, objList, add, obj);
                }else if(a_elmnt->type  == TYPE_BOOLEAN){
                    bson_boolean bb = *(bson_boolean *) a_elmnt->value;
                    jboolean result = false;
                    if(bb == BOOLEAN_FALSE || bb == BOOLEAN_INVALID){
                        result = false;
                    }else if(bb == BOOLEAN_TRUE){
                        result = true;
                    }

                    jclass boolClass = (*env)->FindClass(env, "java/lang/Boolean");
                    jmethodID boolInit = (*env)->GetMethodID(env, boolClass, "<init>", "(Z)V");
                    jobject obj = (*env)->NewObject(env, boolClass, boolInit, result);

                    (*env)->CallBooleanMethod(env, objList, add, obj);
                }else if(a_elmnt->type == TYPE_DOUBLE){
                    jdouble result = *(double *) a_elmnt->value;

                    jclass doubleClass = (*env)->FindClass(env, "java/lang/Double");
                    jmethodID doubleInit = (*env)->GetMethodID(env, doubleClass, "<init>", "(D)V");
                    jobject obj = (*env)->NewObject(env, doubleClass, doubleInit, result);

                    (*env)->CallBooleanMethod(env, objList, add, obj);
                }
            }

            (*env)->CallObjectMethod(env, hashMap, put, key, objList);

        }else if(element->type == TYPE_INT32){
            jint result = *(int32_t *) element->value;

            jclass intClass = (*env)->FindClass(env, "java/lang/Integer");
            jmethodID intInit = (*env)->GetMethodID(env, intClass, "<init>", "(I)V");
            jobject obj = (*env)->NewObject(env, intClass, intInit, result);

            (*env)->CallObjectMethod(env, hashMap, put, key, obj);
        }else if(element->type  == TYPE_INT64){
            jlong result = *(int64_t *) element->value;

            jclass longClass = (*env)->FindClass(env, "java/lang/Long");
            jmethodID longInit = (*env)->GetMethodID(env, longClass, "<init>", "(J)V");
            jobject obj = (*env)->NewObject(env, longClass, longInit, result);

            (*env)->CallObjectMethod(env, hashMap, put, key, obj);
        }else if(element->type == TYPE_STRING){
            jstring obj = (*env)->NewStringUTF(env, (char *) element->value);
            (*env)->CallObjectMethod(env, hashMap, put, key, obj);
        }else if(element->type  == TYPE_BOOLEAN){
            bson_boolean bb = *(bson_boolean *) element->value;
            jboolean result = false;
            if(bb == BOOLEAN_FALSE || bb == BOOLEAN_INVALID){
                result = false;
            }else if(bb == BOOLEAN_TRUE){
                result = true;
            }

            jclass boolClass = (*env)->FindClass(env, "java/lang/Boolean");
            jmethodID boolInit = (*env)->GetMethodID(env, boolClass, "<init>", "(Z)V");
            jobject obj = (*env)->NewObject(env, boolClass, boolInit, result);

            (*env)->CallObjectMethod(env, hashMap, put, key, obj);
        }else if(element->type == TYPE_DOUBLE){
            jdouble result = *(double *) element->value;

            jclass doubleClass = (*env)->FindClass(env, "java/lang/Double");
            jmethodID doubleInit = (*env)->GetMethodID(env, doubleClass, "<init>", "(D)V");
            jobject obj = (*env)->NewObject(env, doubleClass, doubleInit, result);

            (*env)->CallObjectMethod(env, hashMap, put, key, obj);
        }
    }

    return hashMap;
}