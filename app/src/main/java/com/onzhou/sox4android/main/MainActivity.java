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
import com.onzhou.sox4android.audio.AudioReverbRecorder;
import com.onzhou.sox4android.audio.AudioTrackManager;
import com.onzhou.sox4android.audio.IAudioRecorder;
import com.onzhou.sox4android.audio.ReverbParam;
import com.onzhou.sox4android.sox.NativeSox;
import com.onzhou.sox4android.task.AssertReleaseTask;

import java.io.File;

public class MainActivity extends AppCompatActivity implements AssertReleaseTask.ReleaseCallback, SeekBar.OnSeekBarChangeListener {

    private static final int PERMISSION_CODE = 100;

    private TextView mBtnReverb, mBtnRecordAudio, mBtnPlay;

    private IAudioRecorder mAudioRecorder;

    private TextView mTvBrance, mTvDamping, mTvRoomscale, mTvDepth, mTvPredelay;

    private SeekBar mSeekReverbrance, mSeekHfDamp, mSeekRoomScale, mSeekStereoDepth, mSeekPreDelay;

    private boolean startRecord;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupButton();
        applyPermissions();
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "song.wav", this);
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
        mBtnPlay = findViewById(R.id.btn_record_play);

        mTvBrance = findViewById(R.id.reverb_tv_brance);
        mTvDamping = findViewById(R.id.reverb_tv_hfDamping);
        mTvRoomscale = findViewById(R.id.reverb_tv_roomScale);
        mTvDepth = findViewById(R.id.reverb_tv_depth);
        mTvPredelay = findViewById(R.id.reverb_tv_predelay);

        mSeekReverbrance = findViewById(R.id.seek_bar_reverbrance);
        mSeekHfDamp = findViewById(R.id.seek_bar_hfDamping);
        mSeekRoomScale = findViewById(R.id.seek_bar_roomScale);
        mSeekStereoDepth = findViewById(R.id.seek_bar_stereoDepth);
        mSeekPreDelay = findViewById(R.id.seek_bar_preDelay);

        mSeekReverbrance.setOnSeekBarChangeListener(this);
        mSeekHfDamp.setOnSeekBarChangeListener(this);
        mSeekRoomScale.setOnSeekBarChangeListener(this);
        mSeekStereoDepth.setOnSeekBarChangeListener(this);
        mSeekPreDelay.setOnSeekBarChangeListener(this);
    }


    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
        if (seekBar == mSeekReverbrance) {
            mTvBrance.setText(String.format("混响大小 %d", progress));
        } else if (seekBar == mSeekHfDamp) {
            mTvDamping.setText(String.format("高频阻尼 %d", progress));
        } else if (seekBar == mSeekRoomScale) {
            mTvRoomscale.setText(String.format("房间大小 %d", progress));
        } else if (seekBar == mSeekStereoDepth) {
            mTvDepth.setText(String.format("立体声深度 %d", progress));
        } else if (seekBar == mSeekPreDelay) {
            mTvPredelay.setText(String.format("早反射声的时间 %d", progress));
        }
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {

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

        ReverbParam audioParam = new ReverbParam();
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
            if (mBtnPlay != null) {
                mBtnPlay.setEnabled(false);
            }
            startRecord = true;
            if (mAudioRecorder == null) {
                File outputFile = new File(getExternalFilesDir(null), "result.pcm");
                mAudioRecorder = new AudioReverbRecorder(outputFile.getAbsolutePath(), getExternalFilesDir(null).getAbsolutePath());
            }

            ReverbParam audioParam = new ReverbParam();
            audioParam.reverbrance = mSeekReverbrance.getProgress();
            audioParam.hfDamping = mSeekHfDamp.getProgress();
            audioParam.roomScale = mSeekRoomScale.getProgress();
            audioParam.stereoDepth = mSeekStereoDepth.getProgress();
            audioParam.preDelay = mSeekPreDelay.getProgress();
            mAudioRecorder.setReverbParam(audioParam);

            mAudioRecorder.recordStart();
            mBtnRecordAudio.setText("停止录制");
            Toast.makeText(this, "开始录制", Toast.LENGTH_SHORT).show();
        } else {
            if (mBtnPlay != null) {
                mBtnPlay.setEnabled(true);
            }
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
        File file = new File(getExternalFilesDir(null), "result.pcm");
        AudioTrackManager.getInstance().startPlay(file.getAbsolutePath());
    }

    class ReverbTask extends AsyncTask<Void, Void, Void> {

        private ReverbParam audioParam;

        ReverbTask(ReverbParam audioParam) {
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
            File inputFile = new File(getExternalFilesDir(null), "input.wav");
            File outputFile = new File(getExternalFilesDir(null), "output.wav");

            nativeSox.setReverbParam(audioParam.reverbrance, audioParam.hfDamping, audioParam.roomScale, audioParam.stereoDepth, audioParam.preDelay, 0);
            nativeSox.reverbWavFile(inputFile.getAbsolutePath(), outputFile.getAbsolutePath());

            return null;
        }

        @Override
        protected void onPostExecute(Void aVoid) {
            super.onPostExecute(aVoid);
            mBtnReverb.setEnabled(true);
            Toast.makeText(MainActivity.this, "转换成功!", Toast.LENGTH_SHORT).show();
        }
    }


}
