

#include "audio_effect.h"

#ifdef NDEBUG /* N.B. assert used with active statements so enable always. */
#undef NDEBUG /* Must undef above assert.h or other that might include it. */
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


void AudioEffect::SetEffectParam(int reverbrance, int hfDamping, int roomScale, int stereoDepth,
                                 int preDelay) {
    this->reverBrance = reverbrance;
    this->hfDamping = hfDamping;
    this->roomScale = roomScale;
    this->stereoDepth = stereoDepth;
    this->preDelay = preDelay;
}

int AudioEffect::ReverbFile(const char *inputFile, const char *outputFile) {
    static sox_format_t *in, *out;
    sox_effects_chain_t *chain;
    sox_effect_t *e;
    char *args[10];

    assert(sox_init() == SOX_SUCCESS);

    //打开输入文件
    assert(in = sox_open_read(inputFile, NULL, NULL, NULL));

    printf("###### 输入 %d %d", in->signal.rate, in->signal.channels);

    //打开输出文件
    assert(out = sox_open_write(outputFile, &in->signal, NULL, NULL, NULL, NULL));

    printf("###### 输出 %d %d", out->signal.rate, out->signal.channels);

    //创建混响链
    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    //开始输入
    e = sox_create_effect(sox_find_effect("input"));
    args[0] = (char *) in, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    e = sox_create_effect(sox_find_effect("echo"));
    char *grin_in = "0.8";
    char *gain_out = "0.88";
    char *delay = "40";
    char *arg4 = "0.45";
    char *arg5 = "60";
    char *arg6 = "0.4";
    char *echoArgs[] = {grin_in, gain_out, delay, arg4, arg5, arg6};
    assert(sox_effect_options(e, 6, echoArgs) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    //混响器
    e = sox_create_effect(sox_find_effect("reverb"));

    char *wetOnly = "-w";
    //混响大小
    char creverbrance[10] = {0};
    sprintf(creverbrance, "%d", reverBrance);
    //高频阻尼
    char chfDamping[10] = {0};
    sprintf(chfDamping, "%d", hfDamping);
    //房间大小
    char croomScale[10] = {0};
    sprintf(croomScale, "%d", roomScale);
    //立体声深度
    char cstereoDepth[10] = {0};
    sprintf(cstereoDepth, "%d", stereoDepth);
    //早反射声的时间
    char cpreDelay[10] = {0};
    sprintf(cpreDelay, "%d", preDelay);

    char *wetGain = "0";

    char *params[] = {creverbrance, chfDamping, croomScale, cstereoDepth, cpreDelay, wetGain};

    assert(sox_effect_options(e, 6, params) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    //开始输出
    e = sox_create_effect(sox_find_effect("output"));
    args[0] = (char *) out, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    /* Flow samples through the effects processing chain until EOF is reached */
    sox_flow_effects(chain, NULL, NULL);

    /* All done; tidy up: */
    sox_delete_effects_chain(chain);
    sox_close(out);
    sox_close(in);
    sox_quit();
    return 0;
}

int AudioEffect::ReverbBuffer(const char *inputFile, const char *outputFile) {
    return 0;
}