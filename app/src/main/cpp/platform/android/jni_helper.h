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

    static JNIEnv * getJNIEnv();

    static void setJNIEnv(JNIEnv *pEnv);

    static JavaVM *getJavaVM();

    static void setJavaVM(JavaVM *javaVM);

    static jclass getClassID(const char *className, JNIEnv *env = 0);

    static bool getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className,
                                    const char *methodName, const char *paramCode);

    static bool getMethodInfo(JniMethodInfo &methodinfo, const char *className,
                              const char *methodName, const char *paramCode);

    static std::string jString2string(jstring str);

private:
    static JavaVM *s_JavaVM;
    static JNIEnv *s_pEnv;;
};

#endif // __ANDROID_JNI_HELPER_H__
