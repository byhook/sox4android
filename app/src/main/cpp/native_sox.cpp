#include <jni.h>
#include <string>
#include "native_sox.h"

void Java_com_onzhou_sox4android_sox_NativeSox_reverb(JNIEnv *env, jobject obj, jstring jinput,
                                                      jstring joutput) {

    const char *inputPath = env->GetStringUTFChars(jinput, NULL);
    const char *outputPath = env->GetStringUTFChars(joutput, NULL);

    AudioEffect audioEffect;
    audioEffect.Reverb(inputPath, outputPath);

    env->ReleaseStringUTFChars(jinput, inputPath);
    env->ReleaseStringUTFChars(joutput, outputPath);

}
