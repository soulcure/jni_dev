package com.coocaa.socket;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private EditText et_ip;

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("tcp-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        et_ip = findViewById(R.id.et_ip);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);

        tv.setText(UtilJni.stringFromJNI());

        findViewById(R.id.btn_connect).setOnClickListener(this);
        findViewById(R.id.btn_send).setOnClickListener(this);
    }


    @Override
    public void onClick(View v) {
        int id = v.getId();
        switch (id) {
            case R.id.btn_connect:
                String ip = et_ip.getText().toString();
                int port = 34000;
                UtilJni.tcpFromJNI(ip, port);
                break;
            case R.id.btn_send:
                String message = "hello world";

                UtilJni.sendMessage(message);
                break;
        }
    }
}