#include "java_call_cpp.h"
#include <jni.h>
#include <string>
#include <iostream>
#include "net/tcp/client/tcp_client.h"
#include "log_util.h"

extern "C"
{
JNIEXPORT jstring JNICALL Java_com_coocaa_socket_UtilJni_stringFromJNI(JNIEnv *env, jclass clazz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_tcpFromJNI(JNIEnv *env, jclass clazz, jstring ip, jint port) {
    const char *p_ip = env->GetStringUTFChars(ip, 0);

    auto callback = [](int code, std::string &message) {
        LOGD("tcp client connect result code:%d message:%s", code, message.c_str());
    };
    auto receive = [](PDUBase &pdu) {
        LOGD("tcp client receive pdu");
    };

    LOGD("TcpClient to [%s]:[%d]...", p_ip, port);
    auto *tcpClient = new TcpClient(p_ip, port, callback, receive);
    tcpClient->Open();
    sdk_manager::Instance()->set_tcp_client(tcpClient);
    env->ReleaseStringUTFChars(ip, p_ip);
}


}