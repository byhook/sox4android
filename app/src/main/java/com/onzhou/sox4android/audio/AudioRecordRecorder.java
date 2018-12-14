package com.onzhou.sox4android.audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.text.TextUtils;

import com.onzhou.sox4android.sox.NativeSox;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.Closeable;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * @anchor: andy
 * @date: 18-12-1
 */
public class AudioRecordRecorder implements IAudioRecorder, OnEncodeListener {

    private static final String TAG = "AudioRecordRecorder";

    private int AUDIO_SOURCE = MediaRecorder.AudioSource.MIC;

    /**
     * 音频采样率
     */
    public static int SAMPLE_RATE = 44100;

    /**
     * 单声道
     */
    public final static int CHANNEL = AudioFormat.CHANNEL_IN_STEREO;

    /**
     * 16比特
     */
    public final static int AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;

    /**
     * 音频录制实例
     */
    protected AudioRecord audioRecord;

    /**
     * 录制线程
     */
    private Thread recordThread;

    /**
     * 输出的文件路径
     */
    private String pcmPath;

    /**
     * 缓冲区大小
     */
    private int bufferSize = 0;

    /**
     * 是否正在录制
     */
    private boolean isRecording = false;

    private WavEncoder mWavEncoder;

    private FileOutputStream outputStream = null;

    private NativeSox mNativeSox;

    private String tempPath, outPath;
    private int mChannels = 2;
    private int mSampleRate = 44100;
    private long byteRate;

    public AudioRecordRecorder(String filePath, String tempPath) {
        this.tempPath = tempPath;
        this.mNativeSox = new NativeSox();
        this.pcmPath = filePath;

        byteRate = mSampleRate * mChannels * 16 / 8;
        mWavEncoder = new WavEncoder();
    }

    @Override
    public void initRecorder() {
        if (null != audioRecord) {
            audioRecord.release();
        }
        try {
            //bufferSize = AudioRecord.getMinBufferSize(SAMPLE_RATE, CHANNEL, AUDIO_FORMAT);
            bufferSize = 4096;
            audioRecord = new AudioRecord(AUDIO_SOURCE, SAMPLE_RATE, CHANNEL, AUDIO_FORMAT, bufferSize);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public int recordStart() {
        if (isRecording) {
            return RECORD_STATE.STATE_RECORDING;
        } else if (audioRecord != null && audioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
            try {
                audioRecord.startRecording();
                isRecording = true;
                recordThread = new Thread(new AudioRecordRunnable());
                recordThread.start();
                return RECORD_STATE.STATE_SUCCESS;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return RECORD_STATE.STATE_ERROR;
    }

    /**
     * 停止音频录制
     */
    @Override
    public void recordStop() {
        try {
            if (audioRecord != null) {
                isRecording = false;
                try {
                    if (recordThread != null) {
                        recordThread.join();
                        recordThread = null;
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                //释放资源
                recordRelease();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void recordRelease() {
        if (audioRecord != null) {
            if (audioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
                audioRecord.stop();
            }
            audioRecord.release();
            audioRecord = null;
        }
    }

    int length = 0;
    ByteArrayOutputStream baos;

    public void onEncodeAACBuffer(byte[] data) {
        try {

            if (baos == null) {
                baos = new ByteArrayOutputStream();
            }
            //积累到32k数据量再转换
            baos.write(data, 0, data.length);
            baos.flush();
            length += data.length;
            if (length >= 528 * 1024) {
                length = 0;


                FileOutputStream segmentFos = new FileOutputStream(new File(tempPath));

                long totalAudioLen = 0;
                long totalDataLen = totalAudioLen + 36;
                long longSampleRate = 44100;
                int channels = mChannels;
                long byteRate = 16 * longSampleRate * channels / 8;
                //写wav头部
                byte[] header = mWavEncoder.writeWavHeader(totalAudioLen, totalDataLen, longSampleRate, channels, byteRate);
                segmentFos.write(header, 0, 44);


                baos.close();
                baos = null;

                //转换一次
                mNativeSox.reverbFile(tempPath, outPath + System.currentTimeMillis() + ".wav",
                        50, 50, 90, 50,
                        30);
            }


        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    private class AudioRecordRunnable implements Runnable {

        @Override
        public void run() {
            try {
                if (!TextUtils.isEmpty(pcmPath)) {
                    outputStream = new FileOutputStream(pcmPath);
                }
                byte[] audioBuffer = new byte[bufferSize];
                while (isRecording && audioRecord != null) {
                    int audioSampleSize = audioRecord.read(audioBuffer, 0, bufferSize);
                    if (audioSampleSize > 0) {

                        onEncodeAACBuffer(audioBuffer);
                    }
                }
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } finally {
                close(outputStream);
                outputStream = null;
            }
        }
    }

    private void close(Closeable closeable) {
        if (closeable != null) {
            try {
                closeable.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    public interface OnAudioRecordListener {

        void onAudioBuffer(byte[] buffer, int length);

    }

}
