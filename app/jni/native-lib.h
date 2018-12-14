#include <jni.h>

#ifndef SOX4ANDROID_NATIVE_LIB_H
#define SOX4ANDROID_NATIVE_LIB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "include/sox.h"

JNIEXPORT void JNICALL setAudioEffect(JNIEnv *env, jobject instance,jstring input_,jstring output_);

JNIEXPORT jbyteArray JNICALL setAudioEffectBuffer(JNIEnv *env, jobject instance,jbyteArray inputByte,jint length, jbyteArray output);

int echoEffect(sox_format_t *in,sox_format_t *out, sox_effects_chain_t *chain, sox_effect_t *e, int ret);

int reverbEffect(sox_format_t *in, sox_format_t *out,sox_effects_chain_t *chain, sox_effect_t *e, int ret);

int initEffect(sox_format_t *in, sox_format_t *out,sox_effects_chain_t *chain, sox_effect_t *e, int ret);

int releaseEffect(sox_format_t *in,sox_format_t *out, sox_effects_chain_t *chain,
              sox_effect_t *e, int ret);

int equalizerEffect(sox_format_t *in,sox_format_t *out, sox_effects_chain_t *chain, sox_effect_t *e, int ret);

int highpassEffect(sox_format_t *in,sox_format_t *out, sox_effects_chain_t *chain, sox_effect_t *e, int ret);

int lowpassEffect(sox_format_t *in, sox_format_t *out,sox_effects_chain_t *chain, sox_effect_t *e, int ret);

int compandEffect(sox_format_t *in, sox_format_t *out,sox_effects_chain_t *chain, sox_effect_t *e, int ret);


#ifdef __cplusplus
}
#endif

#endif //SOX4ANDROID_NATIVE_LIB_H
