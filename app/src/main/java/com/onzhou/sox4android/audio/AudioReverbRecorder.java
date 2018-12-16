package com.onzhou.sox4android.audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.text.TextUtils;

import com.onzhou.sox4android.sox.NativeSox;

import java.io.BufferedInputStream;
import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

/**
 * @anchor: andy
 * @date: 18-12-1
 */
public class AudioReverbRecorder implements IAudioRecorder, OnEncodeListener {

    private static final String TAG = "AudioReverbRecorder";

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

    private String tempPath;

    private ReverbParam mReverbParam;

    public AudioReverbRecorder(String filePath, String tempPath) {
        this.tempPath = tempPath;
        this.mNativeSox = new NativeSox();
        this.pcmPath = filePath;

        mWavEncoder = new WavEncoder();
    }

    @Override
    public void initRecorder() {
        if (null != audioRecord) {
            audioRecord.release();
        }
        try {
            //bufferSize = AudioRecord.getMinBufferSize(SAMPLE_RATE, CHANNEL, AUDIO_FORMAT);
            bufferSize = 128 * 1024;
            audioRecord = new AudioRecord(AUDIO_SOURCE, SAMPLE_RATE, CHANNEL, AUDIO_FORMAT, bufferSize);
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (mNativeSox != null) {
            mNativeSox.init();
            setReverbParam(mReverbParam);
        }
    }

    @Override
    public void setReverbParam(ReverbParam reverbParam) {
        this.mReverbParam = reverbParam;
        if (mNativeSox != null && mReverbParam != null) {
            mNativeSox.setReverbParam(reverbParam.reverbrance, reverbParam.hfDamping, reverbParam.roomScale, reverbParam.stereoDepth, reverbParam.preDelay, reverbParam.wetGain);
        }
    }

    @Override
    public int recordStart() {
        if (isRecording) {
            return RECORD_STATE.STATE_RECORDING;
        } else {
            //重新初始化
            initRecorder();
            //查看状态
            if (audioRecord != null && audioRecord.getState() == AudioRecord.STATE_INITIALIZED) {
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
        if (mNativeSox != null) {
            mNativeSox.release();
        }
    }

    private FileOutputStream tempStream;

    public void onHandlePCMBuffer(byte[] data) {
        try {

            File tempFile = new File(tempPath, "temp.in.wav");

            tempStream = new FileOutputStream(tempFile);

            // 填入参数，比特率等等。这里用的是16位单声道 44100 hz
            WaveHeader wavHeader = new WaveHeader();
            // 长度字段 = 内容的大小（TOTAL_SIZE) +
            // 头部字段的大小(不包括前面4字节的标识符RIFF以及fileLength本身的4字节)
            wavHeader.fileLength = bufferSize + (44 - 8);
            wavHeader.FmtHdrLeth = 16;
            wavHeader.BitsPerSample = 16;
            wavHeader.Channels = 2;
            wavHeader.FormatTag = 0x0001;
            wavHeader.SamplesPerSec = 44100;
            wavHeader.BlockAlign = (short) (wavHeader.Channels * wavHeader.BitsPerSample / 8);
            wavHeader.AvgBytesPerSec = wavHeader.BlockAlign * wavHeader.SamplesPerSec;
            wavHeader.DataHdrLeth = bufferSize;

            byte[] header = wavHeader.getHeader();

            //写wav头部
            tempStream.write(header, 0, header.length);

            tempStream.write(data);
            tempStream.flush();
            tempStream.close();

            //混响
            File outFile = new File(tempPath, "temp.out.wav");
            mNativeSox.reverbWavFile(tempFile.getAbsolutePath(), outFile.getAbsolutePath());

            //WAV转PCM输出文件
            InputStream inStream = null;
            byte[] buffer = new byte[1024 * 4];
            inStream = new BufferedInputStream(new FileInputStream(outFile));
            int size = inStream.read(buffer);
            if (size != -1) {
                outputStream.write(buffer, 44, size - 44);
                size = inStream.read(buffer);
            }
            while (size != -1) {
                outputStream.write(buffer);
                size = inStream.read(buffer);
            }
            inStream.close();

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

                        onHandlePCMBuffer(audioBuffer);
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
