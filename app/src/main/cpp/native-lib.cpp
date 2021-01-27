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

    std::string ip = "172.20.144.108";
    int port = 34000;
    auto callback = [](int code, std::string &message) {
        LOGD("tcp client connect result code:%d message:%s", code, message.c_str());
    };
    auto receive = [](PDUBase &pdu) {
        LOGD("tcp client receive pdu");
    };
    auto *tcpClient = new TcpClient(ip.c_str(), port, callback, receive);
    tcpClient->Open();

    return env->NewStringUTF(hello.c_str());
}