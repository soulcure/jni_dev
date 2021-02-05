#include "JniHelper.h"
#include <android/log.h>
#include <string>
#include <pthread.h>
#include "../../log/log_util.h"


#define  CLASS_NAME "com/coocaa/socket/UtilJni"

static pthread_key_t g_key;

JavaVM *JniHelper::_psJavaVM = nullptr;
jmethodID JniHelper::_methodID = nullptr;
jobject JniHelper::_classLoader = nullptr;
jobject JniHelper::_context = nullptr;
std::function<void()> JniHelper::classloaderCallback = nullptr;

void JniHelper::ConReceivePdu(const char *buf, int len) {
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


JavaVM *JniHelper::getJavaVM() {
    return _psJavaVM;
}

void JniHelper::setJavaVM(JavaVM *javaVM) {
    _psJavaVM = javaVM;
    pthread_key_create(&g_key, _detachCurrentThread);
}

JNIEnv *JniHelper::cacheEnv(JavaVM *jvm) {
    JNIEnv *_env = nullptr;
    // get jni environment
    jint ret = jvm->GetEnv((void **) &_env, JNI_VERSION_1_6);

    switch (ret) {
        case JNI_OK :
            // Success!
            pthread_setspecific(g_key, _env);
            return _env;

        case JNI_EDETACHED :
            // Thread not attached
            if (jvm->AttachCurrentThread(&_env, nullptr) < 0) {
                LOGE("Failed to get the environment using AttachCurrentThread()");

                return nullptr;
            } else {
                // Success : Attached and obtained JNIEnv!
                pthread_setspecific(g_key, _env);
                return _env;
            }

        case JNI_EVERSION :
            // Cannot recover from this error
            LOGE("JNI interface version 1.4 not supported");
        default :
            LOGE("Failed to get the environment using GetEnv()");
            return nullptr;
    }
}

JNIEnv *JniHelper::getEnv() {
    JNIEnv *_env = (JNIEnv *) pthread_getspecific(g_key);
    if (_env == nullptr)
        _env = JniHelper::cacheEnv(_psJavaVM);
    return _env;
}

jobject JniHelper::getContext() {
    return _context;
}

bool JniHelper::setClassLoaderFrom(jobject content) {
    JniMethodInfo jniMethodInfo;
    if (!JniHelper::getMethodInfo_DefaultClassLoader(jniMethodInfo,
                                                     "android/content/Context",
                                                     "getClassLoader",
                                                     "()Ljava/lang/ClassLoader;")) {
        return false;
    }

    jobject obj = JniHelper::getEnv()->CallObjectMethod(content, jniMethodInfo.methodID);

    if (obj == nullptr) {
        return false;
    }

    JniMethodInfo _m;
    if (!JniHelper::getMethodInfo_DefaultClassLoader(_m,
                                                     "java/lang/ClassLoader",
                                                     "loadClass",
                                                     "(Ljava/lang/String;)Ljava/lang/Class;")) {
        return false;
    }

    JniHelper::_classLoader = JniHelper::getEnv()->NewGlobalRef(obj);
    JniHelper::_methodID = _m.methodID;
    JniHelper::_context = JniHelper::getEnv()->NewGlobalRef(content);
    if (JniHelper::classloaderCallback != nullptr) {
        JniHelper::classloaderCallback();
    }

    return true;
}

bool JniHelper::getStaticMethodInfo(JniMethodInfo &methodInfo,
                                    const char *className,
                                    const char *methodName,
                                    const char *paramCode) {
    if ((nullptr == className) ||
        (nullptr == methodName) ||
        (nullptr == paramCode)) {
        return false;
    }

    JNIEnv *env = JniHelper::getEnv();
    if (!env) {
        LOGE("Failed to get JNIEnv");
        return false;
    }

    jclass classID = _getClassID(className);
    if (!classID) {
        LOGE("Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetStaticMethodID(classID, methodName, paramCode);
    if (!methodID) {
        LOGE("Failed to find static method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodInfo.classID = classID;
    methodInfo.env = env;
    methodInfo.methodID = methodID;
    return true;
}

bool JniHelper::getMethodInfo_DefaultClassLoader(JniMethodInfo &methodInfo,
                                                 const char *className,
                                                 const char *methodName,
                                                 const char *paramCode) {
    if ((nullptr == className) ||
        (nullptr == methodName) ||
        (nullptr == paramCode)) {
        return false;
    }

    JNIEnv *env = JniHelper::getEnv();
    if (!env) {
        return false;
    }

    jclass classID = env->FindClass(className);
    if (!classID) {
        LOGE("Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetMethodID(classID, methodName, paramCode);
    if (!methodID) {
        LOGE("Failed to find method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodInfo.classID = classID;
    methodInfo.env = env;
    methodInfo.methodID = methodID;

    return true;
}

bool JniHelper::getMethodInfo(JniMethodInfo &methodInfo,
                              const char *className,
                              const char *methodName,
                              const char *paramCode) {
    if ((nullptr == className) ||
        (nullptr == methodName) ||
        (nullptr == paramCode)) {
        return false;
    }

    JNIEnv *env = JniHelper::getEnv();
    if (!env) {
        return false;
    }

    jclass classID = _getClassID(className);
    if (!classID) {
        LOGE("Failed to find class %s", className);
        env->ExceptionClear();
        return false;
    }

    jmethodID methodID = env->GetMethodID(classID, methodName, paramCode);
    if (!methodID) {
        LOGE("Failed to find method id of %s", methodName);
        env->ExceptionClear();
        return false;
    }

    methodInfo.classID = classID;
    methodInfo.env = env;
    methodInfo.methodID = methodID;

    return true;
}


void JniHelper::deleteLocalRefs(JNIEnv *env, LocalRefMapType &localRefs) {
    if (!env) {
        return;
    }

    for (const auto &ref : localRefs[env]) {
        env->DeleteLocalRef(ref);
    }
    localRefs[env].clear();
}

void JniHelper::reportError(const std::string &className, const std::string &methodName,
                            const std::string &signature) {
    LOGE("Failed to find static java method. Class name: %s, method name: %s, signature: %s ",
         className.c_str(), methodName.c_str(), signature.c_str());
}


jclass JniHelper::_getClassID(const char *className) {
    if (nullptr == className) {
        return nullptr;
    }

    JNIEnv *env = JniHelper::getEnv();
    jstring _jClassName = env->NewStringUTF(className);
    auto _clazz = (jclass) env->CallObjectMethod(JniHelper::_classLoader,
                                                 JniHelper::_methodID,
                                                 _jClassName);
    if (nullptr == _clazz) {
        LOGE("Classloader failed to find class of %s", className);
        env->ExceptionClear();
    }

    env->DeleteLocalRef(_jClassName);

    return _clazz;
}

void JniHelper::_detachCurrentThread(void *p) {
    JniHelper::getJavaVM()->DetachCurrentThread();
}





