package com.coocaa.socket;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.UUID;

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
        findViewById(R.id.btn_string).setOnClickListener(this);
        findViewById(R.id.btn_bytes).setOnClickListener(this);

        AssetManager am = getResources().getAssets();
        UtilJni.nativeSetContext(this,am);
    }


    @Override
    public void onClick(View v) {
        int id = v.getId();
        switch (id) {
            case R.id.btn_connect: {
                String ip = et_ip.getText().toString();
                int port = 34000;
                UtilJni.tcpFromJNI(ip, port);
            }
            break;
            case R.id.btn_string: {
                String uuid = "027a7b52899142f5a9839fb1dcbfeb88";

                try {
                    JSONObject jsonObject = new JSONObject();
                    jsonObject.put("proto", "login");
                    jsonObject.put("sid", uuid);
                    UtilJni.sendString(jsonObject.toString());
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }

            break;
            case R.id.btn_bytes: {
                String v_up = "{\n" +
                        "\t\"id\": \"075a10c7-7afc-49d4-95e8-946dc4cbd0cc\",\n" +
                        "\t\"source\": \"{\\\"id\\\":\\\"027a7b52899142f5a9839fb1dcbfeb88\\\",\\\"extra\\\":{\\\"im-local\\\":\\\"172.20.144.104:34000\\\",\\\"password\\\":\\\"\\\",\\\"stream-local\\\":\\\"172.20.144.104\\\",\\\"address-local\\\":\\\"172.20.144.104\\\",\\\"ssid\\\":\\\"\\\",\\\"im-cloud\\\":\\\"547c08a5f84b411fa945ca8f73c426f6\\\"}}\",\n" +
                        "\t\"target\": \"{\\\"id\\\":\\\"3b3bfb078704409b8e81abb9c741b686\\\",\\\"extra\\\":{\\\"im-local\\\":\\\"172.20.144.108:34000\\\",\\\"spaceId\\\":\\\"aa235bf3521844209558d4f998bf65b0\\\",\\\"password\\\":\\\"66666666\\\",\\\"connectStatus\\\":\\\"1\\\",\\\"stream-local\\\":\\\"172.20.144.108\\\",\\\"address-local\\\":\\\"172.20.144.108\\\",\\\"ssid\\\":\\\"Coocaa-AIOT\\\",\\\"im-cloud\\\":\\\"3b3bfb078704409b8e81abb9c741b686\\\"}}\",\n" +
                        "\t\"client-source\": \"ss-clientID-SmartScreen\",\n" +
                        "\t\"client-target\": \"ss-clientID-appstore_12345\",\n" +
                        "\t\"type\": \"TEXT\",\n" +
                        "\t\"content\": \"{\\\"cmd\\\":\\\"24\\\",\\\"param\\\":\\\"\\\",\\\"type\\\":\\\"KEY_EVENT\\\"}\",\n" +
                        "\t\"extra\": {},\n" +
                        "\t\"reply\": false\n" +
                        "}";
                UtilJni.sendBytes(v_up.getBytes());
                break;
            }
        }
    }


    public static String getUUID() {
        UUID uuid = UUID.randomUUID();
        String str = uuid.toString();
        String uuidStr = str.replace("-", "");

        Log.d("uuid", "uuid=" + uuidStr);
        return uuidStr;
    }
}