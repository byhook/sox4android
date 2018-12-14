package com.onzhou.sox4android.audio;

/**
 * @anchor: andy
 * @date: 2018-12-13
 * @description:
 */
public interface IAudioMixer {

    /**
     * 初始化混响器
     */
    void initMixer();

    void setAudioBuffer(byte[] buffer);

    void setOnAudioMixListener(OnAudioMixListener onAudioMixListener);

}
