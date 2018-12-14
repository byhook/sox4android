

#include <jni.h>
#ifndef SOX4ANDROID_MEMORY_H
#define SOX4ANDROID_MEMORY_H

#include <android/log.h>
#include <malloc.h>


#ifdef __cplusplus
extern "C" {
#endif

#include "include/sox.h"
#include "include/util.h"

JNIEXPORT jbyteArray JNICALL memorybuffer2(JNIEnv *env, jobject instance,jbyteArray bytes_);

JNIEXPORT void JNICALL initSox(JNIEnv *env, jobject instance);

JNIEXPORT void JNICALL closeSox(JNIEnv *env, jobject instance);

#ifdef __cplusplus
}
#endif

#endif //SOX4ANDROID_MEMORY_H
