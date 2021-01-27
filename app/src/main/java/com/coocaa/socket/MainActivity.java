package com.coocaa.socket;

import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("tcp-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(UtilJni.stringFromJNI());

        Button btn_test = findViewById(R.id.btn_test);
        btn_test.setOnClickListener(v -> {
            UtilJni.tcpFromJNI();
        });
    }

}