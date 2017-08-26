MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(MY_LOCAL_PATH)

include $(CLEAR_VARS)
LOCAL_MODULE := mnvmcoreplus
LOCAL_LDFLAGS := -Wl,--build-id

include $(LOCAL_PATH)/main/Android.mk
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(LOCAL_PATH)/macPlus/Android.mk

include $(BUILD_SHARED_LIBRARY)


LOCAL_PATH := $(MY_LOCAL_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE := mnvmcoreii
LOCAL_LDFLAGS := -Wl,--build-id

include $(LOCAL_PATH)/main/Android.mk
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(LOCAL_PATH)/macII/Android.mk

include $(BUILD_SHARED_LIBRARY)
