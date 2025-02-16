LOCAL_PATH := $(call my-dir)

LOCAL_SRC_FILES += \
    $(LOCAL_PATH)/../main/jni/src/ADBEMDEV.c \
    $(LOCAL_PATH)/../main/jni/src/ASCEMDEV.c \
    $(LOCAL_PATH)/../main/jni/src/VIA2EMDV.c \
    $(LOCAL_PATH)/../main/jni/src/VIDEMDEV.c \

LOCAL_C_INCLUDES += $(LOCAL_PATH)/jni/cfg

