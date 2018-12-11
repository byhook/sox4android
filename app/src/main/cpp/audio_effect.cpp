

#include "audio_effect.h"


#ifdef NDEBUG /* N.B. assert used with active statements so enable always. */
#undef NDEBUG /* Must undef above assert.h or other that might include it. */
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


int AudioEffect::Reverb(const char *inputFile, const char *outputFile) {
    static sox_format_t *in, *out;
    sox_effects_chain_t *chain;
    sox_effect_t *e;

    char *args[10];

    assert(sox_init() == SOX_SUCCESS);

    //打开输入文件
    assert(in = sox_open_read(inputFile, NULL, NULL, NULL));

    //指定输出文件
    assert(out = sox_open_write(outputFile, &in->signal, NULL, NULL, NULL, NULL));

    //构造一个效果器链
    chain = sox_create_effects_chain(&in->encoding, &out->encoding);

    e = sox_create_effect(sox_find_effect("input"));
    args[0] = (char *) in, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    /* This becomes the first `effect' in the chain */
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    //混响器
    e = sox_create_effect(sox_find_effect("reverb"));
    char *wetOnly = "-w";
    //混响大小
    char *reverbrance = "50";
    //高频阻尼
    char *hfDamping = "50";
    //房间大小
    char *roomScale = "90";
    //立体声深度
    char *stereoDepth = "50";
    //早反射声的时间
    char *preDelay = "30";

    char *wetGain = "0";

    char *params[] = {wetOnly, reverbrance, hfDamping, roomScale, stereoDepth, preDelay, wetGain};

    assert(sox_effect_options(e, 7, params) == SOX_SUCCESS);

    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    e = sox_create_effect(sox_find_effect("output"));
    args[0] = (char *) out, assert(sox_effect_options(e, 1, args) == SOX_SUCCESS);
    assert(sox_add_effect(chain, e, &in->signal, &in->signal) == SOX_SUCCESS);
    free(e);

    //开始执行处理
    sox_flow_effects(chain, NULL, NULL);

    //释放资源
    sox_delete_effects_chain(chain);
    sox_close(out);
    sox_close(in);
    sox_quit();
    return 0;
}