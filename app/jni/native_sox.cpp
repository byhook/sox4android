#include <jni.h>
#include <stdio.h>
#include "native_sox.h"
#include "audio_effect.h"
#include "example5.h"


#include <android/log.h>


#define LOG_TAG    "NativeSox"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)





/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"reverbFile", "(Ljava/lang/String;Ljava/lang/String;IIIII)V", (void *) reverbFile}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/onzhou/sox4android/sox/NativeSox");
    if ((env->RegisterNatives(cl, methods, sizeof(methods) / sizeof(methods[0]))) < 0) {
        return -1;
    }
    return 0;
}

/**
 * 加载默认回调
 * @param vm
 * @param reserved
 * @return
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}


void reverbFile(JNIEnv *env, jobject obj, jstring jinput,
                jstring joutput, jint reverbrance,
                jint hfDamping, jint roomScale,
                jint stereoDepth,
                jint preDelay) {

    const char *inputPath = env->GetStringUTFChars(jinput, NULL);
    const char *outputPath = env->GetStringUTFChars(joutput, NULL);


    LOGE("################");
    const char *param[] = {"", inputPath, outputPath};
    transfer(3, param);
    LOGE("################");

    /*AudioEffect *audioEffect = new AudioEffect();

    audioEffect->SetEffectParam(reverbrance, hfDamping, roomScale, stereoDepth, preDelay);
    audioEffect->ReverbFile(inputPath, outputPath);

    delete audioEffect;*/

    env->ReleaseStringUTFChars(jinput, inputPath);
    env->ReleaseStringUTFChars(joutput, outputPath);

}

void
reverbBuffer(JNIEnv *env, jobject obj, jbyteArray jbuffer, jint inSize, jbyteArray joutBuffer) {


}
