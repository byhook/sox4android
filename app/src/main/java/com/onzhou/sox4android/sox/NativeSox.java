package com.onzhou.sox4android.sox;

/**
 * @anchor: andy
 * @date: 18-12-11
 */

public class NativeSox {

    static {
        System.loadLibrary("native-sox");
    }

    public native void reverbFile(String inputPath, String outputPath, int reverbrance, int damping, int roomScale, int stereoDepth, int predelay);

    public native byte[] reverbBuffer(byte[] buffer, int length, byte[] outBuffer);

}
