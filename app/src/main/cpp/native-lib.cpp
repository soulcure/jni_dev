#include <jni.h>
#include <string>
#include <iostream>
#include <sys/socket.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_coocaa_socket_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}