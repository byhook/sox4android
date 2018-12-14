//
// Created by Administrator on 2018/4/30 0030.
//
#ifdef NDEBUG /* N.B. assert used with active statements so enable always. */
#undef NDEBUG /* Must undef above assert.h or other that might include it. */
#endif

#include <jni.h>
#include <android/log.h>
#include <malloc.h>
extern "C"{
#include "include/sox.h"
#include "include/util.h"
}
#define LOG_I(...) __android_log_print(ANDROID_LOG_ERROR , "NativeEncode", __VA_ARGS__)

void
reverbEffect(const sox_format_t *in, const sox_effects_chain_t *chain, sox_effect_t *&e, int &ret);

void rednoiseEffect(sox_format_t *in, sox_effects_chain_t *chain, sox_effect_t *e, int ret);


JNIEXPORT void JNICALL initSox(JNIEnv *env, jobject instance) {
    int ret = sox_init();
    if (ret != SOX_SUCCESS){
        LOG_I("sox_init failed");
        sox_quit();
    }
}

void initInput(sox_format_t *in, sox_effects_chain_t *chain, sox_effect_t *e, int ret,sox_signalinfo_t *signalinfo) {
    e = sox_create_effect(sox_find_effect("input"));
    char* inputArgs[10];
    inputArgs[0] = (char *) in;
    ret = sox_effect_options(e, 1, inputArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
    }
    ret = sox_add_effect(chain, e, signalinfo, signalinfo);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
    }
    free(e);
}

JNIEXPORT void JNICALL closeSox(JNIEnv *env, jobject instance) {
    sox_quit();
}

void initOutput(sox_format_t *in, sox_format_t *out, sox_effects_chain_t *chain,
                sox_effect_t *e, int ret,sox_signalinfo_t *signalinfo) {
    e = sox_create_effect(sox_find_effect("output"));
    char* outArgs[10];
    outArgs[0] = (char *)out;
    ret = sox_effect_options(e, 1, outArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
    }
    ret = sox_add_effect(chain, e, signalinfo, signalinfo);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
    }
    free(e);
}
void reverbEffects(sox_format_t *in, sox_effects_chain_t *chain, sox_effect_t *e, int ret,sox_signalinfo_t *signalinfo) {
    e = sox_create_effect(sox_find_effect("reverb"));
    char* wetOnly = "-w";
    char* reverbrance = "50";
    char* hfDamping = "50";
    char* roomScale = "100";
    char* stereoDepth = "100";
    char* preDelay = "0";
    char* wetGain = "0";
    char* reverbArgs[] = {wetOnly,reverbrance,hfDamping,roomScale,stereoDepth,preDelay,wetGain};
    ret = sox_effect_options(e,7,reverbArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
    }
    ret = sox_add_effect(chain, e, signalinfo, signalinfo);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
    }
    free(e);
}
int echoEffects(sox_format_t *in, sox_effects_chain_t *chain, sox_effect_t *e, int ret,sox_signalinfo_t *signalinfo) {
    e = sox_create_effect(sox_find_effect("echo"));
    char* arg1 = "0.8";
    char* arg2 = "0.9";
    char* arg3 = "40";
    char* arg4 = "0.3";
    char* arg5 = "60";
    char* arg6 = "0.25";
    char* echoArgs[] = {arg1,arg2,arg3,arg4,arg5,arg6};
    ret = sox_effect_options(e, 6, echoArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, signalinfo, signalinfo);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}

int getArray(jsize length, sox_sample_t const *samples,int16_t *output) {
    LOG_I("get array in");
    int16_t *  ibuf;
    int flip = 0;
    SOX_SAMPLE_LOCALS;
    for (int i = 0; i < length; ++i) {
        LOG_I("to signed");
        ibuf[i]=SOX_SAMPLE_TO_SIGNED_16BIT(samples[i],flip);
    }
    memcpy(ibuf,output,length);
    return SOX_SUCCESS;
}
void LSX_API message_handler(
        unsigned level,                       /* 1 = FAIL, 2 = WARN, 3 = INFO,
                                              4 = DEBUG, 5 = DEBUG_MORE,
                                              6 = DEBUG_MOST. */
        LSX_PARAM_IN_Z char const * filename, /* Source code __FILENAME__
from which
                                              message originates. */
        LSX_PARAM_IN_PRINTF char const * fmt, /* Message format string. */
        LSX_PARAM_IN va_list ap               /* Message format parameters. */
) {
    if (level > sox_get_globals()->verbosity) {
        return;
    }
    LOG_I("- Received message (level %u, %s):\n", level, filename);
    LOG_I(fmt, ap);
}

