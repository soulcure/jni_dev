#include <jni.h>
#include <string>
#include "net/tcp/client/tcp_client.h"
#include "log_util.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_coocaa_socket_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

//    TcpClient tcpClient=new TcpClient();
//    auto callback = [](int code, std::string message) {
//        LOGD("tcp client connect result code:%d message=%s", code, message);
//    };
//    tcpClient.Connect("172.20.144.108", 34000, callback);

    return env->NewStringUTF(hello.c_str());
}