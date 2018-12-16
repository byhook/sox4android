package com.onzhou.sox4android.audio;

/**
 * @anchor: andy
 * @date: 18-12-1
 */

public interface IAudioRecorder {


    interface RECORD_STATE {
        int STATE_RECORDING = 0;
        int STATE_SUCCESS = 1;
        int STATE_ERROR = 2;
    }

    /**
     * 初始化
     */
    void initRecorder();

    /**
     * 设置混响参数
     * @param reverbParam
     */
    void setReverbParam(ReverbParam reverbParam);

    /**
     * 开始录制音频
     */
    int recordStart();

    /**
     * 停止录制音频
     */
    void recordStop();

}
