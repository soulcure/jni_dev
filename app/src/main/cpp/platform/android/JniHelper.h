#ifndef _JNI_HELPER_H_
#define _JNI_HELPER_H_

#include <jni.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>


typedef struct JniMethodInfo_ {
    JNIEnv *env;
    jclass classID;
    jmethodID methodID;
} JniMethodInfo;


class JniHelper {
public:
    static jmethodID _methodID;
    static jobject _classLoader;

    static void ConReceivePdu(const char *buf, int len);

    static void setJavaVM(JavaVM *javaVM);

    static JavaVM *getJavaVM();

    static JNIEnv *getEnv();

    static bool setClassLoaderFrom(jobject context);

private:
    static JavaVM *_psJavaVM;
    static jobject _context;
    typedef std::unordered_map<JNIEnv *, std::vector<jobject>> LocalRefMapType;

    static std::function<void()> classloaderCallback;

    static JNIEnv *cacheEnv(JavaVM *jvm);

    static jobject getContext();

    static jclass _getClassID(const char *className);

    static void _detachCurrentThread(void *p);

    static bool getStaticMethodInfo(JniMethodInfo &methodInfo,
                                    const char *className,
                                    const char *methodName,
                                    const char *paramCode);


    static bool getMethodInfo(JniMethodInfo &methodInfo,
                              const char *className,
                              const char *methodName,
                              const char *paramCode);

    static bool getMethodInfo_DefaultClassLoader(JniMethodInfo &methodInfo,
                                                 const char *className,
                                                 const char *methodName,
                                                 const char *paramCode);

    static void deleteLocalRefs(JNIEnv *env, LocalRefMapType &localRefs);

    static void reportError(const std::string &className, const std::string &methodName,
                            const std::string &signature);
};


#endif // _JNI_HELPER_H_