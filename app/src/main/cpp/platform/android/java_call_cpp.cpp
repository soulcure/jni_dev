#include "java_call_cpp.h"
#include <jni.h>
#include <string>
#include <iostream>
#include "../../net/tcp/client/tcp_client.h"
#include "../../log/log_util.h"
#include "jni_helper.h"

extern "C"
{
JNIEXPORT jstring JNICALL Java_com_coocaa_socket_UtilJni_stringFromJNI(JNIEnv *env, jclass clazz) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_tcpFromJNI(JNIEnv *env, jclass clazz, jstring ip, jint port) {
    const char *p_ip = env->GetStringUTFChars(ip, JNI_FALSE);

    sdk_manager::Get().create_tcp_client(p_ip, port);
    /*const char *buf = "hello";
    int len = 5;
    LOGD("ConReceivePdu buffer size:[%d]", len);
    JniHelper::ConReceivePdu(buf, len);*/

    env->ReleaseStringUTFChars(ip, p_ip);
}

JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_sendString(JNIEnv *env, jclass clazz, jstring message) {
    const char *p_message = env->GetStringUTFChars(message, JNI_FALSE);
    int length = strlen(p_message);

    sdk_manager::Get().send_message(length, p_message);

    env->ReleaseStringUTFChars(message, p_message);
}


JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_sendBytes(JNIEnv *env, jclass clazz, jbyteArray byteArray) {
    jbyte *temp = env->GetByteArrayElements(byteArray, JNI_FALSE);
    char *p_message = (char *) temp;
    int length = env->GetArrayLength(byteArray);

    sdk_manager::Get().send_bytes(length, p_message);

    env->ReleaseByteArrayElements(byteArray, temp, JNI_FALSE);

}

}