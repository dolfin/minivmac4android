LOCAL_PATH := $(call my-dir)

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/jni/src/GLOBGLUE.c \
	$(LOCAL_PATH)/jni/src/IWMEMDEV.c \
	$(LOCAL_PATH)/jni/src/OSGLUJNI.c \
	$(LOCAL_PATH)/jni/src/M68KITAB.c \
	$(LOCAL_PATH)/jni/src/MINEM68K.c \
	$(LOCAL_PATH)/jni/src/MOUSEMDV.c \
	$(LOCAL_PATH)/jni/src/PROGMAIN.c \
	$(LOCAL_PATH)/jni/src/ROMEMDEV.c \
	$(LOCAL_PATH)/jni/src/RTCEMDEV.c \
	$(LOCAL_PATH)/jni/src/SCCEMDEV.c \
	$(LOCAL_PATH)/jni/src/SCRNEMDV.c \
	$(LOCAL_PATH)/jni/src/SCSIEMDV.c \
	$(LOCAL_PATH)/jni/src/SONYEMDV.c \
	$(LOCAL_PATH)/jni/src/VIAEMDEV.c \


LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/jni \
    $(LOCAL_PATH)/jni/src \
    $(LOCAL_PATH)/jni/cfg \

LOCAL_LDLIBS := -llog
LOCAL_CFLAGS += -O0
