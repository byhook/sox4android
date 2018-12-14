package com.onzhou.sox4android.main;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.AsyncTask;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import com.onzhou.sox4android.R;
import com.onzhou.sox4android.audio.AudioRecordRecorder;
import com.onzhou.sox4android.audio.AudioTrackManager;
import com.onzhou.sox4android.audio.IAudioRecorder;
import com.onzhou.sox4android.audio.WavEncoder;
import com.onzhou.sox4android.sox.NativeSox;
import com.onzhou.sox4android.task.AssertReleaseTask;

import java.io.File;

public class MainActivity extends AppCompatActivity implements AssertReleaseTask.ReleaseCallback {

    private static final int PERMISSION_CODE = 100;

    private TextView mBtnReverb, mBtnRecordAudio;

    private IAudioRecorder mAudioRecorder;

    private SeekBar mSeekReverbrance, mSeekHfDamp, mSeekRoomScale, mSeekStereoDepth, mSeekPreDelay;

    private boolean startRecord;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupButton();
        applyPermissions();
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "input.pcm", this);
        videoReleaseTask.execute();
    }

    private void applyPermissions() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO}, PERMISSION_CODE);
        } else {
            setupView();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == PERMISSION_CODE && grantResults != null && grantResults.length > 0) {
            if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                setupView();
            }
        }
    }

    private void setupButton() {
        mBtnReverb = findViewById(R.id.btn_reverb);
        mBtnRecordAudio = findViewById(R.id.btn_record_audio);
    }

    private void setupView() {
        mSeekReverbrance = findViewById(R.id.seek_bar_reverbrance);
        mSeekHfDamp = findViewById(R.id.seek_bar_hfDamping);
        mSeekRoomScale = findViewById(R.id.seek_bar_roomScale);
        mSeekStereoDepth = findViewById(R.id.seek_bar_stereoDepth);
        mSeekPreDelay = findViewById(R.id.seek_bar_preDelay);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mAudioRecorder != null) {
            mAudioRecorder.recordStop();
            mAudioRecorder = null;
        }
    }

    public void onReverbClick(View view) {
        /*File file = new File(getExternalFilesDir(null), "input.wav");
        AudioTrackManager.getInstance().startPlay(file.getAbsolutePath());*/

        AudioParam audioParam = new AudioParam();
        audioParam.reverbrance = mSeekReverbrance.getProgress();
        audioParam.hfDamping = mSeekHfDamp.getProgress();
        audioParam.roomScale = mSeekRoomScale.getProgress();
        audioParam.stereoDepth = mSeekStereoDepth.getProgress();
        audioParam.preDelay = mSeekPreDelay.getProgress();

        //mAudioRecorder.recordStart();

        ReverbTask reverbTask = new ReverbTask(audioParam);
        reverbTask.execute();
    }

    public void onRecordStart(View view) {
        if (!startRecord) {
            startRecord = true;
            if (mAudioRecorder == null) {
                File outputFile = new File(getExternalFilesDir(null), "output.pcm");
                File tempFile = new File(getExternalFilesDir(null), "/temp.wav");
                mAudioRecorder = new AudioRecordRecorder(outputFile.getAbsolutePath(), tempFile.getAbsolutePath());
                mAudioRecorder.initRecorder();
            }
            mAudioRecorder.recordStart();
            mBtnRecordAudio.setText("停止录制");
            Toast.makeText(this, "开始录制", Toast.LENGTH_SHORT).show();
        } else {
            startRecord = false;
            if (mAudioRecorder != null) {
                mAudioRecorder.recordStop();
            }
            mBtnRecordAudio.setText("开始录制");
            Toast.makeText(this, "停止录制", Toast.LENGTH_SHORT).show();
        }

    }

    @Override
    public void onReleaseSuccess(String filePath) {
        mBtnReverb.setEnabled(true);
    }

    public void onMixAudioClick(View view) {
        File file = new File(getExternalFilesDir(null), "output.pcm");
        AudioTrackManager.getInstance().startPlay(file.getAbsolutePath());
    }

    class ReverbTask extends AsyncTask<Void, Void, Void> {

        private AudioParam audioParam;

        ReverbTask(AudioParam audioParam) {
            this.audioParam = audioParam;
        }

        @Override
        protected void onPreExecute() {
            super.onPreExecute();
            mBtnReverb.setEnabled(false);
        }

        @Override
        protected Void doInBackground(Void... voids) {
            NativeSox nativeSox = new NativeSox();
            File inputFile = new File(getExternalFilesDir(null), "input.pcm");
            File outputFile = new File(getExternalFilesDir(null), "output.pcm");

            nativeSox.reverbFile(inputFile.getAbsolutePath(), outputFile.getAbsolutePath(), audioParam.reverbrance, audioParam.hfDamping, audioParam.roomScale, audioParam.stereoDepth, audioParam.preDelay);
            return null;
        }

        @Override
        protected void onPostExecute(Void aVoid) {
            super.onPostExecute(aVoid);
            mBtnReverb.setEnabled(true);
            Toast.makeText(MainActivity.this, "转换成功!", Toast.LENGTH_SHORT).show();
        }
    }

    protected class AudioParam {

        public int reverbrance;
        public int hfDamping;
        public int roomScale;
        public int stereoDepth;
        public int preDelay;

    }

}
