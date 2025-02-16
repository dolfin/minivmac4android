LOCAL_PATH := $(call my-dir)

LOCAL_SRC_FILES += \
	$(LOCAL_PATH)/../main/jni/src/KBRDEMDV.c \
	$(LOCAL_PATH)/../main/jni/src/SNDEMDEV.c \


LOCAL_C_INCLUDES += $(LOCAL_PATH)/jni/cfg
