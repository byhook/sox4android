/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_onzhou_sox4android_sox_NativeSox */

#ifndef _Included_com_onzhou_sox4android_sox_NativeSox
#define _Included_com_onzhou_sox4android_sox_NativeSox
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL reverbFile(JNIEnv *env, jobject obj, jstring jinput,
                                  jstring joutput, jint reverbrance,
                                  jint hfDamping, jint roomScale,
                                  jint stereoDepth,
                                  jint preDelay);

/**
 * 内存中转换由于NDK在23以上才支持fmemopen函数
 * 因此此方法暂时不可用
 */
JNIEXPORT jbyteArray JNICALL reverbBuffer
        (JNIEnv *, jobject, jbyteArray, jint, jbyteArray);

#ifdef __cplusplus
}
#endif
#endif
