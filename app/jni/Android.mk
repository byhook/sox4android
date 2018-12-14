LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)    
LOCAL_MODULE    := sox
LOCAL_SRC_FILES := libsox.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := native-sox

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

# missing stdio functions

LOCAL_STATIC_LIBRARIES := sox

LOCAL_SRC_FILES := \
  $(LOCAL_PATH)/src/open_memstream.c \
  $(LOCAL_PATH)/src/fopencookie.c \
  $(LOCAL_PATH)/src/fmemopen.c \
  $(LOCAL_PATH)/audio_effect.cpp \
  $(LOCAL_PATH)/example5.cpp \
  $(LOCAL_PATH)/native_sox.cpp \
  $(LOCAL_PATH)/memory.cpp \
  $(LOCAL_PATH)/native-lib.cpp

LOCAL_CFLAGS += -O3

LOCAL_LDLIBS    := -lm -llog

include $(BUILD_SHARED_LIBRARY)
