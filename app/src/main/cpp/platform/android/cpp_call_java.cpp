//
// Created by chenqiongyao on 2021/2/1.
//
#include <jni.h>
#include "jni_helper.h"
#include "cpp_call_java.h"


void ConReceivePdu(const char *buf, int len) {
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "onReceivePdu", "([B)V")) {
        jbyteArray byteArray = t.env->NewByteArray(len);
        if (buf != nullptr) {
            t.env->SetByteArrayRegion(byteArray, 0, len, (jbyte *) buf);
        }
        t.env->CallStaticVoidMethod(t.classID, t.methodID, byteArray);
        t.env->DeleteLocalRef(byteArray);
        t.env->DeleteLocalRef(t.classID);
    }
}

