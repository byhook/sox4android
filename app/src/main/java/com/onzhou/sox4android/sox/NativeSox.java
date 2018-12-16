package com.onzhou.sox4android.sox;

/**
 * @anchor: andy
 * @date: 18-12-11
 */

public class NativeSox {

    static {
        System.loadLibrary("native-sox");
    }

    public native void init();

    public native void release();

    /**
     * 设置混响的参数
     * @param reverbrance
     * @param damping
     * @param roomScale
     * @param stereoDepth
     * @param predelay
     * @param wetGain
     */
    public native void setReverbParam(int reverbrance, int damping, int roomScale, int stereoDepth, int predelay, int wetGain);

    /**
     * 混响文件
     * @param inputPath
     * @param outputPath
     */
    public native void reverbWavFile(String inputPath, String outputPath);

    /**
     * 混响
     * @param buffer
     * @param length
     * @param outBuffer
     * @return
     */
    public native byte[] reverbPcmBuffer(byte[] buffer, int length, byte[] outBuffer);

}
