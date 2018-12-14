#include <android/log.h>
#include <malloc.h>
#include <string.h>
#include "native-lib.h"
#include <assert.h>

#define LOG_I(...) __android_log_print(ANDROID_LOG_ERROR , "NativeEncode", __VA_ARGS__)


#define MAX_SAMPLES (size_t)2048
JNIEXPORT jbyteArray JNICALL setAudioEffectBuffer(JNIEnv *env, jobject instance,jbyteArray bytes_,jint inSize, jbyteArray outData) {
    jbyte *bytes = env->GetByteArrayElements(bytes_, NULL);
    //LOG_I("xxx mem buffer %d %d %d %d %d %d %d %d",bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5],bytes[6],bytes[7]);
    jsize length = env->GetArrayLength(bytes_);
    sox_format_t * in, * out; /* input and output files */
    sox_effects_chain_t * chain;
    sox_effect_t * e;
    int ret;
    ret = sox_init();
    LOG_I("start");

    //jbyte bufferptr[length];
    //memset(bufferptr,0,length);

    char *bufferptr;

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

    char *buffer;
    size_t buffer_size;

    in = sox_open_mem_read(bytes, length, &in_sig, &in_enc, "s16");
    if (!in){
        LOG_I("sox_open_mem_read fail");
        goto end;
    }
    LOG_I("open mem success");
    LOG_I("encoding info=%d,filetype=%s",in->encoding.encoding,in->filetype);
    //out = sox_open_mem_write(bufferptr, length, &in->signal, NULL, "sox", NULL);
    out = sox_open_memstream_write(&bufferptr, &buffer_size, &in_sig, &in_enc, "sox", NULL);
    if (!out){
        LOG_I("sox_open_mem_write fail xxxxx");
        goto end;
    }
    /*size_t number_read;
    sox_sample_t samples[MAX_SAMPLES];
    while ((number_read = sox_read(in, samples, MAX_SAMPLES))){
         LOG_I("mem buffer %08X %d",bufferptr,number_read);
         assert(sox_write(out, samples, number_read) == number_read);
    }
    LOG_I("mem buffer %08X %d",bufferptr,number_read);*/

    chain = sox_create_effects_chain(&in->encoding, &out->encoding);
    char *args[10];
    ret = initEffect(in, out,chain, e, ret);
    if (ret!=0){
        LOG_I("init effect error");
    }
    /*ret = echoEffect(in,out, chain, e, ret);
    if (ret!=0){
         LOG_I("echo effect error");
     }*/
     ret = reverbEffect(in,out, chain, e, ret);
     if (ret!=0){
         LOG_I("reverb effect error");
     }

    ret = releaseEffect(in, out, chain, e, ret);
    if (ret!=0){
        LOG_I("mem release effect error");
    }
    //让整个效果器链运行起来
    ret = sox_flow_effects(chain, NULL, NULL);
     if (ret!=0){
         LOG_I("mem sox_flow_effects error");
     }
    //sox_flow_effects执行完毕整个流程也就结束了，销毁资源
    //memset(resultbytes,0,length);
    //memcpy(bufferptr,bytes, length);

    LOG_I("mem copy complete %d",length);
    sox_delete_effects_chain(chain);
    sox_close(out);
    sox_close(in);
    sox_quit();//必须和sox_init配对出现，不然再次执行sox_init会崩溃

    end:

    LOG_I("mem buffer %d %d %d %d      buffer_size=%d",bufferptr[0],bufferptr[1],bufferptr[2],bufferptr[3],buffer_size);
    LOG_I("mem buffer %d %d %d %d",bytes[0],bytes[1],bytes[2],bytes[3]);
    env->ReleaseByteArrayElements(bytes_, bytes, 0);
    jbyteArray jshortArray1 = env->NewByteArray(length);
    env->SetByteArrayRegion(jshortArray1, 0, length, (jbyte*)bufferptr);
    //delete resultbytes;
    return jshortArray1;
}

