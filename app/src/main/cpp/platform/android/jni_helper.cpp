#include "jni_helper.h"
#include <android/log.h>
#include <string>


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

    JniHelper::setJNIEnv(pEnv);
    jclass classID = pEnv->FindClass("com/coocaa/socket/UtilJni");
    //返回java版本
    return JNI_VERSION_1_6;
}


static bool getEnv(JNIEnv **env) {
    auto envStatus = JniHelper::getJavaVM()->GetEnv((void **) env, JNI_VERSION_1_6);
    auto threadStatus = JniHelper::getJavaVM()->AttachCurrentThread(env, nullptr);
    LOGE("getEnv envStatus=[%d]", envStatus);
    LOGE("getEnv threadStatus=[%d]", threadStatus);

    return envStatus == JNI_OK && threadStatus >= 0;
}

//static jclass getClassID_(const char *className, JNIEnv *env) {
//    if (env && getEnv(&env)) {
//        return env->FindClass(className);
//    }
//    LOGE("Failed to find getClass id of %s", className);
//    return nullptr;
//}
static jclass getClassID_(const char *className, JNIEnv *env) {
    JNIEnv *pEnv = env;
    jclass ret = 0;
    do {
        if (!pEnv) {
            if (!getEnv(&pEnv)) {
                break;
            }
        }

        ret = pEnv->FindClass(className);
        if (!ret) {
            LOGD("Failed to find class of %s", className);
            break;
        }
    } while (0);

    return ret;
}


static bool getStaticMethodInfo_(JniMethodInfo &methodInfo, const char *className,
                                 const char *methodName, const char *paramCode) {
    jmethodID methodID = 0;
    JNIEnv *pEnv = 0;
    bool bRet = false;
    do {
        if (!getEnv(&pEnv)) {
            break;
        }

        jclass classID = getClassID_(className, pEnv);

        methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);
        if (!methodID) {
            LOGD("Failed to find static method id of %s", methodName);
            break;
        }

        methodInfo.classID = classID;
        methodInfo.env = pEnv;
        methodInfo.methodID = methodID;

        bRet = true;
    } while (0);

    return bRet;
}

static bool getMethodInfo_(JniMethodInfo &methodInfo, const char *className,
                           const char *methodName, const char *paramCode) {
    jmethodID methodID;
    JNIEnv *pEnv = nullptr;
    if (getEnv(&pEnv)) {
        jclass classID = getClassID_(className, pEnv);
        methodID = pEnv->GetMethodID(classID, methodName, paramCode);
        if (methodID) {
            methodInfo.classID = classID;
            methodInfo.env = pEnv;
            methodInfo.methodID = methodID;
            return true;
        }
    }
    LOGE("Failed to find method id of %s", methodName);
    return false;

}

static std::string jString2string_(jstring jStr) {
    JNIEnv *env = nullptr;
    jboolean isCopy;
    if (getEnv(&env)) {
        const char *chars = env->GetStringUTFChars(jStr, &isCopy);
        std::string ret(chars);
        if (isCopy) {
            env->ReleaseStringUTFChars(jStr, chars);
        }
        return ret;
    }
    return nullptr;
}
}


JavaVM *JniHelper::s_JavaVM = nullptr;
JNIEnv *JniHelper::s_pEnv = nullptr;

JNIEnv *JniHelper::getJNIEnv() {
    return s_pEnv;
}

void JniHelper::setJNIEnv(JNIEnv *pEnv) {
    s_pEnv = pEnv;
}

JavaVM *JniHelper::getJavaVM() {
    return s_JavaVM;
}

void JniHelper::setJavaVM(JavaVM *javaVM) {
    s_JavaVM = javaVM;
}

jclass JniHelper::getClassID(const char *className, JNIEnv *env) {
    return getClassID_(className, env);
}

bool JniHelper::getStaticMethodInfo(JniMethodInfo &methodInfo, const char *className,
                                    const char *methodName, const char *paramCode) {
    return getStaticMethodInfo_(methodInfo, className, methodName, paramCode);
}

bool JniHelper::getMethodInfo(JniMethodInfo &methodInfo, const char *className,
                              const char *methodName, const char *paramCode) {
    return getMethodInfo_(methodInfo, className, methodName, paramCode);
}

std::string JniHelper::jString2string(jstring str) {
    return jString2string_(str);
}
