#include <jni.h>
#include <stdio.h>
#include "native_sox.h"
#include "stdioext.h"

void Java_com_onzhou_sox4android_sox_NativeSox_reverb(JNIEnv *env, jobject obj, jstring jinput,
                                                      jstring joutput, jint reverbrance,
                                                      jint hfDamping, jint roomScale,
                                                      jint stereoDepth,
                                                      jint preDelay) {

    const char *inputPath = env->GetStringUTFChars(jinput, NULL);
    const char *outputPath = env->GetStringUTFChars(joutput, NULL);

    //AudioEffect *audioEffect = new AudioEffect();

    char buffer[100] = {0};
    fmemopen(buffer, 100, "w+b");

    //audioEffect->SetEffectParam(reverbrance, hfDamping, roomScale, stereoDepth, preDelay);
    //ReverbBuffer(inputPath, outputPath);

    //delete audioEffect;

    env->ReleaseStringUTFChars(jinput, inputPath);
    env->ReleaseStringUTFChars(joutput, outputPath);

}

JNIEXPORT void JNICALL Java_com_onzhou_sox4android_sox_NativeSox_reverbBuffer
        (JNIEnv *env, jobject obj, jbyteArray jbuffer, jint inSize, jbyteArray joutBuffer) {

    jbyte *inputBuffer = env->GetByteArrayElements(jbuffer, NULL);


    env->ReleaseByteArrayElements(jbuffer, inputBuffer, NULL);
}
