#include <jni.h>
#include <string>
#include <iostream>
#include "net/tcp/client/tcp_client.h"
#include "log_util.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_coocaa_socket_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_coocaa_socket_MainActivity_tcpFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string ip = "172.20.144.115";
    int port = 34000;
    auto callback = [](int code, std::string &message) {
        LOGD("tcp client connect result code:%d message:%s", code, message.c_str());
    };
    auto receive = [](PDUBase &pdu) {
        LOGD("tcp client receive pdu");
    };

    LOGD("TcpClient to [%s]:[%d]...", ip.c_str(), port);
    auto *tcpClient = new TcpClient(ip, port, callback, receive);
    tcpClient->Open();

    //TcpClient tcpClient(ip, port, callback, receive);
    //tcpClient.Open();
}