
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

public:

    void SetEffectParam(int reverbrance, int hfDamping, int roomScale, int stereoDepth,
                        int preDelay);

    int ReverbFile(const char *inputFile, const char *outputFile);


    int ReverbBuffer(const char *inputFile, const char *outputFile);

};


#ifdef __cplusplus
}
#endif

#endif //SOX4ANDROID_AUDIO_EFFECT_H
