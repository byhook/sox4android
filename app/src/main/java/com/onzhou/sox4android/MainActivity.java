package com.onzhou.sox4android;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.onzhou.sox4android.sox.NativeSox;
import com.onzhou.sox4android.task.AssertReleaseTask;

import java.io.File;

public class MainActivity extends AppCompatActivity implements AssertReleaseTask.ReleaseCallback {

    private TextView mBtnReverb;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        setupView();
        AssertReleaseTask videoReleaseTask = new AssertReleaseTask(this, "song.wav", this);
        videoReleaseTask.execute();
    }

    private void setupView() {
        mBtnReverb = findViewById(R.id.btn_reverb);
    }

    public void onReverbClick(View view) {
        mBtnReverb.setEnabled(false);
        ReverbTask reverbTask = new ReverbTask();
        reverbTask.execute();
    }

    @Override
    public void onReleaseSuccess(String filePath) {
        mBtnReverb.setEnabled(true);
    }

    class ReverbTask extends AsyncTask<Void, Void, Void> {

        @Override
        protected Void doInBackground(Void... voids) {
            NativeSox nativeSox = new NativeSox();
            File inputFile = new File(getExternalFilesDir(null), "song.wav");
            File outputFile = new File(getExternalFilesDir(null), "output.wav");
            nativeSox.reverb(inputFile.getAbsolutePath(), outputFile.getAbsolutePath());
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
