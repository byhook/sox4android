
#ifndef SOX4ANDROID_AUDIO_EFFECT_H
#define SOX4ANDROID_AUDIO_EFFECT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "include/sox.h"

class AudioEffect {

private:

    int reverBrance;
    int hfDamping;
    int roomScale;
    int stereoDepth;
    int preDelay;

    sox_format_t *in, *out;
    sox_effects_chain_t *chain;
    sox_effect_t *reverbEffect;

public:

    void SetEffectParam(int reverbrance, int hfDamping, int roomScale, int stereoDepth,
                        int preDelay);

    int ReverbFile(const char *inputFile, const char *outputFile);


    int ReverbBuffer(char *inBuffer, int inSize, char *outBuffer);

    int processBuffer(char *inBuffer, int inSize, char *outBuffer, int sampleRate, int channel,
                          int reverberance, int HFDamping,
                          int roomScale,
                          int stereoDepth,
                          int preDelay, int wetGain);

    void Init();

    int ReverbPCMFile(const char *inputFile, const char *outputFile);

    void Destroy();

};


#ifdef __cplusplus
}
#endif

#endif //SOX4ANDROID_AUDIO_EFFECT_H
