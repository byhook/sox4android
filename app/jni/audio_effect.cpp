

#include "audio_effect.h"
#include "logger.h"

#ifdef NDEBUG /* N.B. assert used with active statements so enable always. */
#undef NDEBUG /* Must undef above assert.h or other that might include it. */
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


void AudioEffect::SetEffectParam(int reverbrance, int hfDamping, int roomScale, int stereoDepth,
                                 int preDelay,int wetGain) {
    this->reverBrance = reverbrance;
    this->hfDamping = hfDamping;
    this->roomScale = roomScale;
    this->stereoDepth = stereoDepth;
    this->preDelay = preDelay;
    this->wetGain = wetGain;
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


int AudioEffect::processBuffer(char *inBuffer, int inSize, char *outBuffer, int sampleRate,
                                int channel,
                                int reverberance,
                                int HFDamping,
                                int roomScale,
                                int stereoDepth,
                                int preDelay, int wetGain) {
    sox_format_t *in, *out;
    sox_effects_chain_t *chain;
    sox_effect_t *reverbEffect;

    assert(sox_init() == SOX_SUCCESS);

    sox_encodinginfo_t in_enc;
    sox_signalinfo_t in_sig;

    in_enc.bits_per_sample = 16;
    in_enc.encoding = SOX_ENCODING_SIGN2;
    in_enc.compression = 0.f;
    in_enc.opposite_endian = sox_false;
    in_enc.reverse_bits = sox_option_no;
    in_enc.reverse_bytes = sox_option_no;
    in_enc.reverse_nibbles = sox_option_no;

    in_sig.rate = 44100.f;
    in_sig.channels = 2;
    in_sig.length = 0;
    in_sig.precision = 16;
    in_sig.mult = NULL;

    assert(in = sox_open_mem_read(inBuffer, inSize,&in_sig, &in_enc, "s16"));
    size_t tmpBufferSize;
    assert(out = sox_open_memstream_write(&outBuffer, &tmpBufferSize, &in->signal, NULL, "s16",
                                          NULL));

    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    /* Create an effects chain; some effects need to know about the input
   * or output file encoding so we provide that information here */
    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    sox_effect_t *e;
    /* The first effect in the effect chain must be something that can source
     * samples; in this case, we use the built-in handler that inputs
     * data from an audio file */
    e = sox_create_effect(sox_find_effect("input"));
    char *args[10];
    args[0] = (char *) in;
    assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    /* This becomes the first `effect' in the chain */
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    reverbEffect = sox_create_effect(sox_find_effect("reverb"));
    char *wetOnly = "-w";
    char reverberanceChar[5];
    sprintf(reverberanceChar, "%d", reverberance);
    char hfDampingChar[5];
    sprintf(hfDampingChar, "%d", HFDamping);
    char roomScaleChar[5];
    sprintf(roomScaleChar, "%d", roomScale);
    char stereoDepthChar[5];
    sprintf(stereoDepthChar, "%d", stereoDepth);
    char preDelayChar[5];
    sprintf(preDelayChar, "%d", preDelay);
    char wetGainChar[5];
    sprintf(wetGainChar, "%d", wetGain);
    char *argss[] = {reverberanceChar, hfDampingChar, roomScaleChar,
                     stereoDepthChar, preDelayChar, wetGainChar};
    assert(sox_effect_options(reverbEffect, 0, argss) == SOX_SUCCESS);

    assert(sox_add_effect(chain, reverbEffect, &in->signal, &in->signal) == SOX_SUCCESS);
    free(reverbEffect);

    e = sox_create_effect(sox_find_effect("output"));
    args[0] = (char *) out;
    assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    sox_flow_effects(chain, NULL, NULL);

    sox_delete_effects_chain(chain);
    sox_close(out);
    sox_close(in);

    sox_quit();
    return tmpBufferSize;
}

int AudioEffect::ReverbBuffer(char *inBuffer, int inSize, char *outBuffer) {

    sox_format_t * in, * out; /* input and output files */
    sox_effects_chain_t * chain;

    int ret;
    ret = sox_init();
    LOGE("start");
    if (ret != SOX_SUCCESS){
        LOGE("sox_init failed");
        return 0;
    }
    sox_encodinginfo_t in_enc;
    sox_encodinginfo_t out_enc;
    sox_signalinfo_t in_sig;
    sox_signalinfo_t out_sig;

    in_enc.bits_per_sample = 16;
    in_enc.encoding = SOX_ENCODING_SIGN2;
    in_enc.compression = 0.f;
    in_enc.opposite_endian = sox_false;
    in_enc.reverse_bits = sox_option_no;
    in_enc.reverse_bytes = sox_option_no;
    in_enc.reverse_nibbles = sox_option_no;

    out_enc.bits_per_sample = 16;
    out_enc.encoding = SOX_ENCODING_SIGN2;
    out_enc.compression = 0.f;
    out_enc.opposite_endian = sox_false;
    out_enc.reverse_bits = sox_option_no;
    out_enc.reverse_bytes = sox_option_no;
    out_enc.reverse_nibbles = sox_option_no;

    in_sig.rate = 44100.f;
    in_sig.channels = 2;
    in_sig.length = 0;
    in_sig.precision = 16;
    in_sig.mult = NULL;

    out_sig.rate = 44100.f;
    out_sig.channels = 2;
    out_sig.length = 0;
    out_sig.precision = 16;
    out_sig.mult = NULL;


    LOGE("ReverbBuffer xxxxxxxxxxxxxxxx in=%08X %d",inBuffer,inSize);

    assert(in = sox_open_mem_read(inBuffer, inSize,  &in_sig, &in_enc, "s16"));
    LOGE("ReverbBuffer in=%08X",in);

    size_t tmpBufferSize;
    assert(out = sox_open_memstream_write(&outBuffer, &tmpBufferSize, &in->signal, NULL, "sox",NULL));

    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    /* Create an effects chain; some effects need to know about the input
   * or output file encoding so we provide that information here */
    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    sox_effect_t *e;
    /* The first effect in the effect chain must be something that can source
     * samples; in this case, we use the built-in handler that inputs
     * data from an audio file */
    e = sox_create_effect(sox_find_effect("input"));
    char *args[10];
    args[0] = (char *) in;
    assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    /* This becomes the first `effect' in the chain */
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    reverbEffect = sox_create_effect(sox_find_effect("reverb"));
    char *wetOnly = "-w";
    char reverberanceChar[5];
    sprintf(reverberanceChar, "%d", 50);
    char hfDampingChar[5];
    sprintf(hfDampingChar, "%d", 50);
    char roomScaleChar[5];
    sprintf(roomScaleChar, "%d", 90);
    char stereoDepthChar[5];
    sprintf(stereoDepthChar, "%d", 50);
    char preDelayChar[5];
    sprintf(preDelayChar, "%d", 30);
    char wetGainChar[5];
    sprintf(wetGainChar, "%d", 0);
    char *argss[] = {reverberanceChar, hfDampingChar, roomScaleChar,
                     stereoDepthChar, preDelayChar, wetGainChar};
    assert(sox_effect_options(reverbEffect, 0, argss) == SOX_SUCCESS);

    assert(sox_add_effect(chain, reverbEffect, &in->signal, &in->signal) == SOX_SUCCESS);
    free(reverbEffect);

    e = sox_create_effect(sox_find_effect("output"));
    args[0] = (char *) out;
    assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    sox_flow_effects(chain, NULL, NULL);

    sox_delete_effects_chain(chain);
    sox_close(out);
    sox_close(in);

    sox_quit();
    return tmpBufferSize;
}