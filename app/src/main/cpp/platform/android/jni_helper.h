#ifndef __ANDROID_JNI_HELPER_H__
#define __ANDROID_JNI_HELPER_H__

#include <jni.h>
#include <string>
#include "../../log/log_util.h"

typedef struct JniMethodInfo_ {
    JNIEnv *env;
    jclass classID;
    jmethodID methodID;
} JniMethodInfo;


class JniHelper {
public:
    static JavaVM *getJavaVM();

    static void setJavaVM(JavaVM *javaVM);

    static void ConReceivePdu(const char *buf, int len);

private:
    static JavaVM *s_JavaVM;
};

#endif // __ANDROID_JNI_HELPER_H__
