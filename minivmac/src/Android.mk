MY_LOCAL_PATH := $(call my-dir)
LOCAL_PATH := $(MY_LOCAL_PATH)

# Build the proxy JNI library: libjni_proxy.so
include $(CLEAR_VARS)
LOCAL_MODULE    := jni_proxy
LOCAL_SRC_FILES := $(LOCAL_PATH)/main/jni/jni_proxy.c
# Link with libdl and liblog for dlopen/dlsym and logging.
LOCAL_LDLIBS    := -ldl -llog
include $(BUILD_SHARED_LIBRARY)

# Build MacPlus library: libmnvmcoreplus.so
include $(CLEAR_VARS)
LOCAL_MODULE := mnvmcoreplus
LOCAL_LDFLAGS := -Wl,--build-id

include $(LOCAL_PATH)/main/Android.mk
LOCAL_PATH := $(MY_LOCAL_PATH)
include $(LOCAL_PATH)/main/jni/variants/macPlus/Android.mk

include $(BUILD_SHARED_LIBRARY)

ifeq ($(NDK_FLAVOR),macII)
	# Build MacII library: libmnvmcoreii.so
	LOCAL_PATH := $(MY_LOCAL_PATH)
	include $(CLEAR_VARS)
	LOCAL_MODULE := mnvmcoreii
	LOCAL_LDFLAGS := -Wl,--build-id

	include $(LOCAL_PATH)/main/Android.mk
	LOCAL_PATH := $(MY_LOCAL_PATH)
	include $(LOCAL_PATH)/main/jni/variants/macII/Android.mk

	include $(BUILD_SHARED_LIBRARY)

	# Build MacII library: libmnvmcoreii1024.so
	LOCAL_PATH := $(MY_LOCAL_PATH)
	include $(CLEAR_VARS)
	LOCAL_MODULE := mnvmcoreii1024
	LOCAL_LDFLAGS := -Wl,--build-id

	include $(LOCAL_PATH)/main/Android.mk
	LOCAL_PATH := $(MY_LOCAL_PATH)
	include $(LOCAL_PATH)/main/jni/variants/macII-1024/Android.mk

	include $(BUILD_SHARED_LIBRARY)

	# Build MacII library: libmnvmcoreii512.so
	LOCAL_PATH := $(MY_LOCAL_PATH)
	include $(CLEAR_VARS)
	LOCAL_MODULE := mnvmcoreii512
	LOCAL_LDFLAGS := -Wl,--build-id

	include $(LOCAL_PATH)/main/Android.mk
	LOCAL_PATH := $(MY_LOCAL_PATH)
	include $(LOCAL_PATH)/main/jni/variants/macII-512/Android.mk

	include $(BUILD_SHARED_LIBRARY)

	# Build Mac128k library: libmnvmcore128.so
	LOCAL_PATH := $(MY_LOCAL_PATH)
	include $(CLEAR_VARS)
	LOCAL_MODULE := mnvmcore128
	LOCAL_LDFLAGS := -Wl,--build-id

	include $(LOCAL_PATH)/main/Android.mk
	LOCAL_PATH := $(MY_LOCAL_PATH)
	include $(LOCAL_PATH)/main/jni/variants/mac128k/Android.mk

	include $(BUILD_SHARED_LIBRARY)
endif