#include <jni.h>
#include <stdio.h>
#include "native_sox.h"
#include "audio_effect.h"
#include <malloc.h>
#include <assert.h>

#include <android/log.h>


#define LOG_TAG    "NativeEncode"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
    {"init", "()V", (void *) init},
    {"release", "()V", (void *) release},
    {"setReverbParam", "(IIIIII)V", (void *) setReverbParam},
    {"reverbWavFile", "(Ljava/lang/String;Ljava/lang/String;)V", (void *) reverbWavFile},
    {"reverbPcmBuffer", "([BI[B)[B", (void *) reverbPcmBuffer}
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


AudioEffect *audioEffect = NULL;

void init(JNIEnv *env, jobject obj){
     if(audioEffect==NULL){
         audioEffect = new AudioEffect();
     }
}

void release(JNIEnv *env, jobject obj){
     if(audioEffect!=NULL){
         delete audioEffect;
         audioEffect = NULL;
     }
}

void setReverbParam(JNIEnv *env, jobject obj, jint reverbrance,jint hfDamping, jint roomScale,
                                                      jint stereoDepth,
                                                      jint preDelay,
                                                      jint wetGain) {
    if(audioEffect!=NULL){
         audioEffect->SetEffectParam(reverbrance, hfDamping, roomScale, stereoDepth, preDelay,wetGain);
    }
}


void reverbWavFile(JNIEnv *env, jobject obj, jstring jinput,jstring joutput) {

    const char *inputPath = env->GetStringUTFChars(jinput, NULL);
    const char *outputPath = env->GetStringUTFChars(joutput, NULL);


    /*LOGE("################");
    const char *param[] = {"", inputPath, outputPath};
    transfer(3, param);
    LOGE("################");*/

    //setAudioEffect(env,obj,jinput,joutput);

    if(audioEffect!=NULL){
         audioEffect->ReverbFile(inputPath, outputPath);
    }

    env->ReleaseStringUTFChars(jinput, inputPath);
    env->ReleaseStringUTFChars(joutput, outputPath);

}

#define MAX_SAMPLES (size_t)2048

jbyteArray
reverbPcmBuffer(JNIEnv *env, jobject obj, jbyteArray inData, jint inSize, jbyteArray outData) {



    /*char *data = new char[inSize];
    env->GetByteArrayRegion(inData, 0, inSize, reinterpret_cast<jbyte *>(data));
    char *outBuffer;
    int outSize = audioEffect->processBuffer(data, inSize, outBuffer, 44100, 2,
                                              50,
                                              50, 90, 50,
                                              30, 0);
    env->SetByteArrayRegion(outData, 0, outSize, (jbyte *) outBuffer);*/

    //initSox(env,obj);
    LOGE("reverbBuffer start");
    //return setAudioEffectBuffer(env,obj,inData,inSize,outData);
    //jbyteArray byteBuffer =  memorybuffer2(env, obj,inData);
    //closeSox(env,obj);
    //return byteBuffer;
    return NULL;
}
