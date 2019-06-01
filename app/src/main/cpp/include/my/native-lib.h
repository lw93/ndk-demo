//
// Created by TR on 2018/11/20.
//


#ifndef NDK_DEMO_NATIVE_HEADER_H
#define NDK_DEMO_NATIVE_HEADER_H

#include <jni.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>

#include "libyuv/libyuv.h"

//封装格式
#include "libavformat/avformat.h"
//解码
#include "libavcodec/avcodec.h"
//缩放
#include "libswscale/swscale.h"

//重采样
#include "libswresample/swresample.h"

#define MAX_AUDIO_FRME_SIZE 48000 * 4 * 1024 //一个frame的内存大小

void native_format_log(const char *tag, const char *log, ...);

void native_threw_exception(JNIEnv *env);

JNIEXPORT void JNICALL Java_com_xygit_ndk_demo_jni_AvPlayApi_startAudio(JNIEnv *, jobject, jstring, jstring);

JNIEXPORT void JNICALL Java_com_xygit_ndk_demo_jni_AvPlayApi_startVedio(JNIEnv *, jobject, jstring, jobject);


#endif //NDK_DEMO_NATIVE_HEADER_H


