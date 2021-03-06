﻿#ifndef JAVA_TO_CPP_H
#define JAVA_TO_CPP_H

#include <jni.h>
#include <string>
#include <iostream>

#include "sdk_manager.h"

//第一个参数 JNIEnv
//JNIEnv，Java本地接口环境(Java Native Interface Environment）

//第二个参数 jclass 或 jobject
//声明Native方法是静态方法时，对应参数jclass <类名>
//声明的Native方法时非静态方法时，对应参数是jobject <对象>
extern "C"
{
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved);

JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_nativeSetContext(JNIEnv *env, jclass clazz,
                                                jobject context);

JNIEXPORT jstring JNICALL
Java_com_coocaa_socket_UtilJni_stringFromJNI(JNIEnv *env, jclass clazz);

JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_tcpFromJNI(JNIEnv *env, jclass clazz, jstring ip, jint port);


JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_sendString(JNIEnv *env, jclass clazz, jstring message);

JNIEXPORT void JNICALL
Java_com_coocaa_socket_UtilJni_sendBytes(JNIEnv *env, jclass clazz, jbyteArray byteArray);


}
#endif //JAVA_TO_CPP_H