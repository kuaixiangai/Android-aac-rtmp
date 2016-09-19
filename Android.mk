# Android.mk for FFmpeg
#
# kuaixiang
# 136062834@qq.com
# 

LOCAL_PATH := $(call my-dir)

# FFmpeg library
#include $(CLEAR_VARS)
#LOCAL_MODULE := avcodec
#LOCAL_SRC_FILES := libavcodec.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := avdevice
#LOCAL_SRC_FILES := libavdevice.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := avfilter
#LOCAL_SRC_FILES := libavfilter.so
#include $(PREBUILT_SHARED_LIBRARY)
##
#include $(CLEAR_VARS)
#LOCAL_MODULE := avformat
#LOCAL_SRC_FILES := libavformat.so
#include $(PREBUILT_SHARED_LIBRARY)
##
#include $(CLEAR_VARS)
#LOCAL_MODULE := avutil
#LOCAL_SRC_FILES := libavutil.so
#include $(PREBUILT_SHARED_LIBRARY)
##
#include $(CLEAR_VARS)
#LOCAL_MODULE := postproc
#LOCAL_SRC_FILES := libpostproc.so
#include $(PREBUILT_SHARED_LIBRARY)
##
#include $(CLEAR_VARS)
#LOCAL_MODULE := swresample
#LOCAL_SRC_FILES := libswresample.so
#include $(PREBUILT_SHARED_LIBRARY)
##
#include $(CLEAR_VARS)
#LOCAL_MODULE := swscale
#LOCAL_SRC_FILES := libswscale.so
#include $(PREBUILT_SHARED_LIBRARY)
#
#include $(CLEAR_VARS)
#LOCAL_MODULE := avresample
#LOCAL_SRC_FILES := libavresample.so
#include $(PREBUILT_SHARED_LIBRARY)

# Program
include $(CLEAR_VARS)
LOCAL_MODULE := sffstreamer
LOCAL_SRC_FILES :=simplest_ffmpeg_streamer.c 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_LDLIBS := -llog -lz -lavfilter -lavdevice  -lavformat -lavcodec  -lavutil -lpostproc -lswresample -lswscale -lavresample -lx264 -lfdk-aac -lfaac
#LOCAL_SHARED_LIBRARIES := avfilter avcodec avdevice  avformat avutil postproc swresample swscale avresample
include $(BUILD_SHARED_LIBRARY)

