#include "jni_helper.h"
#include <android/log.h>
#include <string>

#define  CLASS_NAME "com/coocaa/socket/UtilJni"

extern "C"
{

//////////////////////////////////////////////////////////////////////////
// java vm helper function
//////////////////////////////////////////////////////////////////////////
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JniHelper::setJavaVM(vm);
    LOGD("JNI_OnLoad be called...");

    JNIEnv *pEnv = nullptr;
    //获取环境
    jint ret = vm->GetEnv((void **) &pEnv, JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        LOGE("jni_helper JNI_OnLoad ERROR...");
        return JNI_ERR;
    }
    //返回java版本
    return JNI_VERSION_1_6;
}

// get env and cache it
static JNIEnv *getJNIEnv() {

    JavaVM *jvm = JniHelper::getJavaVM();
    if (NULL == jvm) {
        LOGD("Failed to get JNIEnv. JniHelper::getJavaVM() is NULL");
        return NULL;
    }

    JNIEnv *env = NULL;
    // get jni environment
    jint ret = jvm->GetEnv((void **) &env, JNI_VERSION_1_4);

    switch (ret) {
        case JNI_OK :
            // Success!

            return env;

        case JNI_EDETACHED :
            // Thread not attached
            if (jvm->AttachCurrentThread(&env, NULL) < 0) {
                LOGD("Failed to get the environment using AttachCurrentThread()");
                return NULL;
            } else {
                // Success : Attached and obtained JNIEnv!
                return env;
            }

        case JNI_EVERSION :

            // Cannot recover from this error
            LOGD("JNI interface version 1.4 not supported");
        default :

            LOGD("Failed to get the environment using GetEnv()");
            return NULL;
    }
}

// get class and make it a global reference, release it at endJni().
static jclass getClassID(JNIEnv *pEnv) {
    jclass ret = pEnv->FindClass(CLASS_NAME);
    if (!ret) {
        LOGD("Failed to find class of %s", CLASS_NAME);
    }
    return ret;
}

static bool getStaticMethodInfo(JniMethodInfo &methodinfo, const char *methodName,
                                const char *paramCode) {
    jmethodID methodID = 0;
    JNIEnv *pEnv = 0;
    bool bRet = false;

    do {
        pEnv = getJNIEnv();
        if (!pEnv) {
            break;
        }

        jclass classID = getClassID(pEnv);

        methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);

        if (!methodID) {
            LOGD("Failed to find static method id of %s", methodName);
            break;
        }
        methodinfo.classID = classID;
        methodinfo.env = pEnv;
        methodinfo.methodID = methodID;

        bRet = true;
    } while (0);
    return bRet;


}


void _ConReceivePdu(const char *buf, int len) {
    JniMethodInfo t;
    if (getStaticMethodInfo(t, "onReceivePdu", "([B)V")) {
        jbyteArray byteArray = t.env->NewByteArray(len);
        if (buf != nullptr) {
            t.env->SetByteArrayRegion(byteArray, 0, len, (jbyte *) buf);
        }
        t.env->CallStaticVoidMethod(t.classID, t.methodID, byteArray);
        t.env->DeleteLocalRef(byteArray);
        t.env->DeleteLocalRef(t.classID);
    }
}


}


JavaVM *JniHelper::s_JavaVM = nullptr;

JavaVM *JniHelper::getJavaVM() {
    return s_JavaVM;
}

void JniHelper::setJavaVM(JavaVM *javaVM) {
    s_JavaVM = javaVM;
}

void JniHelper::ConReceivePdu(const char *buf, int len) {
    _ConReceivePdu(buf, len);
}
