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

    sdk_manager::Get().create_tcp_client(p_ip, port);

    env->ReleaseStringUTFChars(ip, p_ip);
}

JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_sendMessage(JNIEnv *env, jclass clazz, jstring message) {
    const char *p_message = env->GetStringUTFChars(message, 0);

    LOGD("UtilJni_sendMessage:%s", p_message);
    sdk_manager::Get().send_message(p_message);

    env->ReleaseStringUTFChars(message, p_message);
}

}