JNIEXPORT void JNICALL setAudioEffect(JNIEnv *env, jobject instance,jstring input_,jstring output_) {
    const char *input = env->GetStringUTFChars(input_, 0);
    const char *output = env->GetStringUTFChars(output_, 0);
    sox_format_t * in, * out; /* input and output files */
    sox_effects_chain_t * chain;
    sox_effect_t * e;
    int ret;
    ret = sox_init();
    LOG_I("start");
    if (ret != SOX_SUCCESS){
        LOG_I("sox_init failed");
        return;
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

    in = sox_open_read(input, &in_sig, &in_enc, "s16");
    out = sox_open_write(output, &out_sig, &out_enc, "s16", NULL, NULL);

    chain = sox_create_effects_chain(&in->encoding, &out->encoding);
    char *args[10];
    ret = initEffect(in, out,chain, e, ret);
    if (ret!=0){
        LOG_I("init effect error");
    }
     ret = reverbEffect(in,out, chain, e, ret);
     if (ret!=0){
         LOG_I("reverb effect error");
     }

     ret = echoEffect(in,out, chain, e, ret);
     if (ret!=0){
         LOG_I("echo effect error");
     }
    ret = releaseEffect(in, out, chain, e, ret);
    if (ret!=0){
        LOG_I("release effect error");
    }
    //让整个效果器链运行起来
    sox_flow_effects(chain, NULL, NULL);
    //sox_flow_effects执行完毕整个流程也就结束了，销毁资源
    LOG_I("执行完毕");
    sox_delete_effects_chain(chain);
    sox_close(out);
    sox_close(in);
    sox_quit();//必须和sox_init配对出现，不然再次执行sox_init会崩溃
    env->ReleaseStringUTFChars(input_, input);
    env->ReleaseStringUTFChars(output_, output);
}
/*********************************压缩器************************************/
int compandEffect(sox_format_t *in, sox_format_t *out,sox_effects_chain_t *chain, sox_effect_t *e, int ret) {
    e = sox_create_effect(sox_find_effect("compand"));
    char* attackRelease = "0.3,1.0";
    char* functionTransTable = "6:-90,-90,-70,-55,-31,-21,0,-20";
    char* gain = "0";
    char* initialVolume = "-90";
    char* delay = "0.1";
    char* compandArgs[] = {attackRelease,functionTransTable,gain,initialVolume,delay};
    sox_effect_options(e,5,compandArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, &in->signal, &out->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}

int highpassEffect(sox_format_t *in,sox_format_t *out, sox_effects_chain_t *chain, sox_effect_t *e, int ret) {
    e = sox_create_effect(sox_find_effect("highpass"));
    char* frequency = "80";
    char* width = "0.5q";
    char* highpassArgs[] = {frequency,width};
    ret = sox_effect_options(e, 2, highpassArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, &in->signal, &out->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}
int lowpassEffect(sox_format_t *in,sox_format_t *out, sox_effects_chain_t *chain, sox_effect_t *e, int ret) {
    e = sox_create_effect(sox_find_effect("lowpass"));
    char* frequency = "60";
    char* width = "0.5q";
    char* highpassArgs[] = {frequency,width};
    ret = sox_effect_options(e, 2, highpassArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, &in->signal, &out->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}
/*****************************均衡器****************************************/
int equalizerEffect(sox_format_t *in,sox_format_t *out, sox_effects_chain_t *chain, sox_effect_t *e, int ret) {
    e = sox_create_effect(sox_find_effect("equalizer"));
    char* frequency = "300";
    char* bandWidth = "1.25q";
    char* gain = "3";
    char* equalizerArgs[] ={frequency,bandWidth,gain};
    ret = sox_effect_options(e, 3, equalizerArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, &in->signal, &out->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}

/*****************************最后效果必须是输出***********************************/
int releaseEffect(sox_format_t *in, sox_format_t *out, sox_effects_chain_t *chain,
sox_effect_t *e, int ret) {
    e = sox_create_effect(sox_find_effect("output"));
    char* outArgs[10];
    outArgs[0] = (char *)out;
    ret = sox_effect_options(e, 1, outArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("mem sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, &in->signal, &out->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("mem sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}
/****************************第一个effect必须是input******************************************/
int initEffect(sox_format_t *in,sox_format_t *out, sox_effects_chain_t *chain, sox_effect_t *e, int ret) {
    e = sox_create_effect(sox_find_effect("input"));
    char* inputArgs[10];
    inputArgs[0] = (char *) in;
    ret = sox_effect_options(e, 1, inputArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, &in->signal, &out->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}

/*********************************混响效果*********************************/
int reverbEffect(sox_format_t *in, sox_format_t *out,sox_effects_chain_t *chain, sox_effect_t *e, int ret) {
    e = sox_create_effect(sox_find_effect("reverb"));
    char* wetOnly = "-w";
    char* reverbrance = "50";
    char* hfDamping = "50";
    char* roomScale = "100";
    char* stereoDepth = "100";
    char* preDelay = "100";
    char* wetGain = "0";
    char* reverbArgs[] = {wetOnly,reverbrance,hfDamping,roomScale,stereoDepth,preDelay,wetGain};
    ret = sox_effect_options(e,7,reverbArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, &in->signal, &out->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}
/**************************回声**********************************************/
int echoEffect(sox_format_t *in, sox_format_t *out,sox_effects_chain_t *chain, sox_effect_t *e, int ret) {
    e = sox_create_effect(sox_find_effect("echo"));
    char* arg1 = "0.8";
    char* arg2 = "0.9";
    char* arg3 = "1000";
    char* arg4 = "0.3";
    char* arg5 = "1800";
    char* arg6 = "0.25";
    char* echoArgs[] = {arg1,arg2,arg3,arg4,arg5,arg6};
    ret = sox_effect_options(e, 6, echoArgs);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_effect_options error");
        return -1;
    }
    ret = sox_add_effect(chain, e, &in->signal, &out->signal);
    if (ret!=SOX_SUCCESS){
        LOG_I("sox_add_effect error");
        return -1;
    }
    free(e);
    return 0;
}

