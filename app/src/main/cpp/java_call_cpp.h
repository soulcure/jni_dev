#ifndef JAVA_TO_CPP_H
#define JAVA_TO_CPP_H

#include <jni.h>

extern "C"
{

JNIEXPORT jstring JNICALL Java_com_coocaa_socket_UtilJni_stringFromJNI(JNIEnv *env, jobject);

JNIEXPORT void JNICALL Java_com_coocaa_socket_UtilJni_tcpFromJNI(JNIEnv *env, jobject);


}
#endif //JAVA_TO_CPP_H