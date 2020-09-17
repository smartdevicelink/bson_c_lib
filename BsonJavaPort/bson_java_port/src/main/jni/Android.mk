LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := bson-c-lib
LOCAL_SRC_FILES := bson_jni.c ../../../../../src/bson_object.c ../../../../../src/emhashmap/emhashmap.c ../../../../../src/bson_array.c ../../../../../src/bson_util.c
LOCAL_LDFLAGS += -Wl,--exclude-libs,libunwind.a
include $(BUILD_SHARED_LIBRARY)
