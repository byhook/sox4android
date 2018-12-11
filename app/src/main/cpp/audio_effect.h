
//

#ifndef SOX4ANDROID_AUDIO_EFFECT_H
#define SOX4ANDROID_AUDIO_EFFECT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "sox.h"

#ifdef __cplusplus
}
#endif

class AudioEffect {


public:

    int Reverb(const char *inputFile, const char *outputFile);

};


#endif //SOX4ANDROID_AUDIO_EFFECT_H