JNIEXPORT jbyteArray JNICALL memorybuffer2(JNIEnv *env, jobject instance,jbyteArray bytes_) {
    jbyte *bytes = env->GetByteArrayElements(bytes_, NULL);
    jsize length = env->GetArrayLength(bytes_);
    LOG_I("length=%d",length);
    int flip = 0;
    static sox_format_t * in, * out;
    sox_effects_chain_t * chain;
    sox_effect_t * e;
    sox_sample_t samples[length]; /* Temporary store whilst copying. */
    size_t number_read;
    int ret;
    jbyte bufferptr[length];
    jbyte *resultbytes = (jbyte *) malloc(length* sizeof(jbyte));
    int16_t *  out16buf;

    sox_signalinfo_t signalinfo = {
            44100,
            2,
            16,
            0,
            NULL
    };
    sox_encodinginfo_t encodinginfo = {
            SOX_ENCODING_SIGN2,
            16,
            0,
            sox_option_no,
            sox_option_no,
            sox_option_no,
            sox_false
    };

    sox_globals_t* sox_globals_p = sox_get_globals ();
    sox_globals_p->bufsiz = 5120;
    sox_globals_p->output_message_handler = message_handler;
    in = sox_open_mem_read(bytes, length, &signalinfo, &encodinginfo, "s16");
    if (!in){
        LOG_I("sox_open_mem_read fail");
        goto end;
    }
    LOG_I("open mem success");
    LOG_I("encoding info=%d,filetype=%s",in->encoding.encoding,in->filetype);
    out = sox_open_mem_write(bufferptr, length, &signalinfo, &encodinginfo, "s16", NULL);
    if (!out){
        LOG_I("sox_open_mem_write fail");
        goto end;
    }
    LOG_I("open write success");
     //添加输入效果
    chain = sox_create_effects_chain(&encodinginfo, &encodinginfo);
    initInput(in, chain, e, ret,&signalinfo);
    //添加其他效果
//    //去环境噪声
//    rednoiseEffect(in, chain, e, ret);
    //混响
    reverbEffects(in, chain, e, ret,&signalinfo);
    //回音
    //echoEffects(in, chain, e, ret,&signalinfo);
    //添加输出
    initOutput(in, out, chain, e, ret,&signalinfo);

    //让整个效果器链运行起来
    ret = sox_flow_effects(chain, NULL, NULL);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_flow_effects fail ret=%d",ret);
    }

    LOG_I("bufferptr size=%d", sizeof(bufferptr));//2048

    LOG_I("copy start");
    memcpy(resultbytes, &bufferptr, length);
    //sox_flow_effects执行完毕整个流程也就结束了，销毁资源
    LOG_I("copy complete");
    sox_delete_effects_chain(chain);
    sox_close(out);
    sox_close(in);
    end:
    env->ReleaseByteArrayElements(bytes_, bytes, 0);
    jbyteArray jshortArray1 = env->NewByteArray(length);
    env->SetByteArrayRegion(jshortArray1, 0, length, resultbytes);
    free(resultbytes);
    return jshortArray1;
}

void rednoiseEffect(sox_format_t *in, sox_effects_chain_t *chain, sox_effect_t *e, int ret) {
//    e = sox_create_effect(sox_find_effect("noiseprof"));
    char *filename = "/storage/emulated/0/Filter/noise.profile";
//    char* profArgs[] = {filename};
//    ret = sox_effect_options(e, 1, profArgs);
//    if (ret!=SOX_SUCCESS){
//        LOG_I("sox_effect_options error");
//    }
//    ret = sox_add_effect(chain, e, &in->signal, &in->signal);
//    if (ret!=SOX_SUCCESS){
//        LOG_I("sox_add_effect error");
//    }
//    free(e);
    e = sox_create_effect(sox_find_effect("noisered"));
    char* factor ="0.6";
    char* noiseredArgs[] = {filename,factor};
    ret = sox_effect_options(e, 2, noiseredArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
    }
    ret = sox_add_effect(chain, e, &in->signal, &in->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
    }
    free(e);
}





