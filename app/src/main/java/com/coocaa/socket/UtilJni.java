package com.coocaa.socket;

import android.util.Log;

public class UtilJni {
    //***************以下为java调用 C++接口******************//
    public static native String stringFromJNI();

    public static native void tcpFromJNI(String ip, int port);

    public static native void sendString(String message);

    public static native void sendBytes(byte[] bytes);


    //*************以下为 C++调用java接口******************//
    public static void onReceivePdu(byte[] buf) {
        String str = new String(buf);
        Log.d("jni", "onReceivePdu body:" + str);
    }


}
