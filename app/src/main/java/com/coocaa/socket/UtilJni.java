package com.coocaa.socket;

public class UtilJni {
    //***************以下为java调用 C++接口******************//
    public static native String stringFromJNI();

    public static native void tcpFromJNI(String ip, int port);

    public static native void sendString(String message);

    public static native void sendBytes(byte[] bytes);


    //*************以下为 C++调用java接口******************//
    public static void sendNetData(byte[] buf) {
        //
    }


}
