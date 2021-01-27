package com.coocaa.socket;

public class UtilJni {
    //***************以下为java调用 C++接口******************//
    public static native String stringFromJNI();

    public static native void tcpFromJNI();


    //*************以下为 C++调用java接口******************//
    public static void sendNetData(byte[] buf) {
        //
    }


}
